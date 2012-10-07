//===============================================//
module radio_rx(  input         CLK_1M,
                  input         RESET,
                  input         RADIO_IN,
                  output [31:0] DATA_OUT );


wire sync_out, clean_out, edge_p, edge_n, online;
wire [11:0] pulse_width;

sync         smodule(CLK_1M, RADIO_IN, sync_out);
hystheresis  hmodule(CLK_1M, RESET, sync_out, clean_out);                  
edetect      emodule(CLK_1M, clean_out, edge_p, edge_n);
radio_rx_sm  rmodule(CLK_1M, RESET, edge_p, edge_n, online, pulse_width);

assign DATA_OUT = {~online, 19'd0, pulse_width};
                  
endmodule
//===============================================//
module radio_rx_sm( input         CLK,
                    input         RESET,
                    input         EDGE_P,
                    input         EDGE_N,                   
                    output        ONLINE,
                    output [11:0] PULSE_WIDTH );

parameter ST_WAIT   = 0;  
parameter ST_COUNT  = 1;
parameter ST_UPDATE = 2;
parameter ST_IDLE   = 3;  
                    
reg [ 1:0] state = 0, nextstate;
reg [ 4:0] control;
reg [15:0] wcnt;
reg [11:0] pcnt, period;

wire timeout = wcnt[15];
wire reset_period, reset_to, count_period, count_to, update_p;

always@(*)
  case(state)
  ST_WAIT:  if(EDGE_P)        nextstate = ST_COUNT;
            else              nextstate = ST_WAIT;
              
  ST_COUNT: if(EDGE_N)        nextstate = ST_UPDATE;
            else              nextstate = ST_COUNT;

  ST_UPDATE:                  nextstate = ST_IDLE;
              
  ST_IDLE:  if(EDGE_P)        nextstate = ST_COUNT;
            else if(timeout)  nextstate = ST_WAIT;
            else              nextstate = ST_IDLE;
              
  default:                    nextstate = ST_WAIT;
  endcase
 
 assign {reset_period, reset_to, count_period, count_to, update_p} = control;
 always@(*)              //PTPTU
  case(state)            //RRCCU
  ST_WAIT:    control = 5'b11000;
  ST_COUNT:   control = 5'b01100;
  ST_UPDATE:  control = 5'b01001;
  ST_IDLE:    control = 5'b10010;
  default:    control = 5'b11000;
  endcase 
                    
always@(posedge CLK)
  if(RESET)
    begin
    state <= 2'd0;
    wcnt <= 16'd0;
    pcnt <= 12'd0;
    period <= 12'd0;
    end
  else
    begin
    state <= nextstate;
    
    if(update_p) period <= pcnt;
    
    if(reset_period) 
      pcnt <= 12'd0;
    else if(count_period && (pcnt != 12'hFFF))
      pcnt <= pcnt + 12'd1;

    if(reset_to) 
      wcnt <= 16'd0;
    else if(count_to && (wcnt != 16'hFFFF))
      wcnt <= wcnt + 16'd1;
    
    end

assign PULSE_WIDTH = period;
assign ONLINE = (state != ST_WAIT);
                    
endmodule
//===============================================//
