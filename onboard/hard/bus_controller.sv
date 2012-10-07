module bus_controller(  input  [29:0]  CPU_ADDR,
                        input          CPU_WE,
                        input  [03:0]  CPU_BE,
                        input  [31:0]  CPU_WD,
                        output [31:0]  CPU_RD,
               
                        output [29:0]  STACK_MEM_A,
                        output         STACK_MEM_WE,
                        output [03:0]  STACK_MEM_BE,
                        output [31:0]  STACK_MEM_WD,
                        input  [31:0]  STACK_MEM_RD,
                        
                        output [29:0]  CODE_MEM_A,
                        output         CODE_MEM_WE,
                        output [31:0]  CODE_MEM_WD,
                        input  [31:0]  CODE_MEM_RD,

                        output         JTAG_WE,
                        output [31:0]  JTAG_WD,
                        input  [31:0]  JTAG_RD,
                        
                        output         ADC_WE,
                        output [31:0]  ADC_WD,
                        input  [31:0]  ADC_RD,
                        
                        output [31:0]  IMU_WD,
                        input  [31:0]  IMU_RD,
                        output         IMU_WE,
                        
                        output [31:0]  UART_TX_WD, 
                        input  [31:0]  UART_TX_RD,
                        output         UART_TX_WE,
                        
                        input  [31:0]  UART_RX_RD,
                        output         UART_RX_WE,
                        
                        input  [31:0]  RTC,
                        
                        output [31:0]  ENGINES_13_WD,
                        output         ENGINES_13_WE,

                        output [31:0]  ENGINES_24_WD,
                        output         ENGINES_24_WE,
                        
                        input  [31:0]  RADIO_CH1,
                        input  [31:0]  RADIO_CH2,
                        input  [31:0]  RADIO_CH3,
                        input  [31:0]  RADIO_CH4,
                        input  [31:0]  RADIO_CH5,
                        input  [31:0]  RADIO_CH6,
                        
                        input  [31:0]  USER_IO,
                        
                        output         LEDS_WE,
                        output [31:0]  LEDS_WD  );

wire [31:0] IO_MEM_RD;
wire        IO_MEM_WE;

aspace_mux address_space_mux( .SEL         ( CPU_ADDR[29:28] ),
                              .CPU_WE      ( CPU_WE          ),
                              .CPU_RD      ( CPU_RD          ),
                              .DATA_SEG    ( STACK_MEM_RD    ),
                              .DATA_SEG_WE ( STACK_MEM_WE    ),
                              .CODE_SEG    ( CODE_MEM_RD     ),
                              .CODE_SEG_WE ( CODE_MEM_WE     ),
                              .IO_SEG      ( IO_MEM_RD       ),
                              .IO_SEG_WE   ( IO_MEM_WE       ) );

