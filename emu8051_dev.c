#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ncurses.h>
#include "emu8051_defs.h"

uint8_t get_pmem_bank(struct emu8051_dev *emu_dev, uint16_t addr)
{
	uint8_t bank;

	bank = emu_dev->sfr[BANK_SW];

	switch (addr) {
	case 0x8000 ... 0x9fff:
		bank &= 0x03;
		break;
	case 0xa000 ... 0xbfff:
		bank = (bank >> 2) & 0x03;
		break;
	case 0xc000 ... 0xdfff:
		bank = (bank >> 4) & 0x03;
		break;
	}

	return bank;
}

/*
 * With the 8051 onboard the ca0132, program memory is segmented into banks:
 * | 0000 - 7fff | ROM 0             |
 * | 8000 - 9fff | Bankswitched      |
 * | a000 - bfff | Bankswitched      |
 * | c000 - dfff | Bankswitched      |
 * | e000 - efff | ROM 3(?)          |
 * | f000 - ffff | EXRAM f000 - ffff |
 */
uint8_t get_pmem(struct emu8051_dev *emu_dev, uint16_t addr)
{
	uint8_t data;

	switch (addr) {
	case 0x0000 ... 0x7fff:
		data = emu_dev->pmem[addr];
		break;
	case 0x8000 ... 0xdfff:
		// Need to check for bankswitching on this and next two.
		switch (get_pmem_bank(emu_dev, addr)) {
		case 0:
			data = emu_dev->xram[addr];
			break;
		case 1:
			data = emu_dev->pmem_b0[addr - 0x8000];
			break;
		case 2:
			data = emu_dev->pmem_b1[addr - 0x8000];
			break;
		case 3:
			data = 0;
			break;
		}
		break;
	case 0xe000 ... 0xffff:
		data = emu_dev->xram[addr];
		break;
	}

	return data;
}

uint8_t get_psw_flag(struct emu8051_dev *emu_dev, uint8_t flag)
{
	if (emu_dev->sfr[PSW] & flag)
		return 1;
	else
		return 0;
}

void set_psw_flag(struct emu8051_dev *emu_dev, uint8_t flag)
{
	op_change_write(emu_dev, S_WRITE, PSW);

	emu_dev->sfr[PSW] |= flag;
}

void clr_psw_flag(struct emu8051_dev *emu_dev, uint8_t flag)
{
	op_change_write(emu_dev, S_WRITE, PSW);

	emu_dev->sfr[PSW] &= ~flag;
}

void push(struct emu8051_dev *emu_dev, uint8_t data)
{
	op_change_write(emu_dev, S_WRITE, SP);

	indirect_write(emu_dev, ++emu_dev->sfr[SP], data);
}

uint8_t pop(struct emu8051_dev *emu_dev)
{
	op_change_write(emu_dev, S_WRITE, SP);

	return emu_dev->iram[emu_dev->sfr[SP]--];
}

void write_sfr(struct emu8051_dev *emu_dev, uint8_t sfr, uint8_t data)
{
	op_change_write(emu_dev, S_WRITE, sfr);

	switch (sfr) {
	case ACC:
		if (data % 2)
			set_psw_flag(emu_dev, PARITY_FLAG);
		else
			clr_psw_flag(emu_dev, PARITY_FLAG);

		emu_dev->sfr[sfr] = data;
		break;
	default:
		break;
	}

	emu_dev->sfr[sfr] = data;
}

uint8_t get_reg_bank(struct emu8051_dev *emu_dev)
{
	uint8_t reg_bank;

	reg_bank = (emu_dev->sfr[PSW] >> 3) & 0x03;

	return reg_bank;
}

/* Check the register bank currently set, then write to the register. */
void set_reg(struct emu8051_dev *emu_dev, uint8_t reg,
		uint8_t data)
{
	uint8_t reg_offset;

	op_change_write(emu_dev, R_WRITE, reg);

	reg_offset = get_reg_bank(emu_dev) * 8;

	emu_dev->iram[reg + reg_offset] = data;
}

uint8_t get_reg(struct emu8051_dev *emu_dev, uint8_t reg)
{
	uint8_t reg_offset;

	reg_offset = get_reg_bank(emu_dev) * 8;

	return emu_dev->iram[reg + reg_offset];
}

