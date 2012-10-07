module io32(   input         CLK,
               input         WE,
               input  [31:0] DATA_IN,
               output [31:0] IO_OUT );

logic [31:0] out_reg;

always_ff @(posedge CLK)
   if(WE) out_reg <= DATA_IN;

assign IO_OUT = out_reg;

endmodule
