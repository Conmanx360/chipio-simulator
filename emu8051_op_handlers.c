#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ncurses.h>
#include "emu8051_defs.h"


void acall_handler(struct emu8051_dev *emu_dev)
{
	uint16_t ret_addr, jmp_addr, tmp;

	op_change_write(emu_dev, S_WRITE, SP);

	/* Push return address onto the stack, low first, high second. */
	ret_addr = emu_dev->pc + 2;

	push(emu_dev, ret_addr & 0xff);
	push(emu_dev, ret_addr >> 8);

	/* High-order bits 15-11 come from incremented PC's 15-11 bits */
	jmp_addr = ret_addr & 0xf8;

	/* Bits 10-8 come from 3 high order bits in the opcode. */
	tmp = (get_pmem(emu_dev, emu_dev->pc) >> 5) & 0x07;
	jmp_addr |= (tmp << 8);

	/* Bits 7-0 come from second byte of instruction. */
	jmp_addr |= get_pmem(emu_dev, emu_dev->pc + 1);

	emu_dev->pc = jmp_addr;
}

static void add_set_flags(struct emu8051_dev *emu_dev, uint16_t tmp,
		uint8_t acc, uint8_t val)
{
	if (tmp > 0xff)
		set_psw_flag(emu_dev, CARRY_FLAG);
	else
		clr_psw_flag(emu_dev, CARRY_FLAG);

	if (((acc % 16) + val) > 15)
		set_psw_flag(emu_dev, AUX_CY_FLAG);
	else
		clr_psw_flag(emu_dev, AUX_CY_FLAG);

	if ((acc & 0x80) && (val & 0x80)) {
		if (!(tmp & 0x80))
			set_psw_flag(emu_dev, OVFLOW_FLAG);
	} else if (!(acc & 0x80) && !(val & 0x80)) {
		if ((tmp & 0x80))
			set_psw_flag(emu_dev, OVFLOW_FLAG);
	} else
		clr_psw_flag(emu_dev, OVFLOW_FLAG);
}

void add_handler(struct emu8051_dev *emu_dev)
{
	uint16_t tmp;
	uint8_t acc, val;

	acc = emu_dev->sfr[ACC];

	switch (get_pmem(emu_dev, emu_dev->pc)) {
	case 0x24:
		val = get_pmem(emu_dev, emu_dev->pc + 1);

		emu_dev->pc += 2;
		break;
	case 0x25:
		val = direct_read(emu_dev, get_pmem(emu_dev, emu_dev->pc + 1));

		emu_dev->pc += 2;
		break;
	case 0x26:
	case 0x27:
		tmp = get_reg(emu_dev, get_pmem(emu_dev, emu_dev->pc) - 0x26);
		val = emu_dev->iram[tmp];

		emu_dev->pc++;
		break;
	case 0x28 ... 0x2f:
		val = get_reg(emu_dev, get_pmem(emu_dev, emu_dev->pc) - 0x28);

		emu_dev->pc++;
		break;
	}

	tmp = acc + val;

	add_set_flags(emu_dev, tmp, acc, val);

	write_sfr(emu_dev, ACC, tmp & 0xff);
}

void addc_handler(struct emu8051_dev *emu_dev)
{
	uint16_t tmp;
	uint8_t acc, val;

	acc = emu_dev->sfr[ACC];

	switch (get_pmem(emu_dev, emu_dev->pc)) {
	case 0x34:
		val = get_pmem(emu_dev, emu_dev->pc + 1);

		emu_dev->pc += 2;
		break;
	case 0x35:
		val = direct_read(emu_dev, get_pmem(emu_dev, emu_dev->pc + 1));

		emu_dev->pc += 2;
		break;
	case 0x36:
	case 0x37:
		tmp = get_reg(emu_dev, get_pmem(emu_dev, emu_dev->pc) - 0x36);
		val = emu_dev->iram[tmp];

		emu_dev->pc++;
		break;
	case 0x38 ... 0x3f:
		val = get_reg(emu_dev, get_pmem(emu_dev, emu_dev->pc) - 0x38);

		emu_dev->pc++;
		break;
	}

	if (get_psw_flag(emu_dev, CARRY_FLAG)) {
		clr_psw_flag(emu_dev, CARRY_FLAG);
		tmp = acc + val + 1;
	} else
		tmp = acc + val;

	add_set_flags(emu_dev, tmp, acc, val);

	write_sfr(emu_dev, ACC, tmp & 0xff);
}

void ajmp_handler(struct emu8051_dev *emu_dev)
{
	uint16_t tmp = 0;

	tmp |= ((emu_dev->pc + 2) & 0xF800);
	tmp |= (get_pmem(emu_dev, emu_dev->pc) >> 5) << 8;
	tmp |= get_pmem(emu_dev, emu_dev->pc + 1);

	emu_dev->pc = tmp;
}

