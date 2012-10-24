module coprocessor0 ( input         CLK,
                      input         RESET,

                      input  [31:0] DEC_STAGE_ADDR,
                      input         DEC_STAGE_EXCP,
                      input         DELAY_SLOT,

                      input  [31:0] EXE_STAGE_ADDR,
                      input         EXE_STAGE_EXCP,

                      input  [31:0] MEM_STAGE_ADDR,
                      input         MEM_STAGE_EXCP,

                      input   [4:0] CP0_REG_SELECT, 
                      output [31:0] CP0_REG_OUT,
                      output [31:0] EXCP_VECTOR );
                    
wire exception_occured = DEC_STAGE_EXCP | EXE_STAGE_EXCP | MEM_STAGE_EXCP;

wire [31:0] excp_epc  = MEM_STAGE_EXCP ? MEM_STAGE_ADDR :
                        EXE_STAGE_EXCP ? EXE_STAGE_ADDR :
                        DEC_STAGE_EXCP ? DEC_STAGE_ADDR : 32'h00000000;                  

wire [31:0] excp_cause = {DELAY_SLOT, 28'd0, MEM_STAGE_EXCP, EXE_STAGE_EXCP, DEC_STAGE_EXCP};

wire [31:0] cause_q, epc_q;

ffd #(32) cause_reg(CLK, RESET, exception_occured, excp_cause, cause_q); 
ffd #(32) epc_reg  (CLK, RESET, exception_occured, excp_epc,   epc_q);

assign CP0_REG_OUT = (CP0_REG_SELECT == 5'd13) ? cause_q :
                     (CP0_REG_SELECT == 5'd14) ? epc_q   : 32'hECECECEC; //epc
 
assign EXCP_VECTOR = 32'h0000DEAD; // constant without MMU
                    
endmodule