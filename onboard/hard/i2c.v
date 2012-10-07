module i2c_cpu( input         CLK,
				input		  CLK_I2C,
                input         RESET,
                
                input         WE,
                input  [31:0] DATA_IN,
                output [31:0] DATA_OUT,
                
                inout         SDA,
                inout         SCL );

wire ready, active;
reg  [8:0] datain;
wire [8:0] dataout;

reg [1:0] state = 0, nextstate;
reg req_start, req_stop, req_io;
wire req_en = (state == 2'd1);

always@(*)
  case(state)
  2'd0: if(WE)     nextstate = 2'd1;
        else       nextstate = 2'd0;
  
  2'd1: if(!ready) nextstate = 2'd2;
        else       nextstate = 2'd1;
          
  2'd2: if(ready)  nextstate = 2'd0;
        else       nextstate = 2'd2;
          
  default:         nextstate = 2'd0;
  endcase
  
always@ (posedge CLK)
  if(RESET) state <= 2'd0;
  else
    begin
    state <= nextstate;
    if(WE) 
      begin
      datain <= { DATA_IN[7:0], DATA_IN[8] };
      req_start <= DATA_IN[18];
      req_io    <= DATA_IN[17];
      req_stop  <= DATA_IN[16];
      end
    end

assign DATA_OUT = { state != 2'd0,
                    active,
                    21'd0,
                    dataout[0],
                    dataout[8:1] };
/*
reg CLK_I2C = 0;
reg [6:0] clk_div = 0;

always@ (posedge CLK)
	begin
	if(clk_div == 16)
		begin
		clk_div <= 6'd0;
		CLK_I2C <= ~CLK_I2C;
		end
	else clk_div <= clk_div + 6'd1;
	end
*/

i2c i2c_module( .CLK           ( CLK_I2C            ),
                .RESET         ( RESET              ),

                .DATA_IN       ( datain             ),
                .DATA_OUT      ( dataout            ),
                .REQUEST_IO    ( req_io    & req_en ),
                .REQUEST_START ( req_start & req_en ),
                .REQUEST_STOP  ( req_stop  & req_en ),
                .READY         ( ready              ),
                .ACTIVE        ( active             ),

                .SDA           ( SDA                ),
                .SCL           ( SCL                ) ); 

endmodule
//=================================================================//

module i2c( input         CLK,
            input         RESET,
            
            input   [8:0] DATA_IN,
            output  [8:0] DATA_OUT,
            input         REQUEST_IO,
            input         REQUEST_START,
            input         REQUEST_STOP,
            output        READY,
            output        ACTIVE,            

            inout         SDA,
            inout         SCL );
            

reg [2:0] state = 0, nextstate;
wire sda_w, scl_w, sda_smachine, sda_override,
     shift_in, shift_out;

reg [8:0] datain;
reg [8:0] dataout;
reg [3:0] bit_cnt;

parameter ST_IDLE    = 0;
parameter ST_START   = 1;
parameter ST_WAIT_IO = 2;
parameter ST_SETUP   = 3;
parameter ST_DRIVE   = 4;
parameter ST_CHECK   = 5;
parameter ST_HOLD    = 6;
parameter ST_STOP    = 7;

always@ (*)
  case(state)
  ST_IDLE:    if(REQUEST_START)     nextstate = ST_START; //accept only start request in idle state
              else                  nextstate = ST_IDLE;
              
  ST_START:                         nextstate = ST_WAIT_IO;
                
  ST_WAIT_IO: if     (REQUEST_IO)   nextstate = ST_SETUP;
              else if(REQUEST_STOP) nextstate = ST_STOP;
              else                  nextstate = ST_WAIT_IO;
  
  ST_SETUP:                         nextstate = ST_DRIVE;
  
  ST_DRIVE:                         nextstate = ST_CHECK;
  
  ST_CHECK:                         nextstate = ST_HOLD;
  
  ST_HOLD:  if(bit_cnt == 8)        nextstate = ST_WAIT_IO;
            else                    nextstate = ST_SETUP;
              
  ST_STOP:                          nextstate = ST_IDLE;
  
  default:                          nextstate = ST_IDLE;
  endcase

reg [4:0] controls;
assign {sda_override, sda_smachine, scl_w, shift_in, shift_out} = controls;

always@ (*)
  case(state)               //OM_C_IO
  ST_IDLE:      controls = 5'b11_1_00;
  ST_START:     controls = 5'b10_1_00;
  ST_WAIT_IO:   controls = 5'b10_0_00;
  ST_SETUP:     controls = 5'b0X_0_00;
  ST_DRIVE:     controls = 5'b0X_1_10;
  ST_CHECK:     controls = 5'b0X_1_00;
  ST_HOLD:      controls = 5'b0X_0_01;
  ST_STOP:      controls = 5'b10_1_00;
  default:      controls = 5'b11_1_00;
  endcase            

always@ (posedge CLK)
  begin
  if(RESET)
    begin
    bit_cnt <= 4'd0;
    state <= 4'd0;
    end
  else
    begin
    state <= nextstate;
    
    if(READY)
      begin
      if(REQUEST_IO)
        begin
        dataout <= DATA_IN[8:0];
        datain  <= 8'd0;
        bit_cnt <= 0;
        end
      end
    else
      begin
      if(shift_in) 
        begin
        datain[8:1] <= datain[7:0];
        datain[0] <= SDA;
        end
        
      if(shift_out)
        begin
        dataout[8:1] <= dataout[7:0];
        dataout[0] <= 0;
        bit_cnt <= bit_cnt + 4'd1;
        end
      end
    end
  end
  
assign sda_w = sda_override ? sda_smachine : dataout[8];

assign SDA = sda_w ? 1'bZ : 1'b0;
assign SCL = scl_w ? 1'bZ : 1'b0;

assign READY = ((state == ST_IDLE) | (state == ST_WAIT_IO));
assign ACTIVE = (state != ST_IDLE);
assign DATA_OUT = datain;
            
endmodule
          