void anl_handler(struct emu8051_dev *emu_dev)
{
	uint8_t dst, src, op;

	op = get_pmem(emu_dev, emu_dev->pc);

	switch (op) {
	case 0x52:
		src = emu_dev->sfr[ACC];
		dst = direct_read(emu_dev, get_pmem(emu_dev, emu_dev->pc + 1));

		write_sfr(emu_dev, ACC, dst & src);

		emu_dev->pc += 2;
		break;
	case 0x53:
		src = get_pmem(emu_dev, emu_dev->pc + 2);
		dst = direct_read(emu_dev, get_pmem(emu_dev, emu_dev->pc + 1));

		direct_write(emu_dev, get_pmem(emu_dev, emu_dev->pc + 1), src & dst);

		emu_dev->pc += 3;
		break;
	case 0x54:
		src = emu_dev->sfr[ACC];
		dst = get_pmem(emu_dev, emu_dev->pc + 1);

		write_sfr(emu_dev, ACC, dst & src);

		emu_dev->pc += 2;
		break;
	case 0x55:
		src = emu_dev->sfr[ACC];
		dst = direct_read(emu_dev, get_pmem(emu_dev, emu_dev->pc + 1));

		write_sfr(emu_dev, ACC, dst & src);

		emu_dev->pc += 2;
		break;
	case 0x56:
	case 0x57:
		src = emu_dev->iram[get_reg(emu_dev, op - 0x56)];
		dst = emu_dev->sfr[ACC];

		write_sfr(emu_dev, ACC, dst & src);

		emu_dev->pc++;
		break;
	case 0x58 ... 0x5f:
		src = get_reg(emu_dev, get_pmem(emu_dev, emu_dev->pc) - 0x58);
		dst = emu_dev->sfr[ACC];

		write_sfr(emu_dev, ACC, dst & src);

		emu_dev->pc++;
		break;
	case 0x82:
		if (get_bit(emu_dev, get_pmem(emu_dev, emu_dev->pc + 1)))
			set_psw_flag(emu_dev, CARRY_FLAG);
		else
			clr_psw_flag(emu_dev, CARRY_FLAG);

		emu_dev->pc += 2;
		break;
	case 0xb0:
		if (!get_bit(emu_dev, get_pmem(emu_dev, emu_dev->pc + 1)))
			set_psw_flag(emu_dev, CARRY_FLAG);
		else
			clr_psw_flag(emu_dev, CARRY_FLAG);

		emu_dev->pc += 2;
		break;
	}
}

void cjne_handler(struct emu8051_dev *emu_dev)
{
	uint8_t src, dst, tmp;
	int8_t rel_offset;

	switch (get_pmem(emu_dev, emu_dev->pc)) {
	case 0xb4:
		dst = emu_dev->sfr[ACC];
		src = get_pmem(emu_dev, emu_dev->pc + 1);
		break;
	case 0xb5:
		dst = emu_dev->sfr[ACC];
		src = direct_read(emu_dev, get_pmem(emu_dev, emu_dev->pc + 1));
		break;
	case 0xb6:
	case 0xb7:
		tmp = get_reg(emu_dev, get_pmem(emu_dev, emu_dev->pc) - 0xb6);
		dst = emu_dev->iram[tmp];
		src = get_pmem(emu_dev, emu_dev->pc + 1);
		break;
	case 0xb8 ... 0xbf:
		dst = get_reg(emu_dev, get_pmem(emu_dev, emu_dev->pc) - 0xb8);
		src = get_pmem(emu_dev, emu_dev->pc + 1);
		break;
	}

	rel_offset = get_pmem(emu_dev, emu_dev->pc + 2);

	emu_dev->pc += 3;


	if (src == dst)
		clr_psw_flag(emu_dev, CARRY_FLAG);
	else if (dst < src) {
		emu_dev->pc += rel_offset;
		set_psw_flag(emu_dev, CARRY_FLAG);
	} else {
		emu_dev->pc += rel_offset;
		clr_psw_flag(emu_dev, CARRY_FLAG);
	}
}

void clr_handler(struct emu8051_dev *emu_dev)
{
	switch (get_pmem(emu_dev, emu_dev->pc)) {
	case 0xc2:
		set_bit(emu_dev, get_pmem(emu_dev, emu_dev->pc + 1), 0);

		emu_dev->pc += 2;
		break;
	case 0xc3:
		clr_psw_flag(emu_dev, CARRY_FLAG);

		emu_dev->pc++;
		break;
	case 0xe4:
		write_sfr(emu_dev, ACC, 0x00);

		emu_dev->pc++;
		break;
	}
}

/*
 * Complement, basically, invert the bit. If it's 1, then set it to 0, and
 * vice versa.
 */
