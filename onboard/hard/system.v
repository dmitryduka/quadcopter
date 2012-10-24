module system   (  input         CLK,

                  //DE0 ADC
                   output        ADC_CS,
                   output        ADC_SCLK,
                   output        ADC_SDO,
                   input         ADC_SDI,
                   
                  //DE0 USER IO
                   input  [1:0]  BTNS,
                   input  [3:0]  DIP_SW,
                   output [7:0]  LEDS,
                  
                  //ANALOG RADIO RX
                   input  [5:0]  RADIO,
                   
                  //External IMU
                   inout         IMU_SDA,
                   inout         IMU_SCL,
                   
                  //UART
                   input         UART_RX,
                   output        UART_TX,
                   output        XBEE_RESETN,
                   
                  //4-way Engine control
                   output [3:0]  ENGINE_CONTROL,
                   
                  //Debug pins
                   output [1:0]  DEBUG );

//==================WIRE DECLARATIONS============================//
wire CLK_CPU, CLK_MEM, CLK_1M, RESET, RUN, I_SOURCE;

wire [31:0] INST_RD, CPU_RD, CPU_WD, STACK_MEM_WD, STACK_MEM_RD,
            INST_D_RD, INST_D_WD;
wire [29:0] INST_ADDR, CPU_ADDR, STACK_MEM_A, INST_D_A;
wire [3:0]  CPU_BE, STACK_MEM_BE;
wire        CPU_WE, STACK_MEM_WE, INST_D_WE;

//JTAG signals
wire        JTAG_WE;
wire [31:0] JTAG_WD, JTAG_RD, DEBUG_INST, CPU_INSTRUCTION;

//ADC signals
wire        ADC_WE;
wire [31:0] ADC_RD, ADC_WD;

//UART TX signals
wire        UART_TX_WE;
wire [31:0] UART_TX_RD, UART_TX_WD;

//UART RX signals
wire        UART_RX_WE;
wire [31:0] UART_RX_RD;


//USER IO
wire [31:0] USER_IO;

//Led signals
wire [31:0] LEDS_WD;
wire        LEDS_WE;

//Analog radio signals
wire [31:0] RADIO_CH1, RADIO_CH2, RADIO_CH3, 
            RADIO_CH4, RADIO_CH5, RADIO_CH6;

//IMU signals
wire [31:0] IMU_WD, IMU_RD;
wire        IMU_WE;

//RTC signals
wire [31:0] RTC_RD;

//Engine Control signals
wire [31:0] ENGINES_13_WD, ENGINES_24_WD, 
            ENGINES_13_CTRL, ENGINES_24_CTRL;
wire        ENGINES_13_WE, ENGINES_24_WE;

//===================SYSTEM CLOCK================================//

pll main_pll(CLK, CLK_CPU, CLK_MEM, CLK_1M);

//======================THE CPU==================================//

mcpu the_cpu (.CLK         ( CLK_CPU            ),
              .CLK_MEM     ( CLK_MEM            ),
              .RESET       ( RESET              ),
              .RUN         ( RUN                ),
              
              //External inst memory iface
              .INST_ADDR   ( INST_ADDR          ),
              .INST_RD     ( CPU_INSTRUCTION    ),
                 
              //External data memory iface
              .DATA_WE     ( CPU_WE             ),
              .DATA_BE     ( CPU_BE             ),
              .DATA_ADDR   ( CPU_ADDR           ),
              .DATA_WD     ( CPU_WD             ),
              .DATA_RD     ( CPU_RD             ));

//===================INSTRUCTION MEMORY==========================//

imem instr_mem(   .CLK     ( CLK_MEM            ),
                  .DATA_A  ( INST_D_A           ),
                  .DATA_WE ( INST_D_WE          ),
                  .DATA_WD ( INST_D_WD          ),
                  .DATA_RD ( INST_D_RD          ),
              
                  .MAIN_A  ( INST_ADDR          ), 
                  .MAIN_RD ( INST_RD            )); 

//======================STACK SPACE==============================//

dmem stack_mem(   .CLK     ( CLK_MEM            ),
                  .WE      ( STACK_MEM_WE       ),    
                  .BE      ( STACK_MEM_BE       ),
                  .ADDR    ( STACK_MEM_A        ), 
                  .WD      ( STACK_MEM_WD       ),
                  .RD      ( STACK_MEM_RD       ));

