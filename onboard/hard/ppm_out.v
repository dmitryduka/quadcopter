module ppm_out( input       CLK_1M,
                input [9:0] VAL,
                output      PPM );


//motor starts @00101010 = 1042 uS pulse                

reg [11:0] val = 0;
reg [11:0] cnt = 0;
reg out = 0;

always@ (posedge CLK_1M)
  begin
  if(cnt < 2250)
    begin
    cnt <= cnt + 12'd1;
    if(cnt < val) out <= 1'b1;
    else          out <= 1'b0;  
    end
  else
    begin
    cnt <= 12'd0;
    val <= VAL + 12'd1000;
    end  
  end

assign PPM = out;                
endmodule