void cpl_handler(struct emu8051_dev *emu_dev)
{
	uint8_t bit;

	switch (get_pmem(emu_dev, emu_dev->pc)) {
	case 0xb2:
		bit = get_bit(emu_dev, get_pmem(emu_dev, emu_dev->pc + 1));
		set_bit(emu_dev, get_pmem(emu_dev, emu_dev->pc + 1), !bit);

		emu_dev->pc += 2;
		break;
	case 0xb3:
		clr_psw_flag(emu_dev, CARRY_FLAG);

		emu_dev->pc++;
		break;
	case 0xf4:
		write_sfr(emu_dev, ACC, ~emu_dev->sfr[ACC]);

		emu_dev->pc++;
		break;
	}
}

/*
 * Decimal adjust instruction, for binary coded decimal. We need to check for
 * a few things:
 * First, if the first four bits are above nine, we need to add 6 to roll them
 * over. Alternatively, if the aux carry flag is set, we need to do the same thing.
 * Then, we check the upper 4 bits, if there's a rollover, set the carry flag, or,
 * if the value is larger than 9, add 6, check for overflow.
 */
void da_handler(struct emu8051_dev *emu_dev)
{
	uint16_t tmp;

	tmp = emu_dev->sfr[ACC];

	if (get_psw_flag(emu_dev, AUX_CY_FLAG))
		tmp += 0x06;
	else if ((tmp & 0x0f) > 0x09)
		tmp += 0x06;

	if (((tmp & 0xf0) >> 4) > 0x09)
		tmp += 0x60;

	if (tmp > 0xff)
		set_psw_flag(emu_dev, CARRY_FLAG);

	write_sfr(emu_dev, ACC, tmp & 0xff);

	emu_dev->pc++;
}

void dec_handler(struct emu8051_dev *emu_dev)
{
	uint8_t val;

	switch (get_pmem(emu_dev, emu_dev->pc)) {
	case 0x14:
//		emu_dev->sfr[ACC]--;
		val = emu_dev->sfr[ACC];
		write_sfr(emu_dev, ACC, val - 1);

		emu_dev->pc++;
		break;
	case 0x15:
		val = direct_read(emu_dev, get_pmem(emu_dev, emu_dev->pc + 1));
		direct_write(emu_dev, get_pmem(emu_dev, emu_dev->pc + 1), --val);

		emu_dev->pc += 2;
		break;
	case 0x16:
	case 0x17:
		val = get_reg(emu_dev, get_pmem(emu_dev, emu_dev->pc) - 0x16);
		indirect_write(emu_dev, val, emu_dev->iram[val] - 1);

		emu_dev->pc++;
		break;
	case 0x18 ... 0x1f:
		val = get_reg(emu_dev, get_pmem(emu_dev, emu_dev->pc) - 0x18);
		set_reg(emu_dev, get_pmem(emu_dev, emu_dev->pc) - 0x18, --val);

		emu_dev->pc++;
		break;
	}
}

void div_handler(struct emu8051_dev *emu_dev)
{
	uint8_t a, b;

	a = emu_dev->sfr[ACC];
	b = emu_dev->sfr[SFR_B];

	write_sfr(emu_dev, ACC, a / b);
	write_sfr(emu_dev, SFR_B, a % b);
//	emu_dev->sfr[SFR_B] = a % b;

	clr_psw_flag(emu_dev, CARRY_FLAG);

	if (b == 0)
		set_psw_flag(emu_dev, OVFLOW_FLAG);
	else
		clr_psw_flag(emu_dev, OVFLOW_FLAG);

	emu_dev->pc++;
}

/*
 * Difference between this and cjne: only one variable is used, and compared
 * against 0. cjne always uses two separate variables.
 */
void djnz_handler(struct emu8051_dev *emu_dev)
{
	uint8_t val, op;
	int8_t rel_offset;

	op = get_pmem(emu_dev, emu_dev->pc);

	switch (op) {
	case 0xd5:
		val = direct_read(emu_dev, get_pmem(emu_dev, emu_dev->pc + 1));
		direct_write(emu_dev, get_pmem(emu_dev, emu_dev->pc + 1), --val);
		rel_offset = get_pmem(emu_dev, emu_dev->pc + 2);

		emu_dev->pc += 3;
		break;
	case 0xd8 ... 0xdf:
		val = get_reg(emu_dev, op - 0xd8);
		set_reg(emu_dev, op - 0xd8, --val);
		rel_offset = get_pmem(emu_dev, emu_dev->pc + 1);

		emu_dev->pc += 2;
		break;
	}

	if (val != 0)
		emu_dev->pc += rel_offset;
	else
		return;
}