jtag jtag_m     ( .CPU_CLK    ( CLK_CPU    ),
               //CPU controls
                  .RESET      ( RESET      ), //cpu reset, 1 -- reset;
                  .RUN        ( RUN        ), //cpu run. 0 - pause, 1 - run. Pulsed in step-by-step
                  .I_SOURCE   ( I_SOURCE   ), //cpu instruction source; 0 for normal mem
               //32bit DEBUG DATA PORT
                  .WE         ( JTAG_WE    ),
                  .WD         ( JTAG_WD    ),
                  .RD         ( JTAG_RD    ),
               //32bit DEBUG INSTR PORT
                  .DEBUG_INST ( DEBUG_INST ),  //cpu instruction from jtag
                  .MEM_INST   ( INST_RD    ),  //current instruction from main mem
                  .INST_ADDR  ( INST_ADDR  )); //cpu inst memory address

assign XBEE_RESETN = ~RESET;
                  
mux2 isrc_mux  (I_SOURCE,  INST_RD, 
                           DEBUG_INST,    CPU_INSTRUCTION );

//=====================BUS CONTROLLER============================//

bus_controller bc(   .CPU_ADDR         ( CPU_ADDR      ),
                     .CPU_WE           ( CPU_WE        ),
                     .CPU_BE           ( CPU_BE        ),
                     .CPU_WD           ( CPU_WD        ),
                     .CPU_RD           ( CPU_RD        ),
                     
                     .STACK_MEM_A      ( STACK_MEM_A   ),
                     .STACK_MEM_BE     ( STACK_MEM_BE  ),
                     .STACK_MEM_WE     ( STACK_MEM_WE  ),
                     .STACK_MEM_WD     ( STACK_MEM_WD  ),
                     .STACK_MEM_RD     ( STACK_MEM_RD  ),
                     
                     .CODE_MEM_A       ( INST_D_A      ),
                     .CODE_MEM_WE      ( INST_D_WE     ),
                     .CODE_MEM_WD      ( INST_D_WD     ),
                     .CODE_MEM_RD      ( INST_D_RD     ),
                     
                     .JTAG_WE          ( JTAG_WE       ),
                     .JTAG_WD          ( JTAG_WD       ),
                     .JTAG_RD          ( JTAG_RD       ),
                     
                     .ADC_WE           ( ADC_WE        ),
                     .ADC_WD           ( ADC_WD        ),
                     .ADC_RD           ( ADC_RD        ),
                     
                     .IMU_WD           ( IMU_WD        ),
                     .IMU_RD           ( IMU_RD        ),
                     .IMU_WE           ( IMU_WE        ),
                     
                     .UART_TX_WD       ( UART_TX_WD    ), 
                     .UART_TX_RD       ( UART_TX_RD    ),
                     .UART_TX_WE       ( UART_TX_WE    ),
                     
                     .UART_RX_RD       ( UART_RX_RD    ),
                     .UART_RX_WE       ( UART_RX_WE    ),

                     .ENGINES_13_WD    ( ENGINES_13_WD ),
                     .ENGINES_13_WE    ( ENGINES_13_WE ),

                     .ENGINES_24_WD    ( ENGINES_24_WD ),
                     .ENGINES_24_WE    ( ENGINES_24_WE ),
                     
                     .RTC              ( RTC_RD        ),
                     
                     .RADIO_CH1        ( RADIO_CH1     ),
                     .RADIO_CH2        ( RADIO_CH2     ),
                     .RADIO_CH3        ( RADIO_CH3     ),
                     .RADIO_CH4        ( RADIO_CH4     ),
                     .RADIO_CH5        ( RADIO_CH5     ),
                     .RADIO_CH6        ( RADIO_CH6     ),
                     
                     .USER_IO          ( USER_IO       ),
                     
                     .LEDS_WE          ( LEDS_WE       ),
                     .LEDS_WD          ( LEDS_WD       ));

//========================LEDS===================================//

io8 led_reg(   .CLK     ( CLK_CPU     ),
               .WE      ( LEDS_WE     ),
               .DATA_IN ( LEDS_WD     ),
               .IO_OUT  ( LEDS        ));

//======================USER INPUT===============================//

io6 user_io(   .CLK_IO  ( CLK_1M      ),
               .BTNS    ( BTNS        ),
               .DIP_SW  ( DIP_SW      ),
               .IO_OUT  ( USER_IO     ));
               
//=========================ADC===================================//

adc_interface the_adc(    .CLK      ( CLK_CPU  ),
                          .CLK_1M   ( CLK_1M   ),

                      //processor interface
                          .DATA_IN  ( ADC_WD   ),
                          .DATA_OUT ( ADC_RD   ),
                          .WR       ( ADC_WE   ),

                      //device interface
                          .CS       ( ADC_CS   ),
                          .SCLK     ( ADC_SCLK ),
                          .SDO      ( ADC_SDO  ),
                          .SDI      ( ADC_SDI  ));