iospace_mux inout_space_mux(  .SEL         ( CPU_ADDR[3:0] ),        
                              .IO_SEG_WE   ( IO_MEM_WE     ),
                              .IO_SEG      ( IO_MEM_RD     ),
                      
                              .IO_0_RD     ( JTAG_RD       ),
                              .IO_0_WE     ( JTAG_WE       ),
                      
                              .IO_1_RD     ( USER_IO       ),
                              .IO_1_WE     ( LEDS_WE       ),
                              
                              .IO_2_RD     ( ADC_RD        ), 
                              .IO_2_WE     ( ADC_WE        ),
                              
                              .IO_3_RD     ( IMU_RD        ),
                              .IO_3_WE     ( IMU_WE        ),
                              
                              .IO_4_RD     ( RTC           ),
                              
                              .IO_5_RD     ( UART_TX_RD    ),
                              .IO_5_WE     ( UART_TX_WE    ),
                              
                              .IO_6_RD     ( UART_RX_RD    ),
                              .IO_6_WE     ( UART_RX_WE    ),
                              
                              .IO_7_RD     ( 32'hEE000012  ),
                              .IO_7_WE     ( ENGINES_13_WE ),
                              
                              .IO_8_RD     ( 32'hEE000024  ),
                              .IO_8_WE     ( ENGINES_24_WE ),
                              
                              .IO_9_RD     ( 32'd9         ),
                              
                              .IO_10_RD    ( RADIO_CH1     ),
                              .IO_11_RD    ( RADIO_CH2     ),
                              .IO_12_RD    ( RADIO_CH3     ),
                              .IO_13_RD    ( RADIO_CH4     ),
                              .IO_14_RD    ( RADIO_CH5     ),
                              .IO_15_RD    ( RADIO_CH6     ) );
                              
assign STACK_MEM_A   = CPU_ADDR;
assign STACK_MEM_WD  = CPU_WD;

assign CODE_MEM_A    = CPU_ADDR;
assign CODE_MEM_WD   = CPU_WD;

assign ADC_WD        = CPU_WD;
assign LEDS_WD       = CPU_WD;
assign JTAG_WD       = CPU_WD;
assign IMU_WD        = CPU_WD;
assign UART_TX_WD    = CPU_WD;
assign ENGINES_13_WD = CPU_WD;
assign ENGINES_24_WD = CPU_WD;


assign STACK_MEM_BE  = CPU_BE;

endmodule
//================================================================//
module aspace_mux (  input   [1:0] SEL,
                     input         CPU_WE,
                     output [31:0] CPU_RD,
                     
                     input  [31:0] DATA_SEG,
                     output        DATA_SEG_WE,
                     
                     input  [31:0] CODE_SEG,
                     output        CODE_SEG_WE,
                     
                     input  [31:0] IO_SEG,
                     output        IO_SEG_WE );
                     

logic [34:0] ctrl;

always_comb
   case(SEL)
   2'b00:   ctrl = {3'b001, IO_SEG};   //IO  : ADDR[31:30] == 2'b00
   2'b01:   ctrl = {3'b010, CODE_SEG}; //CODE: ADDR[31:30] == 2'b01
   default: ctrl = {3'b100, DATA_SEG}; //DATA: ADDR[31:30] == 2'b1X
   endcase

assign CPU_RD = ctrl[31:0];
assign DATA_SEG_WE = CPU_WE & ctrl[34];
assign CODE_SEG_WE = CPU_WE & ctrl[33];
assign IO_SEG_WE   = CPU_WE & ctrl[32];

endmodule
//================================================================//
module iospace_mux (  input   [3:0] SEL,        
                      input         IO_SEG_WE,
                      output [31:0] IO_SEG,
                      
                      input  [31:0] IO_0_RD,
                      output        IO_0_WE,
                      
                      input  [31:0] IO_1_RD,
                      output        IO_1_WE,

                      input  [31:0] IO_2_RD,
                      output        IO_2_WE,

                      input  [31:0] IO_3_RD,
                      output        IO_3_WE,
                     
                      input  [31:0] IO_4_RD,
                      output        IO_4_WE,
                      
                      input  [31:0] IO_5_RD,
                      output        IO_5_WE,

                      input  [31:0] IO_6_RD,
                      output        IO_6_WE,

                      input  [31:0] IO_7_RD,
                      output        IO_7_WE,
                    
                      input  [31:0] IO_8_RD,
                      output        IO_8_WE,
                      
                      input  [31:0] IO_9_RD,
                      output        IO_9_WE,

                      input  [31:0] IO_10_RD,
                      output        IO_10_WE,

                      input  [31:0] IO_11_RD,
                      output        IO_11_WE,
                     
                      input  [31:0] IO_12_RD,
                      output        IO_12_WE,
                      
                      input  [31:0] IO_13_RD,
                      output        IO_13_WE,

                      input  [31:0] IO_14_RD,
                      output        IO_14_WE,

                      input  [31:0] IO_15_RD,
                      output        IO_15_WE  );

logic [15:0] wes;
logic [31:0] out;

always_comb
   begin
   wes = '0;
   wes[SEL] = IO_SEG_WE;
   end

always_comb
   case(SEL)
   4'd0:  out =  IO_0_RD;
   4'd1:  out =  IO_1_RD;
   4'd2:  out =  IO_2_RD;
   4'd3:  out =  IO_3_RD;
   4'd4:  out =  IO_4_RD;
   4'd5:  out =  IO_5_RD;
   4'd6:  out =  IO_6_RD;
   4'd7:  out =  IO_7_RD;
   4'd8:  out =  IO_8_RD;
   4'd9:  out =  IO_9_RD;
   4'd10: out = IO_10_RD;
   4'd11: out = IO_11_RD;
   4'd12: out = IO_12_RD;
   4'd13: out = IO_13_RD;
   4'd14: out = IO_14_RD;
   4'd15: out = IO_15_RD;
   endcase
   
assign IO_0_WE = wes[0];
assign IO_1_WE = wes[1];
assign IO_2_WE = wes[2];
assign IO_3_WE = wes[3];
assign IO_4_WE = wes[4];
assign IO_5_WE = wes[5];
assign IO_6_WE = wes[6];
assign IO_7_WE = wes[7];
assign IO_8_WE = wes[8];
assign IO_9_WE = wes[9];
assign IO_10_WE = wes[10];
assign IO_11_WE = wes[11];
assign IO_12_WE = wes[12];
assign IO_13_WE = wes[13];
assign IO_14_WE = wes[14];
assign IO_15_WE = wes[15];

assign IO_SEG = out;

endmodule