void inc_handler(struct emu8051_dev *emu_dev)
{
	uint16_t tmp;
	uint8_t val, op;

	op = get_pmem(emu_dev, emu_dev->pc);
	switch (op) {
	case 0x04:
		val = emu_dev->sfr[ACC];
		write_sfr(emu_dev, ACC, ++val);
//		emu_dev->sfr[ACC]++;

		emu_dev->pc++;
		break;
	case 0x05:
		val = direct_read(emu_dev, get_pmem(emu_dev, emu_dev->pc + 1));
		direct_write(emu_dev, get_pmem(emu_dev, emu_dev->pc + 1), ++val);

		emu_dev->pc += 2;
		break;
	case 0x06:
	case 0x07:
		val = get_reg(emu_dev, op - 0x06);
//		emu_dev->iram[val]++;
		indirect_write(emu_dev, val, emu_dev->iram[val] + 1);

		emu_dev->pc++;
		break;
	case 0x08 ... 0x0f:
		val = get_reg(emu_dev, op - 0x08); set_reg(emu_dev, op - 0x08, ++val);

		emu_dev->pc++;
		break;
	case 0xa3:
		tmp = get_dptr(emu_dev);
		set_dptr(emu_dev, ++tmp);

		emu_dev->pc++;
		break;
	}
}

void jb_handler(struct emu8051_dev *emu_dev)
{
	int8_t rel_addr = get_pmem(emu_dev, emu_dev->pc + 2);

	if (get_bit(emu_dev, get_pmem(emu_dev, emu_dev->pc + 1)))
		emu_dev->pc += rel_addr;

	emu_dev->pc += 3;
}

void jbc_handler(struct emu8051_dev *emu_dev)
{
	int8_t rel_addr = get_pmem(emu_dev, emu_dev->pc + 2);

	if (get_bit(emu_dev, get_pmem(emu_dev, emu_dev->pc + 1))) {
		emu_dev->pc += rel_addr;
		set_bit(emu_dev, get_pmem(emu_dev, emu_dev->pc + 1), 0);
	}

	emu_dev->pc += 3;
}

void jc_handler(struct emu8051_dev *emu_dev)
{
	int8_t rel_addr = get_pmem(emu_dev, emu_dev->pc + 1);

	if (get_psw_flag(emu_dev, CARRY_FLAG))
		emu_dev->pc += rel_addr;

	emu_dev->pc += 2;
}

void jmp_handler(struct emu8051_dev *emu_dev)
{
	emu_dev->pc = get_dptr(emu_dev) + emu_dev->sfr[ACC];
}

void jnb_handler(struct emu8051_dev *emu_dev)
{
	int8_t rel_addr = get_pmem(emu_dev, emu_dev->pc + 2);

	if (!get_bit(emu_dev, get_pmem(emu_dev, emu_dev->pc + 1))) {
		emu_dev->pc += rel_addr;
	}

	emu_dev->pc += 3;

}

void jnc_handler(struct emu8051_dev *emu_dev)
{
	int8_t rel_addr = get_pmem(emu_dev, emu_dev->pc + 1);

	if (!get_psw_flag(emu_dev, CARRY_FLAG))
		emu_dev->pc += rel_addr;

	emu_dev->pc += 2;
}

void jnz_handler(struct emu8051_dev *emu_dev)
{
	int8_t rel_addr = get_pmem(emu_dev, emu_dev->pc + 1);

	if (emu_dev->sfr[ACC])
		emu_dev->pc += rel_addr;

	emu_dev->pc += 2;
}

void jz_handler(struct emu8051_dev *emu_dev)
{
	int8_t rel_addr = get_pmem(emu_dev, emu_dev->pc + 1);

	if (!emu_dev->sfr[ACC])
		emu_dev->pc += rel_addr;

	emu_dev->pc += 2;
}

void lcall_handler(struct emu8051_dev *emu_dev)
{
	uint16_t addr;

	addr = get_pmem(emu_dev, emu_dev->pc + 2);
	addr |= (get_pmem(emu_dev, emu_dev->pc + 1) << 8);

	emu_dev->pc += 3;

	push(emu_dev, emu_dev->pc & 0xFF);
	push(emu_dev, emu_dev->pc >> 8);

	emu_dev->pc = addr;
}

void ljmp_handler(struct emu8051_dev *emu_dev)
{
	uint16_t addr;

	addr = get_pmem(emu_dev, emu_dev->pc + 2);
	addr |= (get_pmem(emu_dev, emu_dev->pc + 1) << 8);

	emu_dev->pc = addr;
}

