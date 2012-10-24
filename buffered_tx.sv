module buffered_tx ( input         CLK,
                     input         RESET,
                     input  [31:0] DATA_WD,
                     output [31:0] DATA_RD,
                     input         DATA_WE, 
                     output        UART_TX );

parameter DEPTH = 5;

logic [7:0] RAM [0:2**DEPTH-1];
logic [7:0] reg_ram; //Altera syncram optimization


wire [DEPTH-1:0] write_ptr, read_ptr;
wire [DEPTH-1:0] byte_count = write_ptr - read_ptr;

wire tx_ready, tx_en;
wire fifo_empty = tx_ready && (write_ptr == read_ptr);

always_ff@ (posedge CLK)
   if(DATA_WE) RAM[write_ptr] <= DATA_WD[7:0];
   
always_ff@ (posedge CLK)
  reg_ram <= RAM[read_ptr];

tx_fsm the_fsm( CLK, RESET, fifo_empty, tx_ready, tx_en);
        
counter #(DEPTH) wr_counter(CLK, RESET, DATA_WE, write_ptr);
counter #(DEPTH) rd_counter(CLK, RESET, tx_en,   read_ptr ); 
                    
uart_tx #(434)  the_uart_tx( .CLK    ( CLK           ),  //434 --> 115200 @50MHz CLK
                             .RESET  ( RESET         ),
                             .DATA   ( reg_ram       ),
                             .EN     ( tx_en         ),
                             .READY  ( tx_ready      ),
                             .TX     ( UART_TX       ) );                     
                     
assign DATA_RD = {{32-DEPTH{1'b0}}, byte_count};
                     
endmodule
//=============================================================//
module tx_fsm( input  CLK,
               input  RESET,
               input  FIFO_EMPTY,
               input  TX_READY,
               output TX_EN );
               
enum int unsigned { ST_READY = 0, 
                    ST_STROBE = 1, 
                    ST_WAIT = 2 } state, next;
               
always_comb 
   begin
   next = ST_READY;
     case(state)
     ST_READY:  if(TX_READY && !FIFO_EMPTY)   next = ST_STROBE;
                else                          next = ST_READY;
                  
     ST_STROBE:                               next = ST_WAIT;
                  
     ST_WAIT:   if(TX_READY)                  next = ST_READY;
                else                          next = ST_WAIT;
     endcase
   end   
   
always_ff@(posedge CLK)
   if(RESET) state <= ST_READY;
   else      state <= next;
   
assign TX_EN = (state == ST_STROBE);
               
endmodule
//=============================================================//
