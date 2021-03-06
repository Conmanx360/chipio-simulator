#include "emu8051_defs.h"

const struct opcode_info opcodes[];

const struct opcode_info *get_op_info(uint8_t op)
{
	return &opcodes[op];
}

const struct opcode_info opcodes[] = {
	{ .op_id  = NOP_ID,
	  .args   = 0,
	  .length = 1,
	  .handler = NULL,
	},
	{ .op_id  = AJMP_ID,
	  .dst_id = ADDR11_ID,
	  .args   = 1,
	  .length = 2,
	  .handler = ajmp_handler,
	},
	{ .op_id  = LJMP_ID,
	  .dst_id = ADDR16_ID,
	  .args   = 1,
	  .length = 3,
	  .handler = ljmp_handler,
	},
	{ .op_id  = RR_ID,
	  .dst_id = A_ID,
	  .args   = 1,
	  .length = 1,
	  .handler = rr_handler,
	},
	{ .op_id  = INC_ID,
	  .dst_id = A_ID,
	  .args   = 1,
	  .length = 1,
	  .handler = inc_handler,
	},
	{ .op_id  = INC_ID,
	  .dst_id = DIRECT_ID,
	  .args   = 1,
	  .length = 2,
	  .handler = inc_handler,
	},
	{ .op_id  = INC_ID,
	  .dst_id = I_R0_ID,
	  .args   = 1,
	  .length = 1,
	  .handler = inc_handler,
	},
	{ .op_id  = INC_ID,
	  .dst_id = I_R1_ID,
	  .args   = 1,
	  .length = 1,
	  .handler = inc_handler,
	},
	{ .op_id  = INC_ID,
	  .dst_id = R0_ID,
	  .args   = 1,
	  .length = 1,
	  .handler = inc_handler,
	},
	{ .op_id  = INC_ID,
	  .dst_id = R1_ID,
	  .args   = 1,
	  .length = 1,
	  .handler = inc_handler,
	},
	{ .op_id  = INC_ID,
	  .dst_id = R2_ID,
	  .args   = 1,
	  .length = 1,
	  .handler = inc_handler,
	},
	{ .op_id  = INC_ID,
	  .dst_id = R3_ID,
	  .args   = 1,
	  .length = 1,
	  .handler = inc_handler,
	},
	{ .op_id  = INC_ID,
	  .dst_id = R4_ID,
	  .args   = 1,
	  .length = 1,
	  .handler = inc_handler,
	},
	{ .op_id  = INC_ID,
	  .dst_id = R5_ID,
	  .args   = 1,
	  .length = 1,
	  .handler = inc_handler,
	},
	{ .op_id  = INC_ID,
	  .dst_id = R6_ID,
	  .args   = 1,
	  .length = 1,
	  .handler = inc_handler,
	},
	{ .op_id  = INC_ID,
	  .dst_id = R7_ID,
	  .args   = 1,
	  .length = 1,
	  .handler = inc_handler,
	},
	{ .op_id  = JBC_ID,
	  .dst_id = BIT_ID,
	  .src_id = OFFSET_ID,
	  .args   = 2,
	  .length = 3,
	  .handler = jbc_handler,
	},
	{ .op_id  = ACALL_ID,
	  .dst_id = ADDR11_ID,
	  .args   = 1,
	  .length = 2,
	  .handler = acall_handler,
	},
	{ .op_id  = LCALL_ID,
	  .dst_id = ADDR16_ID,
	  .args   = 1,
	  .length = 3,
	  .handler = lcall_handler,
	},
	{ .op_id  = RRC_ID,
	  .dst_id = A_ID,
	  .args   = 1,
	  .length = 1,
	  .handler = rrc_handler,
	},
	{ .op_id  = DEC_ID,
	  .dst_id = A_ID,
	  .args   = 1,
	  .length = 1,
	  .handler = dec_handler,
	},
	{ .op_id  = DEC_ID,
	  .dst_id = DIRECT_ID,
	  .args   = 1,
	  .length = 2,
	  .handler = dec_handler,
	},
	{ .op_id  = DEC_ID,
	  .dst_id = I_R0_ID,
	  .args   = 1,
	  .length = 1,
	  .handler = dec_handler,
	},
	{ .op_id  = DEC_ID,
	  .dst_id = I_R1_ID,
	  .args   = 1,
	  .length = 1,
	  .handler = dec_handler,
	},
	{ .op_id  = DEC_ID,
	  .dst_id = R0_ID,
	  .args   = 1,
	  .length = 1,
	  .handler = dec_handler,
	},
	{ .op_id  = DEC_ID,
	  .dst_id = R1_ID,
	  .args   = 1,
	  .length = 1,
	  .handler = dec_handler,
	},
	{ .op_id  = DEC_ID,
	  .dst_id = R2_ID,
	  .args   = 1,
	  .length = 1,
	  .handler = dec_handler,
	},
	{ .op_id  = DEC_ID,
	  .dst_id = R3_ID,
	  .args   = 1,
	  .length = 1,
	  .handler = dec_handler,
	},
	{ .op_id  = DEC_ID,
	  .dst_id = R4_ID,
	  .args   = 1,
	  .length = 1,
	  .handler = dec_handler,
	},
	{ .op_id  = DEC_ID,
	  .dst_id = R5_ID,
	  .args   = 1,
	  .length = 1,
	  .handler = dec_handler,
	},
	{ .op_id  = DEC_ID,
	  .dst_id = R6_ID,
	  .args   = 1,
	  .length = 1,
	  .handler = dec_handler,
	},
	{ .op_id  = DEC_ID,
	  .dst_id = R7_ID,
	  .args   = 1,
	  .length = 1,
	  .handler = dec_handler,
	},
	{ .op_id  = JB_ID,
	  .dst_id = BIT_ID,
	  .src_id = OFFSET_ID,
	  .args   = 2,
	  .length = 3,
	  .handler = jb_handler,
	},
	{ .op_id  = AJMP_ID,
	  .dst_id = ADDR11_ID,
	  .args   = 1,
	  .length = 2,
	  .handler = ajmp_handler,
	},
	{ .op_id  = RET_ID,
	  .dst_id = NO_SRC,
	  .src_id = NO_SRC,
	  .args  = 0,
	  .length = 1,
	  .handler = ret_handler,
	},
	{ .op_id  = RL_ID,
	  .dst_id = A_ID,
	  .args   = 1,
	  .length = 1,
	  .handler = rl_handler,
	},
	{ .op_id  = ADD_ID,
	  .dst_id = A_ID,
	  .src_id = IMMED_ID,
	  .args   = 2,
	  .length = 2,
	  .handler = add_handler,
	},
	{ .op_id  = ADD_ID,
	  .dst_id = A_ID,
	  .src_id = DIRECT_ID,
	  .args   = 2,
	  .length = 2,
	  .handler = add_handler,
	},
	{ .op_id  = ADD_ID,
	  .dst_id = A_ID,
	  .src_id = I_R0_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = add_handler,
	},
	{ .op_id  = ADD_ID,
	  .dst_id = A_ID,
	  .src_id = I_R1_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = add_handler,
	},
	{ .op_id  = ADD_ID,
	  .dst_id = A_ID,
	  .src_id = R0_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = add_handler,
	},
	{ .op_id  = ADD_ID,
	  .dst_id = A_ID,
	  .src_id = R1_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = add_handler,
	},
	{ .op_id  = ADD_ID,
	  .dst_id = A_ID,
	  .src_id = R2_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = add_handler,
	},
	{ .op_id  = ADD_ID,
	  .dst_id = A_ID,
	  .src_id = R3_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = add_handler,
	},
	{ .op_id  = ADD_ID,
	  .dst_id = A_ID,
	  .src_id = R4_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = add_handler,
	},
	{ .op_id  = ADD_ID,
	  .dst_id = A_ID,
	  .src_id = R5_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = add_handler,
	},
	{ .op_id  = ADD_ID,
	  .dst_id = A_ID,
	  .src_id = R6_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = add_handler,
	},
	{ .op_id  = ADD_ID,
	  .dst_id = A_ID,
	  .src_id = R7_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = add_handler,
	},
	{ .op_id  = JNB_ID,
	  .dst_id = BIT_ID,
	  .src_id = OFFSET_ID,
	  .args   = 2,
	  .length = 3,
	  .handler = jnb_handler,
	},
	{ .op_id  = ACALL_ID,
	  .dst_id = ADDR11_ID,
	  .args   = 1,
	  .length = 2,
	  .handler = acall_handler,
	},
	{ .op_id  = RETI_ID,
	  .dst_id = NO_SRC,
	  .src_id = NO_SRC,
	  .args  = 0,
	  .length = 1,
	  .handler = reti_handler,
	},
	{ .op_id  = RLC_ID,
	  .dst_id = A_ID,
	  .args   = 1,
	  .length = 1,
	  .handler = rlc_handler,
	},
	{ .op_id  = ADDC_ID,
	  .dst_id = A_ID,
	  .src_id = IMMED_ID,
	  .args   = 2,
	  .length = 2,
	  .handler = addc_handler,
	},
	{ .op_id  = ADDC_ID,
	  .dst_id = A_ID,
	  .src_id = DIRECT_ID,
	  .args   = 2,
	  .length = 2,
	  .handler = addc_handler,
	},
	{ .op_id  = ADDC_ID,
	  .dst_id = A_ID,
	  .src_id = I_R0_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = addc_handler,
	},
	{ .op_id  = ADDC_ID,
	  .dst_id = A_ID,
	  .src_id = I_R1_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = addc_handler,
	},
	{ .op_id  = ADDC_ID,
	  .dst_id = A_ID,
	  .src_id = R0_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = addc_handler,
	},
	{ .op_id  = ADDC_ID,
	  .dst_id = A_ID,
	  .src_id = R1_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = addc_handler,
	},
	{ .op_id  = ADDC_ID,
	  .dst_id = A_ID,
	  .src_id = R2_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = addc_handler,
	},
	{ .op_id  = ADDC_ID,
	  .dst_id = A_ID,
	  .src_id = R3_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = addc_handler,
	},
	{ .op_id  = ADDC_ID,
	  .dst_id = A_ID,
	  .src_id = R4_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = addc_handler,
	},
	{ .op_id  = ADDC_ID,
	  .dst_id = A_ID,
	  .src_id = R5_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = addc_handler,
	},
	{ .op_id  = ADDC_ID,
	  .dst_id = A_ID,
	  .src_id = R6_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = addc_handler,
	},
	{ .op_id  = ADDC_ID,
	  .dst_id = A_ID,
	  .src_id = R7_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = addc_handler,
	},
	{ .op_id  = JC_ID,
	  .dst_id = OFFSET_ID,
	  .args   = 1,
	  .length = 2,
	  .handler = jc_handler,
	},
	{ .op_id  = AJMP_ID,
	  .dst_id = ADDR11_ID,
	  .args   = 1,
	  .length = 2,
	  .handler = ajmp_handler,
	},
	{ .op_id  = ORL_ID,
	  .dst_id = DIRECT_ID,
	  .src_id = A_ID,
	  .args   = 2,
	  .length = 2,
	  .handler = orl_handler,
	},
	{ .op_id  = ORL_ID,
	  .dst_id = DIRECT_ID,
	  .src_id = IMMED_ID,
	  .args   = 2,
	  .length = 3,
	  .handler = orl_handler,
	},
	{ .op_id  = ORL_ID,
	  .dst_id = A_ID,
	  .src_id = IMMED_ID,
	  .args   = 2,
	  .length = 2,
	  .handler = orl_handler,
	},
	{ .op_id  = ORL_ID,
	  .dst_id = A_ID,
	  .src_id = DIRECT_ID,
	  .args   = 2,
	  .length = 2,
	  .handler = orl_handler,
	},
	{ .op_id  = ORL_ID,
	  .dst_id = A_ID,
	  .src_id = I_R0_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = orl_handler,
	},
	{ .op_id  = ORL_ID,
	  .dst_id = A_ID,
	  .src_id = I_R1_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = orl_handler,
	},
	{ .op_id  = ORL_ID,
	  .dst_id = A_ID,
	  .src_id = R0_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = orl_handler,
	},
	{ .op_id  = ORL_ID,
	  .dst_id = A_ID,
	  .src_id = R1_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = orl_handler,
	},
	{ .op_id  = ORL_ID,
	  .dst_id = A_ID,
	  .src_id = R2_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = orl_handler,
	},
	{ .op_id  = ORL_ID,
	  .dst_id = A_ID,
	  .src_id = R3_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = orl_handler,
	},
	{ .op_id  = ORL_ID,
	  .dst_id = A_ID,
	  .src_id = R4_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = orl_handler,
	},
	{ .op_id  = ORL_ID,
	  .dst_id = A_ID,
	  .src_id = R5_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = orl_handler,
	},
	{ .op_id  = ORL_ID,
	  .dst_id = A_ID,
	  .src_id = R6_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = orl_handler,
	},
	{ .op_id  = ORL_ID,
	  .dst_id = A_ID,
	  .src_id = R7_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = orl_handler,
	},
	{ .op_id  = JNC_ID,
	  .dst_id = OFFSET_ID,
	  .args   = 1,
	  .length = 2,
	  .handler = jnc_handler,
	},
	{ .op_id  = ACALL_ID,
	  .dst_id = ADDR11_ID,
	  .args   = 1,
	  .length = 2,
	  .handler = acall_handler,
	},
	{ .op_id  = ANL_ID,
	  .dst_id = DIRECT_ID,
	  .src_id = A_ID,
	  .args   = 2,
	  .length = 2,
	  .handler = anl_handler,
	},
	{ .op_id  = ANL_ID,
	  .dst_id = DIRECT_ID,
	  .src_id = IMMED_ID,
	  .args   = 2,
	  .length = 3,
	  .handler = anl_handler,
	},
	{ .op_id  = ANL_ID,
	  .dst_id = A_ID,
	  .src_id = IMMED_ID,
	  .args   = 2,
	  .length = 2,
	  .handler = anl_handler,
	},
	{ .op_id  = ANL_ID,
	  .dst_id = A_ID,
	  .src_id = DIRECT_ID,
	  .args   = 2,
	  .length = 2,
	  .handler = anl_handler,
	},
	{ .op_id  = ANL_ID,
	  .dst_id = A_ID,
	  .src_id = I_R0_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = anl_handler,
	},
	{ .op_id  = ANL_ID,
	  .dst_id = A_ID,
	  .src_id = I_R1_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = anl_handler,
	},
	{ .op_id  = ANL_ID,
	  .dst_id = A_ID,
	  .src_id = R0_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = anl_handler,
	},
	{ .op_id  = ANL_ID,
	  .dst_id = A_ID,
	  .src_id = R1_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = anl_handler,
	},
	{ .op_id  = ANL_ID,
	  .dst_id = A_ID,
	  .src_id = R2_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = anl_handler,
	},
	{ .op_id  = ANL_ID,
	  .dst_id = A_ID,
	  .src_id = R3_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = anl_handler,
	},
	{ .op_id  = ANL_ID,
	  .dst_id = A_ID,
	  .src_id = R4_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = anl_handler,
	},
	{ .op_id  = ANL_ID,
	  .dst_id = A_ID,
	  .src_id = R5_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = anl_handler,
	},
	{ .op_id  = ANL_ID,
	  .dst_id = A_ID,
	  .src_id = R6_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = anl_handler,
	},
	{ .op_id  = ANL_ID,
	  .dst_id = A_ID,
	  .src_id = R7_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = anl_handler,
	},
	{ .op_id  = JZ_ID,
	  .dst_id = OFFSET_ID,
	  .args   = 1,
	  .length = 2,
	  .handler = jz_handler,
	},
	{ .op_id  = AJMP_ID,
	  .dst_id = ADDR11_ID,
	  .args   = 1,
	  .length = 2,
	  .handler = ajmp_handler,
	},
	{ .op_id  = XRL_ID,
	  .dst_id = DIRECT_ID,
	  .src_id = A_ID,
	  .args   = 2,
	  .length = 2,
	  .handler = xrl_handler,
	},
	{ .op_id  = XRL_ID,
	  .dst_id = DIRECT_ID,
	  .src_id = IMMED_ID,
	  .args   = 2,
	  .length = 3,
	  .handler = xrl_handler,
	},
	{ .op_id  = XRL_ID,
	  .dst_id = A_ID,
	  .src_id = IMMED_ID,
	  .args   = 2,
	  .length = 2,
	  .handler = xrl_handler,
	},
	{ .op_id  = XRL_ID,
	  .dst_id = A_ID,
	  .src_id = DIRECT_ID,
	  .args   = 2,
	  .length = 2,
	  .handler = xrl_handler,
	},
	{ .op_id  = XRL_ID,
	  .dst_id = A_ID,
	  .src_id = I_R0_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = xrl_handler,
	},
	{ .op_id  = XRL_ID,
	  .dst_id = A_ID,
	  .src_id = I_R1_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = xrl_handler,
	},
	{ .op_id  = XRL_ID,
	  .dst_id = A_ID,
	  .src_id = R0_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = xrl_handler,
	},
	{ .op_id  = XRL_ID,
	  .dst_id = A_ID,
	  .src_id = R1_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = xrl_handler,
	},
	{ .op_id  = XRL_ID,
	  .dst_id = A_ID,
	  .src_id = R2_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = xrl_handler,
	},
	{ .op_id  = XRL_ID,
	  .dst_id = A_ID,
	  .src_id = R3_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = xrl_handler,
	},
	{ .op_id  = XRL_ID,
	  .dst_id = A_ID,
	  .src_id = R4_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = xrl_handler,
	},
	{ .op_id  = XRL_ID,
	  .dst_id = A_ID,
	  .src_id = R5_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = xrl_handler,
	},
	{ .op_id  = XRL_ID,
	  .dst_id = A_ID,
	  .src_id = R6_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = xrl_handler,
	},
	{ .op_id  = XRL_ID,
	  .dst_id = A_ID,
	  .src_id = R7_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = xrl_handler,
	},
	{ .op_id  = JNZ_ID,
	  .dst_id = OFFSET_ID,
	  .args   = 1,
	  .length = 2,
	  .handler = jnz_handler,
	},
	{ .op_id  = ACALL_ID,
	  .dst_id = ADDR11_ID,
	  .args   = 1,
	  .length = 2,
	  .handler = acall_handler,
	},
	{ .op_id  = ORL_ID,
	  .dst_id = C_ID,
	  .src_id = BIT_ID,
	  .args   = 2,
	  .length = 2,
	  .handler = orl_handler,
	},
	{ .op_id  = JMP_ID,
	  .dst_id = A_DPTR_ID,
	  .args   = 1,
	  .length = 1,
	  .handler = jmp_handler,
	},
	{ .op_id  = MOV_ID,
	  .dst_id = A_ID,
	  .src_id = IMMED_ID,
	  .args   = 2,
	  .length = 2,
	  .handler = mov_handler,
	},
	{ .op_id  = MOV_ID,
	  .dst_id = DIRECT_ID,
	  .src_id = IMMED_ID,
	  .args   = 2,
	  .length = 3,
	  .handler = mov_handler,
	},
	{ .op_id  = MOV_ID,
	  .dst_id = I_R0_ID,
	  .src_id = IMMED_ID,
	  .args   = 2,
	  .length = 2,
	  .handler = mov_handler,
	},
	{ .op_id  = MOV_ID,
	  .dst_id = I_R1_ID,
	  .src_id = IMMED_ID,
	  .args   = 2,
	  .length = 2,
	  .handler = mov_handler,
	},
	{ .op_id  = MOV_ID,
	  .dst_id = R0_ID,
	  .src_id = IMMED_ID,
	  .args   = 2,
	  .length = 2,
	  .handler = mov_handler,
	},
	{ .op_id  = MOV_ID,
	  .dst_id = R1_ID,
	  .src_id = IMMED_ID,
	  .args   = 2,
	  .length = 2,
	  .handler = mov_handler,
	},
	{ .op_id  = MOV_ID,
	  .dst_id = R2_ID,
	  .src_id = IMMED_ID,
	  .args   = 2,
	  .length = 2,
	  .handler = mov_handler,
	},
	{ .op_id  = MOV_ID,
	  .dst_id = R3_ID,
	  .src_id = IMMED_ID,
	  .args   = 2,
	  .length = 2,
	  .handler = mov_handler,
	},
	{ .op_id  = MOV_ID,
	  .dst_id = R4_ID,
	  .src_id = IMMED_ID,
	  .args   = 2,
	  .length = 2,
	  .handler = mov_handler,
	},
	{ .op_id  = MOV_ID,
	  .dst_id = R5_ID,
	  .src_id = IMMED_ID,
	  .args   = 2,
	  .length = 2,
	  .handler = mov_handler,
	},
	{ .op_id  = MOV_ID,
	  .dst_id = R6_ID,
	  .src_id = IMMED_ID,
	  .args   = 2,
	  .length = 2,
	  .handler = mov_handler,
	},
	{ .op_id  = MOV_ID,
	  .dst_id = R7_ID,
	  .src_id = IMMED_ID,
	  .args   = 2,
	  .length = 2,
	  .handler = mov_handler,
	},
	{ .op_id  = SJMP_ID,
	  .dst_id = OFFSET_ID,
	  .args   = 1,
	  .length = 2,
	  .handler = sjmp_handler,
	},
	{ .op_id  = AJMP_ID,
	  .dst_id = ADDR11_ID,
	  .args   = 1,
	  .length = 2,
	  .handler = ajmp_handler,
	},
	{ .op_id  = ANL_ID,
	  .dst_id = C_ID,
	  .src_id = BIT_ID,
	  .args   = 2,
	  .length = 2,
	  .handler = anl_handler,
	},
	{ .op_id  = MOVC_ID,
	  .dst_id = A_ID,
	  .src_id = A_PC_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = movc_handler,
	},
	{ .op_id  = DIV_ID,
	  .dst_id = AB_ID,
	  .args   = 1,
	  .length = 1,
	  .handler = div_handler,
	},
	{ .op_id  = MOV_ID,
	  .dst_id = DIRECT_ID,
	  .src_id = DIRECT_ID,
	  .args   = 2,
	  .length = 3,
	  .handler = mov_handler,
	},
	{ .op_id  = MOV_ID,
	  .dst_id = DIRECT_ID,
	  .src_id = I_R0_ID,
	  .args   = 2,
	  .length = 2,
	  .handler = mov_handler,
	},
	{ .op_id  = MOV_ID,
	  .dst_id = DIRECT_ID,
	  .src_id = I_R1_ID,
	  .args   = 2,
	  .length = 2,
	  .handler = mov_handler,
	},
	{ .op_id  = MOV_ID,
	  .dst_id = DIRECT_ID,
	  .src_id = R0_ID,
	  .args   = 2,
	  .length = 2,
	  .handler = mov_handler,
	},
	{ .op_id  = MOV_ID,
	  .dst_id = DIRECT_ID,
	  .src_id = R1_ID,
	  .args   = 2,
	  .length = 2,
	  .handler = mov_handler,
	},
	{ .op_id  = MOV_ID,
	  .dst_id = DIRECT_ID,
	  .src_id = R2_ID,
	  .args   = 2,
	  .length = 2,
	  .handler = mov_handler,
	},
	{ .op_id  = MOV_ID,
	  .dst_id = DIRECT_ID,
	  .src_id = R3_ID,
	  .args   = 2,
	  .length = 2,
	  .handler = mov_handler,
	},
	{ .op_id  = MOV_ID,
	  .dst_id = DIRECT_ID,
	  .src_id = R4_ID,
	  .args   = 2,
	  .length = 2,
	  .handler = mov_handler,
	},
	{ .op_id  = MOV_ID,
	  .dst_id = DIRECT_ID,
	  .src_id = R5_ID,
	  .args   = 2,
	  .length = 2,
	  .handler = mov_handler,
	},
	{ .op_id  = MOV_ID,
	  .dst_id = DIRECT_ID,
	  .src_id = R6_ID,
	  .args   = 2,
	  .length = 2,
	  .handler = mov_handler,
	},
	{ .op_id  = MOV_ID,
	  .dst_id = DIRECT_ID,
	  .src_id = R7_ID,
	  .args   = 2,
	  .length = 2,
	  .handler = mov_handler,
	},
	{ .op_id  = MOV_ID,
	  .dst_id = DPTR_ID,
	  .src_id = IMMED_ID,
	  .args   = 2,
	  .length = 3,
	  .handler = mov_handler,
	},
	{ .op_id  = ACALL_ID,
	  .dst_id = ADDR11_ID,
	  .args   = 1,
	  .length = 2,
	  .handler = acall_handler,
	},
	{ .op_id  = MOV_ID,
	  .dst_id = BIT_ID,
	  .src_id = C_ID,
	  .args   = 2,
	  .length = 2,
	  .handler = mov_handler,
	},
	{ .op_id  = MOVC_ID,
	  .dst_id = A_ID,
	  .src_id = A_DPTR_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = movc_handler,
	},
	{ .op_id  = SUBB_ID,
	  .dst_id = A_ID,
	  .src_id = IMMED_ID,
	  .args   = 2,
	  .length = 2,
	  .handler = subb_handler,
	},
	{ .op_id  = SUBB_ID,
	  .dst_id = A_ID,
	  .src_id = DIRECT_ID,
	  .args   = 2,
	  .length = 2,
	  .handler = subb_handler,
	},
	{ .op_id  = SUBB_ID,
	  .dst_id = A_ID,
	  .src_id = I_R0_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = subb_handler,
	},
	{ .op_id  = SUBB_ID,
	  .dst_id = A_ID,
	  .src_id = I_R1_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = subb_handler,
	},
	{ .op_id  = SUBB_ID,
	  .dst_id = A_ID,
	  .src_id = R0_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = subb_handler,
	},
	{ .op_id  = SUBB_ID,
	  .dst_id = A_ID,
	  .src_id = R1_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = subb_handler,
	},
	{ .op_id  = SUBB_ID,
	  .dst_id = A_ID,
	  .src_id = R2_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = subb_handler,
	},
	{ .op_id  = SUBB_ID,
	  .dst_id = A_ID,
	  .src_id = R3_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = subb_handler,
	},
	{ .op_id  = SUBB_ID,
	  .dst_id = A_ID,
	  .src_id = R4_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = subb_handler,
	},
	{ .op_id  = SUBB_ID,
	  .dst_id = A_ID,
	  .src_id = R5_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = subb_handler,
	},
	{ .op_id  = SUBB_ID,
	  .dst_id = A_ID,
	  .src_id = R6_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = subb_handler,
	},
	{ .op_id  = SUBB_ID,
	  .dst_id = A_ID,
	  .src_id = R7_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = subb_handler,
	},
	{ .op_id  = ORL_ID,
	  .dst_id = C_ID,
	  .src_id = IBIT_ID,
	  .args   = 2,
	  .length = 2,
	  .handler = orl_handler,
	},
	{ .op_id  = AJMP_ID,
	  .dst_id = ADDR11_ID,
	  .args   = 1,
	  .length = 2,
	  .handler = ajmp_handler,
	},
	{ .op_id  = MOV_ID,
	  .dst_id = C_ID,
	  .src_id = BIT_ID,
	  .args   = 2,
	  .length = 2,
	  .handler = mov_handler,
	},
	{ .op_id  = INC_ID,
	  .dst_id = DPTR_ID,
	  .args   = 1,
	  .length = 1,
	  .handler = inc_handler,
	},
	{ .op_id  = MUL_ID,
	  .dst_id = AB_ID,
	  .args   = 1,
	  .length = 1,
	  .handler = mul_handler,
	},
	{ .op_id  = RSRV_ID,
	  .length = 1,
	  .handler = NULL,
	},
	{ .op_id  = MOV_ID,
	  .dst_id = I_R0_ID,
	  .src_id = DIRECT_ID,
	  .args   = 2,
	  .length = 2,
	  .handler = mov_handler,
	},
	{ .op_id  = MOV_ID,
	  .dst_id = I_R1_ID,
	  .src_id = DIRECT_ID,
	  .args   = 2,
	  .length = 2,
	  .handler = mov_handler,
	},
	{ .op_id  = MOV_ID,
	  .dst_id = R0_ID,
	  .src_id = DIRECT_ID,
	  .args   = 2,
	  .length = 2,
	  .handler = mov_handler,
	},
	{ .op_id  = MOV_ID,
	  .dst_id = R1_ID,
	  .src_id = DIRECT_ID,
	  .args   = 2,
	  .length = 2,
	  .handler = mov_handler,
	},
	{ .op_id  = MOV_ID,
	  .dst_id = R2_ID,
	  .src_id = DIRECT_ID,
	  .args   = 2,
	  .length = 2,
	  .handler = mov_handler,
	},
	{ .op_id  = MOV_ID,
	  .dst_id = R3_ID,
	  .src_id = DIRECT_ID,
	  .args   = 2,
	  .length = 2,
	  .handler = mov_handler,
	},
	{ .op_id  = MOV_ID,
	  .dst_id = R4_ID,
	  .src_id = DIRECT_ID,
	  .args   = 2,
	  .length = 2,
	  .handler = mov_handler,
	},
	{ .op_id  = MOV_ID,
	  .dst_id = R5_ID,
	  .src_id = DIRECT_ID,
	  .args   = 2,
	  .length = 2,
	  .handler = mov_handler,
	},
	{ .op_id  = MOV_ID,
	  .dst_id = R6_ID,
	  .src_id = DIRECT_ID,
	  .args   = 2,
	  .length = 2,
	  .handler = mov_handler,
	},
	{ .op_id  = MOV_ID,
	  .dst_id = R7_ID,
	  .src_id = DIRECT_ID,
	  .args   = 2,
	  .length = 2,
	  .handler = mov_handler,
	},
	{ .op_id  = ANL_ID,
	  .dst_id = C_ID,
	  .src_id = IBIT_ID,
	  .args   = 2,
	  .length = 2,
	  .handler = anl_handler,
	},
	{ .op_id  = ACALL_ID,
	  .dst_id = ADDR11_ID,
	  .args   = 1,
	  .length = 2,
	  .handler = acall_handler,
	},
	{ .op_id  = CPL_ID,
	  .dst_id = BIT_ID,
	  .args   = 1,
	  .length = 2,
	  .handler = cpl_handler,
	},
	{ .op_id  = CPL_ID,
	  .dst_id = C_ID,
	  .args   = 1,
	  .length = 1,
	  .handler = cpl_handler,
	},
	{ .op_id  = CJNE_ID,
	  .dst_id = A_ID,
	  .src_id = IMMED_ID,
	  .args   = 2,
	  .length = 3,
	  .handler = cjne_handler,
	},
	{ .op_id  = CJNE_ID,
	  .dst_id = A_ID,
	  .src_id = DIRECT_ID,
	  .args   = 2,
	  .length = 3,
	  .handler = cjne_handler,
	},
	{ .op_id  = CJNE_ID,
	  .dst_id = I_R0_ID,
	  .src_id = IMMED_ID,
	  .args   = 2,
	  .length = 3,
	  .handler = cjne_handler,
	},
	{ .op_id  = CJNE_ID,
	  .dst_id = I_R1_ID,
	  .src_id = IMMED_ID,
	  .args   = 2,
	  .length = 3,
	  .handler = cjne_handler,
	},
	{ .op_id  = CJNE_ID,
	  .dst_id = R0_ID,
	  .src_id = IMMED_ID,
	  .args   = 2,
	  .length = 3,
	  .handler = cjne_handler,
	},
	{ .op_id  = CJNE_ID,
	  .dst_id = R1_ID,
	  .src_id = IMMED_ID,
	  .args   = 2,
	  .length = 3,
	  .handler = cjne_handler,
	},
	{ .op_id  = CJNE_ID,
	  .dst_id = R2_ID,
	  .src_id = IMMED_ID,
	  .args   = 2,
	  .length = 3,
	  .handler = cjne_handler,
	},
	{ .op_id  = CJNE_ID,
	  .dst_id = R3_ID,
	  .src_id = IMMED_ID,
	  .args   = 2,
	  .length = 3,
	  .handler = cjne_handler,
	},
	{ .op_id  = CJNE_ID,
	  .dst_id = R4_ID,
	  .src_id = IMMED_ID,
	  .args   = 2,
	  .length = 3,
	  .handler = cjne_handler,
	},
	{ .op_id  = CJNE_ID,
	  .dst_id = R5_ID,
	  .src_id = IMMED_ID,
	  .args   = 2,
	  .length = 3,
	  .handler = cjne_handler,
	},
	{ .op_id  = CJNE_ID,
	  .dst_id = R6_ID,
	  .src_id = IMMED_ID,
	  .args   = 2,
	  .length = 3,
	  .handler = cjne_handler,
	},
	{ .op_id  = CJNE_ID,
	  .dst_id = R7_ID,
	  .src_id = IMMED_ID,
	  .args   = 2,
	  .length = 3,
	  .handler = cjne_handler,
	},
	{ .op_id  = PUSH_ID,
	  .dst_id = DIRECT_ID,
	  .args   = 1,
	  .length = 2,
	  .handler = push_handler,
	},
	{ .op_id  = AJMP_ID,
	  .dst_id = ADDR11_ID,
	  .args   = 1,
	  .length = 2,
	  .handler = ajmp_handler,
	},
	{ .op_id  = CLR_ID,
	  .dst_id = BIT_ID,
	  .args   = 1,
	  .length = 2,
	  .handler = clr_handler,
	},
	{ .op_id  = CLR_ID,
	  .dst_id = C_ID,
	  .args   = 1,
	  .length = 1,
	  .handler = clr_handler,
	},
	{ .op_id  = SWAP_ID,
	  .dst_id = A_ID,
	  .args   = 1,
	  .length = 1,
	  .handler = swap_handler,
	},
	{ .op_id  = XCH_ID,
	  .dst_id = A_ID,
	  .src_id = DIRECT_ID,
	  .args   = 2,
	  .length = 2,
	  .handler = xch_handler,
	},
	{ .op_id  = XCH_ID,
	  .dst_id = A_ID,
	  .src_id = I_R0_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = xch_handler,
	},
	{ .op_id  = XCH_ID,
	  .dst_id = A_ID,
	  .src_id = I_R1_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = xch_handler,
	},
	{ .op_id  = XCH_ID,
	  .dst_id = A_ID,
	  .src_id = R0_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = xch_handler,
	},
	{ .op_id  = XCH_ID,
	  .dst_id = A_ID,
	  .src_id = R1_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = xch_handler,
	},
	{ .op_id  = XCH_ID,
	  .dst_id = A_ID,
	  .src_id = R2_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = xch_handler,
	},
	{ .op_id  = XCH_ID,
	  .dst_id = A_ID,
	  .src_id = R3_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = xch_handler,
	},
	{ .op_id  = XCH_ID,
	  .dst_id = A_ID,
	  .src_id = R4_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = xch_handler,
	},
	{ .op_id  = XCH_ID,
	  .dst_id = A_ID,
	  .src_id = R5_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = xch_handler,
	},
	{ .op_id  = XCH_ID,
	  .dst_id = A_ID,
	  .src_id = R6_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = xch_handler,
	},
	{ .op_id  = XCH_ID,
	  .dst_id = A_ID,
	  .src_id = R7_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = xch_handler,
	},
	{ .op_id  = POP_ID,
	  .dst_id = DIRECT_ID,
	  .args   = 1,
	  .length = 2,
	  .handler = pop_handler,
	},
	{ .op_id  = ACALL_ID,
	  .dst_id = ADDR11_ID,
	  .args   = 1,
	  .length = 2,
	  .handler = acall_handler,
	},
	{ .op_id  = SETB_ID,
	  .dst_id = BIT_ID,
	  .args   = 1,
	  .length = 2,
	  .handler = setb_handler,
	},
	{ .op_id  = SETB_ID,
	  .dst_id = C_ID,
	  .args   = 1,
	  .length = 1,
	  .handler = setb_handler,
	},
	{ .op_id  = DA_ID,
	  .dst_id = A_ID,
	  .args   = 1,
	  .length = 1,
	  .handler = da_handler,
	},
	{ .op_id  = DJNZ_ID,
	  .dst_id = DIRECT_ID,
	  .src_id = OFFSET_ID,
	  .args   = 2,
	  .length = 3,
	  .handler = djnz_handler,
	},
	{ .op_id  = XCHD_ID,
	  .dst_id = A_ID,
	  .src_id = I_R0_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = xchd_handler,
	},
	{ .op_id  = XCHD_ID,
	  .dst_id = A_ID,
	  .src_id = I_R1_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = xchd_handler,
	},
	{ .op_id  = DJNZ_ID,
	  .dst_id = R0_ID,
	  .src_id = OFFSET_ID,
	  .args   = 2,
	  .length = 2,
	  .handler = djnz_handler,
	},
	{ .op_id  = DJNZ_ID,
	  .dst_id = R1_ID,
	  .src_id = OFFSET_ID,
	  .args   = 2,
	  .length = 2,
	  .handler = djnz_handler,
	},
	{ .op_id  = DJNZ_ID,
	  .dst_id = R2_ID,
	  .src_id = OFFSET_ID,
	  .args   = 2,
	  .length = 2,
	  .handler = djnz_handler,
	},
	{ .op_id  = DJNZ_ID,
	  .dst_id = R3_ID,
	  .src_id = OFFSET_ID,
	  .args   = 2,
	  .length = 2,
	  .handler = djnz_handler,
	},
	{ .op_id  = DJNZ_ID,
	  .dst_id = R4_ID,
	  .src_id = OFFSET_ID,
	  .args   = 2,
	  .length = 2,
	  .handler = djnz_handler,
	},
	{ .op_id  = DJNZ_ID,
	  .dst_id = R5_ID,
	  .src_id = OFFSET_ID,
	  .args   = 2,
	  .length = 2,
	  .handler = djnz_handler,
	},
	{ .op_id  = DJNZ_ID,
	  .dst_id = R6_ID,
	  .src_id = OFFSET_ID,
	  .args   = 2,
	  .length = 2,
	  .handler = djnz_handler,
	},
	{ .op_id  = DJNZ_ID,
	  .dst_id = R7_ID,
	  .src_id = OFFSET_ID,
	  .args   = 2,
	  .length = 2,
	  .handler = djnz_handler,
	},
	{ .op_id  = MOVX_ID,
	  .dst_id = A_ID,
	  .src_id = IDPTR_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = movx_handler,
	},
	{ .op_id  = AJMP_ID,
	  .dst_id = ADDR11_ID,
	  .args   = 1,
	  .length = 2,
	  .handler = ajmp_handler,
	},
	{ .op_id  = MOVX_ID,
	  .dst_id = A_ID,
	  .src_id = I_R0_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = movx_handler,
	},
	{ .op_id  = MOVX_ID,
	  .dst_id = A_ID,
	  .src_id = I_R1_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = movx_handler,
	},
	{ .op_id  = CLR_ID,
	  .dst_id = A_ID,
	  .args   = 1,
	  .length = 1,
	  .handler = clr_handler,
	},
	{ .op_id  = MOV_ID,
	  .dst_id = A_ID,
	  .src_id = DIRECT_ID,
	  .args   = 2,
	  .length = 2,
	  .handler = mov_handler,
	},
	{ .op_id  = MOV_ID,
	  .dst_id = A_ID,
	  .src_id = I_R0_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = mov_handler,
	},
	{ .op_id  = MOV_ID,
	  .dst_id = A_ID,
	  .src_id = I_R1_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = mov_handler,
	},
	{ .op_id  = MOV_ID,
	  .dst_id = A_ID,
	  .src_id = R0_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = mov_handler,
	},
	{ .op_id  = MOV_ID,
	  .dst_id = A_ID,
	  .src_id = R1_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = mov_handler,
	},
	{ .op_id  = MOV_ID,
	  .dst_id = A_ID,
	  .src_id = R2_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = mov_handler,
	},
	{ .op_id  = MOV_ID,
	  .dst_id = A_ID,
	  .src_id = R3_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = mov_handler,
	},
	{ .op_id  = MOV_ID,
	  .dst_id = A_ID,
	  .src_id = R4_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = mov_handler,
	},
	{ .op_id  = MOV_ID,
	  .dst_id = A_ID,
	  .src_id = R5_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = mov_handler,
	},
	{ .op_id  = MOV_ID,
	  .dst_id = A_ID,
	  .src_id = R6_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = mov_handler,
	},
	{ .op_id  = MOV_ID,
	  .dst_id = A_ID,
	  .src_id = R7_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = mov_handler,
	},
	{ .op_id  = MOVX_ID,
	  .dst_id = IDPTR_ID,
	  .src_id = A_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = movx_handler,
	},
	{ .op_id  = ACALL_ID,
	  .dst_id = ADDR11_ID,
	  .args   = 1,
	  .length = 2,
	  .handler = acall_handler,
	},
	{ .op_id  = MOVX_ID,
	  .dst_id = I_R0_ID,
	  .src_id = A_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = movx_handler,
	},
	{ .op_id  = MOVX_ID,
	  .dst_id = I_R1_ID,
	  .src_id = A_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = movx_handler,
	},
	{ .op_id  = CPL_ID,
	  .dst_id = A_ID,
	  .args   = 1,
	  .length = 1,
	  .handler = cpl_handler,
	},
	{ .op_id  = MOV_ID,
	  .dst_id = DIRECT_ID,
	  .src_id = A_ID,
	  .args   = 2,
	  .length = 2,
	  .handler = mov_handler,
	},
	{ .op_id  = MOV_ID,
	  .dst_id = I_R0_ID,
	  .src_id = A_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = mov_handler,
	},
	{ .op_id  = MOV_ID,
	  .dst_id = I_R1_ID,
	  .src_id = A_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = mov_handler,
	},
	{ .op_id  = MOV_ID,
	  .dst_id = R0_ID,
	  .src_id = A_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = mov_handler,
	},
	{ .op_id  = MOV_ID,
	  .dst_id = R1_ID,
	  .src_id = A_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = mov_handler,
	},
	{ .op_id  = MOV_ID,
	  .dst_id = R2_ID,
	  .src_id = A_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = mov_handler,
	},
	{ .op_id  = MOV_ID,
	  .dst_id = R3_ID,
	  .src_id = A_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = mov_handler,
	},
	{ .op_id  = MOV_ID,
	  .dst_id = R4_ID,
	  .src_id = A_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = mov_handler,
	},
	{ .op_id  = MOV_ID,
	  .dst_id = R5_ID,
	  .src_id = A_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = mov_handler,
	},
	{ .op_id  = MOV_ID,
	  .dst_id = R6_ID,
	  .src_id = A_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = mov_handler,
	},
	{ .op_id  = MOV_ID,
	  .dst_id = R7_ID,
	  .src_id = A_ID,
	  .args   = 2,
	  .length = 1,
	  .handler = mov_handler,
	},
};
