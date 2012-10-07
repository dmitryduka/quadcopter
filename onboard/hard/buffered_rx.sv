module buffered_rx   (  input         CLK,
                        input         RESET,
                        
                        output [31:0] DATA_RD,
                        input         DATA_WE,
                        
                        input         RXD );

parameter DEPTH = 5;

logic [7:0] RAM [0:2**DEPTH-1];
logic [7:0] reg_ram; //Altera syncram optimization

wire [DEPTH-1:0] write_ptr, read_ptr, count;

assign count = write_ptr - read_ptr;
                        
wire [7:0] rx_data;
wire       rx_en;

uart_rx #(434) the_rx( .CLK   ( CLK     ),
                       .RESET ( RESET   ),
                       .DATA  ( rx_data ),
                       .EN    ( rx_en   ),
                       .RX    ( RXD     ) );

always_ff@(posedge CLK)
   begin
   if(rx_en) RAM[write_ptr] <= rx_data;
   reg_ram <= RAM[read_ptr];
   end

counter #(DEPTH) wr_counter(CLK, RESET, rx_en,   write_ptr);
counter #(DEPTH) rd_counter(CLK, RESET, DATA_WE, read_ptr ); 

assign DATA_RD = {'0, count, 8'd0, reg_ram};
                                               
endmodule