//=====================EXTERNAL IMU==============================//

i2c_cpu   ext_imu(   .CLK      ( CLK_CPU  ),
                     .CLK_I2C  ( CLK_1M   ), //CLK_1M6
                     .RESET    ( RESET    ),
                     
                     .WE       ( IMU_WE   ),
                     .DATA_IN  ( IMU_WD   ),
                     .DATA_OUT ( IMU_RD   ),
                     
                     .SDA      ( IMU_SDA  ),
                     .SCL      ( IMU_SCL  ) ); 

assign DEBUG[0] = IMU_SCL;
assign DEBUG[1] = IMU_SDA;

//==========================UART TX==============================//

buffered_tx the_uart_tx( .CLK     ( CLK_CPU    ),
                         .RESET   ( RESET      ),
                         .DATA_WD ( UART_TX_WD ),
                         .DATA_RD ( UART_TX_RD ),
                         .DATA_WE ( UART_TX_WE ), 
                         .UART_TX ( UART_TX    ) );

//==========================UART RX==============================//

buffered_rx the_uart_rx( .CLK     ( CLK_CPU    ),
                         .RESET   ( RESET      ),
                         .DATA_RD ( UART_RX_RD ),
                         .DATA_WE ( UART_RX_WE ),
                         .RXD     ( UART_RX    ) );                         
                         
                         
//=================SIMPLE REALTIME CLOCK=========================//

rtc   the_rtc (      .CLK      ( CLK_CPU  ),
                     .RESET    ( RESET    ),
                     .DATA_OUT ( RTC_RD   ) );

//=====================RADIO CONTROL=============================//

radio_rx radio_ch_1( .CLK_1M   ( CLK_1M    ),
                     .RESET    ( RESET     ),
                     .RADIO_IN ( RADIO[0]  ),
                     .DATA_OUT ( RADIO_CH1 ) );

radio_rx radio_ch_2( .CLK_1M   ( CLK_1M    ),
                     .RESET    ( RESET     ),
                     .RADIO_IN ( RADIO[1]  ),
                     .DATA_OUT ( RADIO_CH2 ) );

radio_rx radio_ch_3( .CLK_1M   ( CLK_1M    ),
                     .RESET    ( RESET     ),
                     .RADIO_IN ( RADIO[2]  ),
                     .DATA_OUT ( RADIO_CH3 ) );

radio_rx radio_ch_4( .CLK_1M   ( CLK_1M    ),
                     .RESET    ( RESET     ),
                     .RADIO_IN ( RADIO[3]  ),
                     .DATA_OUT ( RADIO_CH4 ) );

radio_rx radio_ch_5( .CLK_1M   ( CLK_1M    ),
                     .RESET    ( RESET     ),
                     .RADIO_IN ( RADIO[4]  ),
                     .DATA_OUT ( RADIO_CH5 ) );

radio_rx radio_ch_6( .CLK_1M   ( CLK_1M    ),
                     .RESET    ( RESET     ),
                     .RADIO_IN ( RADIO[5]  ),
                     .DATA_OUT ( RADIO_CH6 ) );
                     
//===================ENGINES REGISTERS===========================//
                    
io32 eng_13_reg( .CLK     ( CLK_CPU            ),
                 .WE      ( ENGINES_13_WE ),
                 .DATA_IN ( ENGINES_13_WD ),
                 .IO_OUT  ( ENGINES_13_CTRL    ) );

io32 eng_24_reg( .CLK     ( CLK_CPU            ),
                 .WE      ( ENGINES_24_WE ),
                 .DATA_IN ( ENGINES_24_WD ),
                 .IO_OUT  ( ENGINES_24_CTRL    ) );

//====================ENGINES CONTROL============================//

ppm_out eng_1_ctrl(  .CLK_1M  ( CLK_1M                ),
                     .VAL     ( ENGINES_13_CTRL[9:0]  ),
                     .PPM     ( ENGINE_CONTROL[0]     ) );
                
ppm_out eng_2_ctrl(  .CLK_1M  ( CLK_1M                ),
                     .VAL     ( ENGINES_24_CTRL[9:0]  ),
                     .PPM     ( ENGINE_CONTROL[1]     ) );

ppm_out eng_3_ctrl(  .CLK_1M  ( CLK_1M                ),
                     .VAL     ( ENGINES_13_CTRL[25:16]),
                     .PPM     ( ENGINE_CONTROL[2]     ) );

ppm_out eng_4_ctrl(  .CLK_1M  ( CLK_1M                ),
                     .VAL     ( ENGINES_24_CTRL[25:16]),
                     .PPM     ( ENGINE_CONTROL[3]     ) );
                     
endmodule
//===============================================================//