void mov_handler(struct emu8051_dev *emu_dev)
{
	uint8_t op, tmp1, tmp2;
	uint16_t tmp;

	op = get_pmem(emu_dev, emu_dev->pc);

	switch (op) {
	case 0x74:
		write_sfr(emu_dev, ACC, get_pmem(emu_dev, emu_dev->pc + 1));
		emu_dev->pc += 2;
		break;
	case 0x75:
		tmp1 = get_pmem(emu_dev, emu_dev->pc + 1);
		tmp2 = get_pmem(emu_dev, emu_dev->pc + 2);
		direct_write(emu_dev, tmp1, tmp2);

		emu_dev->pc += 3;
		break;
	case 0x76:
	case 0x77:
		tmp1 = get_reg(emu_dev, op - 0x76);
		tmp2 = get_pmem(emu_dev, emu_dev->pc + 1);
		indirect_write(emu_dev, tmp1, tmp2);
//		emu_dev->iram[tmp1] = tmp2;

		emu_dev->pc += 2;
		break;
	case 0x78 ... 0x7f:
		tmp1 = op - 0x78;
		tmp2 = get_pmem(emu_dev, emu_dev->pc + 1);
		set_reg(emu_dev, tmp1, tmp2);

		emu_dev->pc += 2;
		break;
	case 0x85:
		tmp1 = get_pmem(emu_dev, emu_dev->pc + 1);
		tmp2 = get_pmem(emu_dev, emu_dev->pc + 2);
		direct_write(emu_dev, tmp2, direct_read(emu_dev, tmp1));

		emu_dev->pc += 3;
		break;
	case 0x86:
	case 0x87:
		tmp1 = get_reg(emu_dev, op - 0x86);
		tmp2 = get_pmem(emu_dev, emu_dev->pc + 1);

		direct_write(emu_dev, tmp2, emu_dev->iram[tmp1]);

		emu_dev->pc += 2;
		break;
	case 0x88 ... 0x8f:
		tmp1 = get_reg(emu_dev, op - 0x88);
		tmp2 = get_pmem(emu_dev, emu_dev->pc + 1);

		direct_write(emu_dev, tmp2, tmp1);

		emu_dev->pc += 2;
		break;
	case 0x90:
		tmp = get_pmem(emu_dev, emu_dev->pc + 1) << 8;
		tmp |= get_pmem(emu_dev, emu_dev->pc + 2);

		set_dptr(emu_dev, tmp);

		emu_dev->pc += 3;
		break;
	case 0x92:
		tmp1 = get_psw_flag(emu_dev, CARRY_FLAG);
		tmp2 = get_pmem(emu_dev, emu_dev->pc + 1);

		set_bit(emu_dev, tmp2, tmp1);

		emu_dev->pc += 2;
		break;
	case 0xa2:
		tmp1 = get_pmem(emu_dev, emu_dev->pc + 1);
		tmp2 = get_bit(emu_dev, tmp1);

		if (tmp2)
			set_psw_flag(emu_dev, CARRY_FLAG);
		else
			clr_psw_flag(emu_dev, CARRY_FLAG);

		emu_dev->pc += 2;
		break;
	case 0xa6:
	case 0xa7:
		tmp1 = get_reg(emu_dev, op - 0xa6);
		tmp2 = direct_read(emu_dev, get_pmem(emu_dev, emu_dev->pc + 1));

//		emu_dev->iram[tmp1] = tmp2;
		indirect_write(emu_dev, tmp1, tmp2);

		emu_dev->pc += 2;
		break;
	case 0xa8 ... 0xaf:
		tmp1 = op - 0xa8;
		tmp2 = direct_read(emu_dev, get_pmem(emu_dev, emu_dev->pc + 1));

		set_reg(emu_dev, tmp1, tmp2);

		emu_dev->pc += 2;
		break;
	case 0xe5:
		tmp1 = direct_read(emu_dev, get_pmem(emu_dev, emu_dev->pc + 1));

		write_sfr(emu_dev, ACC, tmp1);

		emu_dev->pc += 2;
		break;
	case 0xe6:
	case 0xe7:
		tmp1 = get_reg(emu_dev, op - 0xe6);

		write_sfr(emu_dev, ACC, emu_dev->iram[tmp1]);

		emu_dev->pc++;
		break;
	case 0xe8 ... 0xef:
		tmp1 = get_reg(emu_dev, op - 0xe8);

		write_sfr(emu_dev, ACC, tmp1);

		emu_dev->pc++;
		break;
	case 0xf5:
		tmp1 = emu_dev->sfr[ACC];
		tmp2 = get_pmem(emu_dev, emu_dev->pc + 1);

		direct_write(emu_dev, tmp2, tmp1);

		emu_dev->pc += 2;
		break;
	case 0xf6:
	case 0xf7:
		tmp1 = emu_dev->sfr[ACC];
		tmp2 = get_reg(emu_dev, op - 0xf6);

		indirect_write(emu_dev, tmp2, tmp1);
//		emu_dev->iram[tmp2] = tmp1;

		emu_dev->pc++;
		break;
	case 0xf8 ... 0xff:
		tmp1 = emu_dev->sfr[ACC];
		tmp2 = op - 0xf8;

		set_reg(emu_dev, tmp2, tmp1);

		emu_dev->pc++;
		break;
	}
}

void movc_handler(struct emu8051_dev *emu_dev)
{
	uint16_t addr;
	uint8_t op;

	op = get_pmem(emu_dev, emu_dev->pc);

	switch (op) {
	case 0x83:
		addr = (emu_dev->pc + 1) + emu_dev->sfr[ACC];

		write_sfr(emu_dev, ACC, get_pmem(emu_dev, addr));

		emu_dev->pc++;
		break;
	case 0x93:
		addr = get_dptr(emu_dev);
		addr += emu_dev->sfr[ACC];

		write_sfr(emu_dev, ACC, get_pmem(emu_dev, addr));

		emu_dev->pc++;
		break;
	}
}