void indirect_write(struct emu8051_dev *emu_dev, uint8_t addr, uint8_t data)
{
	op_change_write(emu_dev, I_WRITE, addr);

	emu_dev->iram[addr] = data;
}

uint8_t direct_read(struct emu8051_dev *emu_dev, uint8_t addr)
{
	if (addr < 0x80)
		return emu_dev->iram[addr];
	else {
		switch (addr) {
		case 0xdf:
			if (emu_dev->verb_pos > 0)
				return emu_dev->verb_sfr[emu_dev->verb_pos--];
			else
				return emu_dev->verb_sfr[0];
		case 0xf9:
			return 0x40;
		default:
			return emu_dev->sfr[addr - 0x80];
		}
	}
}

void direct_write(struct emu8051_dev *emu_dev, uint8_t addr, uint8_t data)
{
	if (addr < 0x80) {
		op_change_write(emu_dev, D_WRITE, addr);
		emu_dev->iram[addr] = data;
	} else
		write_sfr(emu_dev, addr - 0x80, data);
}

/*
 * Addresses 0x00 through 0x7f are mapped to 0x20-0x2f in iram.
 * Each byte has an address per bit. 0x80-0xff are mapped to certain
 * sfrs, which will get interesting.
 */
uint8_t get_bit(struct emu8051_dev *emu_dev, uint8_t addr)
{
	uint8_t byte, bit;

	bit = addr % 8;
	if (addr < 0x80) {
		byte = 0x20 + (addr / 8);
		return (emu_dev->iram[byte] >> bit) & 0x01;
	} else {
		byte = (addr / 8) * 8 - 0x80;
		return (emu_dev->sfr[byte] >> bit) & 0x01;
	}
}

void set_bit(struct emu8051_dev *emu_dev, uint8_t addr, uint8_t set)
{
	uint8_t byte, bit, bitmask;

	bit = addr % 8;
	bitmask = 0x01 << bit;
	if (addr < 0x80) {
		byte = 0x20 + (addr / 8);
		if (set)
			indirect_write(emu_dev, byte, emu_dev->iram[byte] | bitmask);
		else
			indirect_write(emu_dev, byte, emu_dev->iram[byte] & ~bitmask);
	} else {
		byte = (addr / 8) * 8 - 0x80;
		if (set)
			write_sfr(emu_dev, byte, emu_dev->sfr[byte] | bitmask);
		else
			write_sfr(emu_dev, byte, emu_dev->sfr[byte] & ~bitmask);
	}
}

uint16_t get_dptr(struct emu8051_dev *emu_dev)
{
	uint16_t dptr;

	if (emu_dev->sfr[DP_TGL] & 0x01) {
		dptr = emu_dev->sfr[DPL_1];
		dptr |= emu_dev->sfr[DPH_1] << 8;
	} else {
		dptr = emu_dev->sfr[DPL_0];
		dptr |= emu_dev->sfr[DPH_0] << 8;
	}

	return dptr;
}

void set_dptr(struct emu8051_dev *emu_dev, uint16_t dptr)
{
	if (emu_dev->sfr[DP_TGL] & 0x01) {
		write_sfr(emu_dev, DPL_1, dptr & 0xff);
		write_sfr(emu_dev, DPH_1, dptr >> 8);
	} else {
		write_sfr(emu_dev, DPL_0, dptr & 0xff);
		write_sfr(emu_dev, DPH_0, dptr >> 8);
	}
}

void setup_verb_handler(struct emu8051_data *emu_data, uint8_t nid,
			uint16_t verb, uint8_t val)
{
	struct emu8051_dev *dev = emu_data->emu_dev;
	struct op_change op_ch;

	dev->op_ch = &op_ch;

	dev->op_ch->changes = 0;
	dev->op_ch->pc = dev->pc;

	push(dev, dev->pc & 0xff);
	push(dev, dev->pc >> 8);

	dev->pc = 0x0003;

	direct_write(dev, 0xe1, 0x01);

	direct_write(dev, 0xdc, val);

	/* The first value out is node. */
	dev->verb_pos = 2;

	dev->verb_sfr[2] = nid;
	dev->verb_sfr[1] = verb >> 8;
	dev->verb_sfr[0] = verb & 0xff;

	write_front(&emu_data->dyn_array, op_ch);

	dev->op_ch = NULL;
}
