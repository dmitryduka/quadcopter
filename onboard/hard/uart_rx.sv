module uart_rx #(parameter BIT_TIME = 40) ( input         CLK,
                                            input         RESET,
                  
                                            output [7:0]  DATA,
                                            output        EN,
//                                            output        ERROR,
                  
                                            input         RX );

// The module uses positive logic:
// RX is HIGH at IDLE and MARK
// RX is LOW at SPACE
// And please, don't change polarity inside the module!
// BIT_TIME should be even, by the way.

//----------------DEFINITIONS-----------------------                  
enum int unsigned { ST_IDLE       = 0, 
                    ST_RESYNC     = 1, 
                    ST_DATA_BIT   = 2, 
                    ST_STOP_BIT   = 3 } state, next;

wire [13:0] bit_count_out;
wire  [2:0] byte_count_out;
wire rx_sync, rx_posedge, rx_negedge;
wire rx_transition = rx_posedge | rx_negedge;

wire period       = (bit_count_out == BIT_TIME);
wire half_period  = (bit_count_out == BIT_TIME/2);
wire byte_done    = (byte_count_out == 7);

wire shift_in     = ((state == ST_DATA_BIT) && (period || rx_transition));
wire resync       = ((state == ST_RESYNC) && half_period);
wire idle         = (state == ST_IDLE);
assign EN         = ((state == ST_STOP_BIT) && period);

//----------------TRANSITION LOGIC-------------------
always_comb begin
  next = ST_IDLE;
    case(state)
    ST_IDLE:        if(rx_negedge)        next = ST_RESYNC;
                    else                  next = ST_IDLE;
                  
    ST_RESYNC:      if(rx_posedge)        next = ST_IDLE;     //omit broken frames
                    else if(half_period)  next = ST_DATA_BIT;
                    else                  next = ST_RESYNC;
                  
    ST_DATA_BIT:    if(byte_done & ( period | rx_transition)) 
                                          next = ST_STOP_BIT;
                    else if(rx_transition)
                                          next = ST_RESYNC;
                    else                  next = ST_DATA_BIT;
                  
    ST_STOP_BIT:   if(period)             next = ST_IDLE;
                   else                   next = ST_STOP_BIT;
    endcase
end                

always_ff @(posedge CLK)
  if(RESET) state <= ST_IDLE;
  else      state <= next;

sync syncronizer(CLK, RX, rx_sync);
edetect edge_det(CLK, rx_sync, rx_posedge, rx_negedge);

counter #(14) bit_cnt(CLK, idle | period | resync | ((state != ST_STOP_BIT) & rx_transition), 1'b1, bit_count_out );
counter #(3) byte_cnt(CLK, idle, shift_in, byte_count_out);
shift_reg_lf #(8) sreg(CLK, idle, rx_sync, shift_in, DATA);
                  
endmodule