void movx_handler(struct emu8051_dev *emu_dev)
{
	uint16_t dptr;
	uint8_t op, tmp;

	op = get_pmem(emu_dev, emu_dev->pc);

	dptr = get_dptr(emu_dev);

	switch (op) {
	case 0xe0:
		write_sfr(emu_dev, ACC, emu_dev->xram[dptr]);
		break;
	case 0xe2:
	case 0xe3:
		tmp = get_reg(emu_dev, op - 0xe2);

		write_sfr(emu_dev, ACC, emu_dev->xram[tmp]);
		break;
	case 0xf0:
		op_change_write(emu_dev, E_WRITE, 0); // Address doesn't matter.
		emu_dev->xram[dptr] = emu_dev->sfr[ACC];
		break;
	case 0xf2:
	case 0xf3:
		tmp = get_reg(emu_dev, op - 0xf2);
		op_change_write(emu_dev, ER_WRITE, tmp);

		emu_dev->xram[tmp] = emu_dev->sfr[ACC];
		break;
	}

	emu_dev->pc++;
}

void mul_handler(struct emu8051_dev *emu_dev)
{
	uint16_t res;
	uint8_t a, b;

	a = emu_dev->sfr[ACC];
	b = emu_dev->sfr[SFR_B];

	res = a * b;

	write_sfr(emu_dev, ACC, res & 0xff);
	write_sfr(emu_dev, SFR_B, res >> 8);

	clr_psw_flag(emu_dev, CARRY_FLAG);

	if (res > 0xff)
		set_psw_flag(emu_dev, OVFLOW_FLAG);
	else
		clr_psw_flag(emu_dev, OVFLOW_FLAG);

	emu_dev->pc++;
}

void orl_handler(struct emu8051_dev *emu_dev)
{
	uint8_t op, dst, src;

	op = get_pmem(emu_dev, emu_dev->pc);

	switch (op) {
	case 0x42:
		dst = direct_read(emu_dev, get_pmem(emu_dev, emu_dev->pc + 1));
		src = emu_dev->sfr[ACC];

		dst |= src;
		direct_write(emu_dev, get_pmem(emu_dev, emu_dev->pc + 1), dst);

		emu_dev->pc += 2;
		break;
	case 0x43:
		dst = direct_read(emu_dev, get_pmem(emu_dev, emu_dev->pc + 1));
		src = get_pmem(emu_dev, emu_dev->pc + 2);

		dst |= src;
		direct_write(emu_dev, get_pmem(emu_dev, emu_dev->pc + 1), dst);

		emu_dev->pc += 3;
		break;
	case 0x44:
		dst = emu_dev->sfr[ACC];
		src = get_pmem(emu_dev, emu_dev->pc + 1);

		dst |= src;
		write_sfr(emu_dev, ACC, dst);

		emu_dev->pc += 2;
		break;
	case 0x45:
		dst = emu_dev->sfr[ACC];
		src = direct_read(emu_dev, get_pmem(emu_dev, emu_dev->pc + 1));

		dst |= src;
		write_sfr(emu_dev, ACC, dst);

		emu_dev->pc += 2;
		break;
	case 0x46:
	case 0x47:
		src = emu_dev->iram[get_reg(emu_dev, op - 0x46)];
		dst = emu_dev->sfr[ACC];

		dst |= src;
		write_sfr(emu_dev, ACC, dst);

		emu_dev->pc++;
		break;
	case 0x48 ... 0x4f:
		src = get_reg(emu_dev, get_pmem(emu_dev, emu_dev->pc) - 0x48);
		dst = emu_dev->sfr[ACC];

		dst |= src;
		write_sfr(emu_dev, ACC, dst);

		emu_dev->pc++;
		break;
	case 0x72:
		if (get_bit(emu_dev, get_pmem(emu_dev, emu_dev->pc + 1)))
			set_psw_flag(emu_dev, CARRY_FLAG);

		emu_dev->pc += 2;
		break;
	case 0xa0:
		if (!get_bit(emu_dev, get_pmem(emu_dev, emu_dev->pc + 1)))
			set_psw_flag(emu_dev, CARRY_FLAG);

		emu_dev->pc += 2;
		break;
	}
}

void pop_handler(struct emu8051_dev *emu_dev)
{
	uint8_t dst;

	dst = get_pmem(emu_dev, emu_dev->pc + 1);

	direct_write(emu_dev, dst, pop(emu_dev));

	emu_dev->pc += 2;
}

void push_handler(struct emu8051_dev *emu_dev)
{
	uint8_t src;

	src = direct_read(emu_dev, get_pmem(emu_dev, emu_dev->pc + 1));

	push(emu_dev, src);

	emu_dev->pc += 2;
}

