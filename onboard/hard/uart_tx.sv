module uart_tx #(parameter BIT_TIME = 40) ( input       CLK,
                                            input       RESET,
                
                                            input [7:0] DATA,
                                            input       EN,
                                            output      READY,
                
                                            output      TX );
                
                
//----------------DEFINITIONS-----------------------
enum int unsigned { ST_READY = 0, 
                    ST_START_BIT = 1, 
                    ST_DATA_BIT = 2, 
                    ST_STOP_BIT  = 3 } state, next;

wire [13:0] bit_count_out;
wire  [2:0] byte_count_out;
logic [7:0] sreg;
logic       tx_r;

wire bit_done        = (BIT_TIME == bit_count_out);
wire byte_increment  = (state == ST_DATA_BIT) & bit_done;
wire byte_done       = (3'd7  == byte_count_out);
wire ready           = (state == ST_READY);


//----------------TRANSITION LOGIC-------------------
always_comb begin
  next = ST_READY;
    case(state)
    ST_READY:      if(EN)                next = ST_START_BIT;
                   else                  next = ST_READY;
                  
    ST_START_BIT:  if(bit_done)          next = ST_DATA_BIT;
                   else                  next = ST_START_BIT;
                  
    ST_DATA_BIT:   if(bit_done & byte_done) next = ST_STOP_BIT;
                   else                  next = ST_DATA_BIT;
                  
    ST_STOP_BIT:   if(bit_done)          next = ST_READY;
                   else                  next = ST_STOP_BIT;
    endcase
end                


//---------------SM_OUTPUT LOGIC-----------------------
wire reset_counters, sm_output, output_val;
logic [2:0] controls;
assign {reset_counters, sm_output, output_val} = controls;

                
always_comb begin
  controls = 3'b111;
    case(state)             //RSO
    ST_READY:      controls = 3'b111;
    ST_START_BIT:  controls = 3'b010;
    ST_DATA_BIT:   controls = 3'b00X;
    ST_STOP_BIT:   controls = 3'b011;
    endcase
end

//---------------CLOCKED LOGIC------------------------
always_ff @(posedge CLK)
  if(RESET)
    begin
    state <= ST_READY;
    end
  else
    begin
    state <= next;
    tx_r <= sm_output ? output_val : sreg[0];
    
    if(ready)
      begin
      if(EN) sreg <= DATA;
      end
    else if(bit_done && (state == ST_DATA_BIT)) 
      begin
      sreg <= {1'b0, sreg[7:1]};
      end
    end


assign TX = tx_r;
assign READY = ready;

counter #(14) bit_counter (CLK, reset_counters | bit_done, 1'b1, bit_count_out);
counter #(3)  byte_counter(CLK, reset_counters, byte_increment, byte_count_out);
                
endmodule
//=========================================================================//

