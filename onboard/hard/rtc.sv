module rtc  (  input         CLK,
               input         RESET,
               output [31:0] DATA_OUT );

reg [31:0] cnt = 0;
               
always_ff@(posedge CLK)
   if(RESET) cnt <= '0;
   else      cnt <= cnt + 32'd1;

assign DATA_OUT = cnt;

endmodule