/*
 * These two could just share the same handler. Might leave it as is in case
 * I decide to do something differently.
 */
void ret_handler(struct emu8051_dev *emu_dev)
{
	uint16_t addr;

	addr = pop(emu_dev) << 8;
	addr |= pop(emu_dev);

	emu_dev->pc = addr;
}

void reti_handler(struct emu8051_dev *emu_dev)
{
	uint16_t addr;

	addr = pop(emu_dev) << 8;
	addr |= pop(emu_dev);

	emu_dev->pc = addr;
}

void rl_handler(struct emu8051_dev *emu_dev)
{
	uint8_t tmp;

	tmp = emu_dev->sfr[ACC] << 1 | (emu_dev->sfr[ACC] >> 7);

	write_sfr(emu_dev, ACC, tmp);

	emu_dev->pc++;
}

void rlc_handler(struct emu8051_dev *emu_dev)
{
	uint16_t tmp;

	// So, basically, if bit 7 is set, then set the carry flag. If the
	// carry flag is set, then bit 0 is set.
	tmp = emu_dev->sfr[ACC];
	tmp = tmp << 1;
	if (get_psw_flag(emu_dev, CARRY_FLAG))
		tmp |= 0x01;

	if (tmp & 0x100)
		set_psw_flag(emu_dev, CARRY_FLAG);
	else
		clr_psw_flag(emu_dev, CARRY_FLAG);

	tmp &= 0xff;

	write_sfr(emu_dev, ACC, tmp);
	emu_dev->pc++;
}

void rr_handler(struct emu8051_dev *emu_dev)
{
	uint8_t tmp;

	tmp = emu_dev->sfr[ACC] >> 1 | (emu_dev->sfr[ACC] << 7);

	write_sfr(emu_dev, ACC, tmp);

	emu_dev->pc++;
}

void rrc_handler(struct emu8051_dev *emu_dev)
{
	uint8_t tmp;

	tmp = emu_dev->sfr[ACC] >> 1;
	if (get_psw_flag(emu_dev, CARRY_FLAG))
		tmp |= 0x80;

	if (emu_dev->sfr[ACC] & 0x01)
		set_psw_flag(emu_dev, CARRY_FLAG);
	else
		clr_psw_flag(emu_dev, CARRY_FLAG);

	write_sfr(emu_dev, ACC, tmp);
	emu_dev->pc++;
}

void setb_handler(struct emu8051_dev *emu_dev)
{

	switch(get_pmem(emu_dev, emu_dev->pc)) {
	case 0xd2:
		set_bit(emu_dev, get_pmem(emu_dev, emu_dev->pc + 1), 1);

		emu_dev->pc += 2;
		break;
	case 0xd3:
		set_psw_flag(emu_dev, CARRY_FLAG);

		emu_dev->pc++;
		break;
	}
}

void sjmp_handler(struct emu8051_dev *emu_dev)
{
	int8_t rel_addr = get_pmem(emu_dev, emu_dev->pc + 1);

	emu_dev->pc += rel_addr;
	emu_dev->pc += 2;
}

void subb_handler(struct emu8051_dev *emu_dev)
{
	uint16_t acc, src;
	uint8_t op, res;

	acc = emu_dev->sfr[ACC];

	op = get_pmem(emu_dev, emu_dev->pc);
	switch (op) {
	case 0x94:
		src = get_pmem(emu_dev, emu_dev->pc + 1);

		emu_dev->pc += 2;
		break;
	case 0x95:
		src = direct_read(emu_dev, get_pmem(emu_dev, emu_dev->pc + 1));

		emu_dev->pc += 2;
		break;
	case 0x96:
	case 0x97:
		src = emu_dev->iram[get_reg(emu_dev, op - 0x96)];

		emu_dev->pc++;
		break;
	case 0x98 ... 0x9f:
		src = get_reg(emu_dev, op - 0x98);

		emu_dev->pc++;
		break;
	}

	if (acc < src) {
		acc += 0x100;
		if (get_psw_flag(emu_dev, CARRY_FLAG))
			acc--;

		set_psw_flag(emu_dev, CARRY_FLAG);
	} else {
		if (get_psw_flag(emu_dev, CARRY_FLAG))
			acc--;
		clr_psw_flag(emu_dev, CARRY_FLAG);
	}

	if (acc == 0xffff)
		acc &= 0xff;

	if ((acc & 0x0f) < (src & 0x0f))
		set_psw_flag(emu_dev, AUX_CY_FLAG);
	else
		clr_psw_flag(emu_dev, AUX_CY_FLAG);

	res = acc - src;

	if ((acc & 0x80) && !(src & 0x80) && !(res & 0x80))
		set_psw_flag(emu_dev, OVFLOW_FLAG);
	else if (!(acc & 0x80) && (src & 0x80) && (res & 0x80))
		set_psw_flag(emu_dev, OVFLOW_FLAG);
	else
		clr_psw_flag(emu_dev, OVFLOW_FLAG);

	write_sfr(emu_dev, ACC, res);
}

void swap_handler(struct emu8051_dev *emu_dev)
{
	uint8_t tmp;

	tmp = emu_dev->sfr[ACC];

	write_sfr(emu_dev, ACC, tmp << 4 | tmp >> 4);

	emu_dev->pc++;
}

void xch_handler(struct emu8051_dev *emu_dev)
{
	uint8_t op, acc, tmp;

	acc = emu_dev->sfr[ACC];
	op = get_pmem(emu_dev, emu_dev->pc);

	switch (op) {
	case 0xc5:
		tmp = get_pmem(emu_dev, emu_dev->pc + 1);
		write_sfr(emu_dev, ACC, direct_read(emu_dev, tmp));
		direct_write(emu_dev, tmp, acc);

		emu_dev->pc += 2;
		break;
	case 0xc6:
	case 0xc7:
		tmp = op - 0xc6;
		write_sfr(emu_dev, ACC, emu_dev->iram[get_reg(emu_dev, tmp)]);
//		emu_dev->iram[get_reg(emu_dev, tmp)] = acc;
		indirect_write(emu_dev, get_reg(emu_dev, tmp), acc);

		emu_dev->pc++;
		break;
	case 0xc8 ... 0xcf:
		write_sfr(emu_dev, ACC, get_reg(emu_dev, op - 0xc8));
		set_reg(emu_dev, op - 0xc8, acc);

		emu_dev->pc++;
		break;
	}
}

void xchd_handler(struct emu8051_dev *emu_dev)
{
	uint8_t op, acc, reg, reg_no;

	op = get_pmem(emu_dev, emu_dev->pc);
	reg_no = op - 0xd6;

	acc = emu_dev->sfr[ACC];// & 0xf0;
	reg = emu_dev->iram[get_reg(emu_dev, reg_no)];// & 0xf0;

	write_sfr(emu_dev, ACC, (acc & 0xf0) | (reg & 0x0f));
	indirect_write(emu_dev, get_reg(emu_dev, reg_no), (reg & 0xf0) | (acc & 0x0f));
//	emu_dev->iram[get_reg(emu_dev, reg_no)] = reg | (acc >> 4);

	emu_dev->pc++;
}

void xrl_handler(struct emu8051_dev *emu_dev)
{
	uint8_t op, dst, src;

	op = get_pmem(emu_dev, emu_dev->pc);

	switch (op) {
	case 0x62:
		src = emu_dev->sfr[ACC];
		dst = direct_read(emu_dev, get_pmem(emu_dev, emu_dev->pc + 1));

		write_sfr(emu_dev, ACC, dst ^ src);

		emu_dev->pc += 2;
		break;
	case 0x63:
		src = get_pmem(emu_dev, emu_dev->pc + 2);
		dst = direct_read(emu_dev, get_pmem(emu_dev, emu_dev->pc + 1));

		direct_write(emu_dev, get_pmem(emu_dev, emu_dev->pc + 1), src ^ dst);

		emu_dev->pc += 3;
		break;
	case 0x64:
		src = emu_dev->sfr[ACC];
		dst = get_pmem(emu_dev, emu_dev->pc + 1);

		write_sfr(emu_dev, ACC, dst ^ src);

		emu_dev->pc += 2;
		break;
	case 0x65:
		src = emu_dev->sfr[ACC];
		dst = direct_read(emu_dev, get_pmem(emu_dev, emu_dev->pc + 1));

		write_sfr(emu_dev, ACC, dst ^ src);

		emu_dev->pc += 2;
		break;
	case 0x66:
	case 0x67:
		src = emu_dev->iram[get_reg(emu_dev, op - 0x66)];
		dst = emu_dev->sfr[ACC];

		write_sfr(emu_dev, ACC, dst ^ src);

		emu_dev->pc++;
		break;
	case 0x68 ... 0x6f:
		src = get_reg(emu_dev, op - 0x68);
		dst = emu_dev->sfr[ACC];

		write_sfr(emu_dev, ACC, dst ^ src);

		emu_dev->pc++;
		break;
	}
}

uint8_t handle_opcode(struct emu8051_data *emu_data)
{
	struct emu8051_dev *emu_dev = emu_data->emu_dev;
	uint8_t ret = 0;
	const struct opcode_info *op_info;

	emu_dev->op_ch->changes = 0;
	emu_dev->op_ch->pc = emu_dev->pc;

	op_info = get_op_info(get_pmem(emu_dev, emu_dev->pc));

	if (emu_data->log_data.logging_set)
		logging_check_opcode(emu_data, op_info);

	if (op_info->handler)
		op_info->handler(emu_dev);

	switch (op_info->op_id) {
	case NOP_ID:
		emu_dev->pc++;
		break;
	case RSRV_ID:
		emu_dev->pc++;
		break;
	}

	ret = breakpoint_check(emu_data);

	return ret;
}
