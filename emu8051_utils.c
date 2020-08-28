#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "emu8051_defs.h"

#define EMU8051_DEV_SIZE sizeof(struct emu8051_dev)

void allocate_blocks(struct dynamic_array *dyn_array, uint32_t size);
void grow_dynarray(struct dynamic_array *dyn_array, uint32_t new_size);
char *file_sections[] = {"8051", "PMEM", "PMB0", "PMB1", "XRAM", "IRAM", "SFR ", "BKOP"};
enum file_section_enum {
	FILE_HEADER,
	PMEM_SEC,
	PMEM_B0_SEC,
	PMEM_B1_SEC,
	XRAM_SEC,
	IRAM_SEC,
	SFR_SEC,
	BKOP_SEC,
	NUM_OF_SECS,
};

void write_header(FILE *save_file, uint8_t section)
{
	char *tmp = file_sections[section];

	fwrite(tmp, strlen(tmp), 1, save_file);
}

/*
 * Check current header against all possible headers, return number of
 * sections plus one if we don't have a valid header.
 */
static uint8_t read_header(FILE *file_in)
{
	char hdr[5];
	uint8_t i;

	if (!fread(hdr, 4, 1, file_in))
		return NUM_OF_SECS + 1;

	hdr[4] = '\0';

	for (i = 0; i < NUM_OF_SECS; i++) {
		if (!strcmp(file_sections[i], hdr)) {
			return i;
		}
	}

	return NUM_OF_SECS + 1;
}

void write_save_state(struct emu8051_data *emu_data, FILE *save_file)
{
	struct emu8051_dev *emu_dev = emu_data->emu_dev;
	uint32_t i;

	write_header(save_file, FILE_HEADER);
	fwrite(&emu_dev->pc, sizeof(uint16_t), 1, save_file);

	write_header(save_file, PMEM_SEC);
	fwrite(emu_dev->pmem, sizeof(emu_dev->pmem), 1, save_file);

	write_header(save_file, PMEM_B0_SEC);
	fwrite(emu_dev->pmem_b0, sizeof(emu_dev->pmem_b0), 1, save_file);

	write_header(save_file, PMEM_B1_SEC);
	fwrite(emu_dev->pmem_b1, sizeof(emu_dev->pmem_b1), 1, save_file);

	write_header(save_file, XRAM_SEC);
	fwrite(emu_dev->xram, sizeof(emu_dev->xram), 1, save_file);

	write_header(save_file, IRAM_SEC);
	fwrite(emu_dev->iram, sizeof(emu_dev->iram), 1, save_file);

	write_header(save_file, SFR_SEC);
	fwrite(emu_dev->sfr, sizeof(emu_dev->sfr), 1, save_file);

	write_header(save_file, BKOP_SEC);
	fwrite(&emu_data->dyn_array.count, sizeof(uint32_t), 1, save_file);
	fwrite(&emu_data->dyn_array.size, sizeof(uint32_t), 1, save_file);
	fwrite(&emu_data->dyn_array.start_offset, sizeof(uint32_t), 1, save_file);
	for (i = 0; i < emu_data->dyn_array.size; i++)
		fwrite(emu_data->dyn_array.block[i], sizeof(struct arr_block), 1, save_file);
}

void save_state_to_file(struct emu8051_data *emu_data, char *file_name)
{
	const char *dir_name = "/save-states/";
	FILE *save_file;
	char *pwd, *buf;
	uint32_t i;

	pwd = getenv("PWD");
	if (pwd == NULL)
		return;

	/* Remove trailing spaces. */
	for (i = strlen(file_name) - 1; i != 0; i--) {
		if (file_name[i] == ' ') {
			file_name[i] = '\0';
			continue;
		} else
			break;
	}

	buf = calloc(1, strlen(pwd) + strlen(file_name) + strlen(dir_name) + 1);
	if (buf == NULL)
		return;

	sprintf(buf, "%s%s%s", pwd, dir_name, file_name);

	save_file = fopen(buf, "w+");
	if (save_file == NULL) {
		free(buf);
		return;
	}

	write_save_state(emu_data, save_file);

	free(buf);
	fclose(save_file);
}

int restore_state_from_file(struct emu8051_data *emu_data, FILE *file_in)
{
	struct emu8051_dev *emu_dev = emu_data->emu_dev;
	uint32_t i, new_size;
	uint8_t ret;

	//First thing to do, read 8 bytes, compare it to main header.
	ret = read_header(file_in);
	if (ret != FILE_HEADER)
		return -1;

	if (!fread(&emu_dev->pc, sizeof(uint16_t), 1, file_in))
		return - 1;

	while (!feof(file_in)) {
		ret = read_header(file_in);
		switch (ret) {
		case PMEM_SEC:
			if (!fread(emu_dev->pmem, sizeof(emu_dev->pmem), 1, file_in))
				return -1;
			break;
		case PMEM_B0_SEC:
			if (!fread(emu_dev->pmem_b0, sizeof(emu_dev->pmem_b0), 1, file_in))
				return -1;
			break;
		case PMEM_B1_SEC:
			if (!fread(emu_dev->pmem_b1, sizeof(emu_dev->pmem_b1), 1, file_in))
				return -1;
			break;
		case XRAM_SEC:
			if (!fread(emu_dev->xram, sizeof(emu_dev->xram), 1, file_in))
				return -1;
			break;
		case IRAM_SEC:
			if (!fread(emu_dev->iram, sizeof(emu_dev->iram), 1, file_in))
				return -1;
			break;
		case SFR_SEC:
			if (!fread(emu_dev->sfr, sizeof(emu_dev->sfr), 1, file_in))
				return -1;
			break;
		case BKOP_SEC:
			if (!fread(&emu_data->dyn_array.count, sizeof(uint32_t), 1, file_in))
				return -1;
			if (!fread(&new_size, sizeof(uint32_t), 1, file_in))
				return -1;
			resize_dynarray(&emu_data->dyn_array, (new_size - 1) * ARR_BLOCK_SIZE);
			if (!fread(&emu_data->dyn_array.start_offset, sizeof(uint32_t), 1, file_in))
				return -1;

			for (i = 0; i < emu_data->dyn_array.size; i++) {
				if (!fread(emu_data->dyn_array.block[i],
							sizeof(struct arr_block),
							1, file_in))
					return -1;
			}

			break;
		default:
			break;
		}
	}

	return 0;
}

/*
void backlog_debug_write(FILE *file, struct dynamic_array *dyn_array)
{
        struct arr_block *block;
	struct op_change *op_ch;
	uint32_t max_items = dyn_array->size * ARR_BLOCK_SIZE;
	uint32_t i, block_num;
	char buf[80];

	for (i = 0; i < max_items; i++) {
		block_num = i / ARR_BLOCK_SIZE;
		block = dyn_array->block[block_num];
		op_ch = &block->op[i % ARR_BLOCK_SIZE];
		sprintf(buf, "Ins: %d\nPC: 0x%04x\nChanges: %d\n\n", i, op_ch->pc, op_ch->changes);
		fwrite(buf, sizeof(char), strlen(buf), file);

	}
}

void save_backlog_state_to_file(struct emu8051_data *emu_data, char *file_name)
{
	FILE *save_file;
	char *pwd, *buf;
	const char *dir_name = "/save-states/";

	pwd = getenv("PWD");
	if (pwd == NULL)
		return;

	buf = calloc(1, strlen(pwd) + strlen(file_name) + strlen(dir_name) + 5);
	if (buf == NULL)
		return;

	sprintf(buf, "%s%s%s.txt", pwd, dir_name, file_name);

	save_file = fopen(buf, "w+");
	if (save_file == NULL) {
		free(buf);
		return;
	}

	backlog_debug_write(save_file, &emu_data->dyn_array);

	free(buf);
	fclose(save_file);
}
*/

void op_change_write(struct emu8051_dev *emu_dev, uint16_t addr_type, uint8_t addr)
{
	struct op_change *op_ch;
	uint16_t tmp;
	uint8_t count, i;

	if (emu_dev->op_ch == NULL)
		return;

	op_ch = emu_dev->op_ch;
	count = op_ch->changes;

	op_ch->change_type[count] =  addr | addr_type;

	switch (addr_type) {
	case D_WRITE:
		op_ch->change_val[count] = direct_read(emu_dev, addr);
		break;
	case I_WRITE:
		op_ch->change_val[count] = emu_dev->iram[addr];
		break;
	case R_WRITE:
		op_ch->change_val[count] = get_reg(emu_dev, addr);
		break;
	case E_WRITE:
		op_ch->change_val[count] = emu_dev->xram[get_dptr(emu_dev)];
		break;
	case ER_WRITE:
		op_ch->change_val[count] = emu_dev->xram[addr];
		break;
	case S_WRITE:
		if ((addr == PSW || addr == SP) && count > 0) {
			for (i = 0; i < count; i++) {
				tmp = op_ch->change_type[i];
				tmp &= 0xff;
				if (tmp == addr)
					return;
			}
		}
		op_ch->change_val[count] = direct_read(emu_dev, addr + 0x80);
		break;
	}

	op_ch->changes++;
}

void reverse_op(struct emu8051_dev *emu_dev, struct op_change *op_ch)
{
	uint16_t addr_type;
	uint8_t count, addr, i;

	if (op_ch == NULL)
		return;

	emu_dev->pc = op_ch->pc;
	emu_dev->op_ch = NULL;

	count = op_ch->changes;

	for (i = 0; i < count; i++) {
		addr_type = op_ch->change_type[i] & 0xff00;
		addr = op_ch->change_type[i] & 0xff;

		switch (addr_type) {
		case D_WRITE:
			direct_write(emu_dev, addr, op_ch->change_val[i]);
			break;
		case I_WRITE:
			indirect_write(emu_dev, addr, op_ch->change_val[i]);
			break;
		case R_WRITE:
			set_reg(emu_dev, addr, op_ch->change_val[i]);
			break;
		case E_WRITE:
			emu_dev->xram[get_dptr(emu_dev)] = op_ch->change_val[i];
			break;
		case ER_WRITE:
			emu_dev->xram[addr] = op_ch->change_val[i];
			break;
		case S_WRITE:
			write_sfr(emu_dev, addr, op_ch->change_val[i]);
			break;
		}
	}
}

void free_blocks(struct dynamic_array *dyn_array)
{
	uint32_t i;

	for (i = 0; i < dyn_array->size; i++)
		free(dyn_array->block[i]);

	free(dyn_array->block);
}

void allocate_blocks(struct dynamic_array *dyn_array, uint32_t size)
{
        uint32_t i, tmp;

        tmp = size / ARR_BLOCK_SIZE;

        if (size % ARR_BLOCK_SIZE != 0)
                tmp++;

        tmp++;
        dyn_array->size  = tmp;
        dyn_array->block = calloc(tmp, sizeof(void *));

        for (i = 0; i < tmp; i++)
                dyn_array->block[i] = calloc(1, sizeof(struct arr_block));
}

void remove_rear_block(struct dynamic_array *dyn_array)
{
        struct arr_block *old_block;

        old_block = dyn_array->block[0];
        memmove(dyn_array->block, dyn_array->block + 1,
			(dyn_array->size - 1) * sizeof(void *));

        free(old_block);

        dyn_array->size--;
}

void remove_front_block(struct dynamic_array *dyn_array)
{
        struct arr_block *old_block;

        old_block = dyn_array->block[dyn_array->size - 1];
        free(old_block);

        dyn_array->size--;
}

void shrink_dynarray(struct dynamic_array *dyn_array, uint32_t new_size)
{
	uint32_t size_diff = dyn_array->size - new_size;
	uint32_t i, cur_block;

	cur_block = (dyn_array->count / ARR_BLOCK_SIZE) + 1;
        if (dyn_array->count % ARR_BLOCK_SIZE)
                cur_block++;

	for (i = 0; i < size_diff; i++) {
		if (cur_block < dyn_array->size)
			remove_front_block(dyn_array);
		else
			remove_rear_block(dyn_array);
	}

	dyn_array->block = realloc(dyn_array->block, new_size * sizeof(void *));
	if (dyn_array->count > new_size * ARR_BLOCK_SIZE)
		dyn_array->count = (new_size - 1) * ARR_BLOCK_SIZE;

	dyn_array->size = new_size;
}

void grow_dynarray(struct dynamic_array *dyn_array, uint32_t new_size)
{
	uint32_t size_diff = new_size - dyn_array->size;
	uint32_t i;

	dyn_array->block = realloc(dyn_array->block, new_size * sizeof(void *));

	for (i = 0; i < size_diff; i++)
                dyn_array->block[i + dyn_array->size] = calloc(1, sizeof(struct arr_block));

	dyn_array->size = new_size;
}

void resize_dynarray(struct dynamic_array *dyn_array, uint32_t size)
{
        uint32_t old_blocks, new_blocks;

	old_blocks = dyn_array->size;

	if (size < ARR_BLOCK_SIZE)
		new_blocks = 2;
	else
		new_blocks = (size / ARR_BLOCK_SIZE) + 1;

        if (size % ARR_BLOCK_SIZE != 0)
                new_blocks++;

	if (new_blocks > old_blocks)
		grow_dynarray(dyn_array, new_blocks);
	else if (new_blocks < old_blocks)
		shrink_dynarray(dyn_array, new_blocks);
	else
		return;
}


void attach_front_block(struct dynamic_array *dyn_array)
{
        dyn_array->block[dyn_array->size] = calloc(1, sizeof(struct arr_block));

        dyn_array->size++;
}

struct op_change *pop_front(struct dynamic_array *dyn_array)
{
        struct arr_block *block;
        uint32_t block_num, tmp;

	if (dyn_array->count == 0)
		return NULL;

	tmp = (--dyn_array->count) + dyn_array->start_offset;
        block_num = (tmp) / ARR_BLOCK_SIZE;
        block = dyn_array->block[block_num];

	return &block->op[tmp % ARR_BLOCK_SIZE];
}

struct op_change *get_front(struct dynamic_array *dyn_array)
{
        struct arr_block *block;
        uint32_t block_num, tmp;

	if (dyn_array->count == 0)
		return NULL;

        tmp = (dyn_array->count - 1) + dyn_array->start_offset;
        block_num = (tmp) / ARR_BLOCK_SIZE;
        block = dyn_array->block[block_num];

	return &block->op[tmp % ARR_BLOCK_SIZE];
}

void write_front(struct dynamic_array *dyn_array, struct op_change op)
{
        struct arr_block *block;
        uint32_t block_num, tmp;

        tmp = dyn_array->count + dyn_array->start_offset;
        block_num = (tmp) / ARR_BLOCK_SIZE;
        block = dyn_array->block[block_num];
	block->op[tmp % ARR_BLOCK_SIZE] = op;

        if (dyn_array->count >= (dyn_array->size - 1) * ARR_BLOCK_SIZE) {
                dyn_array->start_offset++;
                if (dyn_array->start_offset == ARR_BLOCK_SIZE) {
                        dyn_array->start_offset = 0;
                        remove_rear_block(dyn_array);
                        attach_front_block(dyn_array);
                }
        } else
                dyn_array->count++;
}

void breakpoint_flag_set(struct emu8051_data *emu_data, uint8_t flag_id, uint8_t set)
{
	struct breakpoint_data *break_data = &emu_data->break_data;

	if (set)
		break_data->break_flags |= flag_id;
	else
		break_data->break_flags &= ~flag_id;
}

uint8_t breakpoint_check(struct emu8051_data *emu_data)
{
	struct emu8051_dev *emu_dev = emu_data->emu_dev;
	struct breakpoint_data *break_data = &emu_data->break_data;

	if ((break_data->break_flags & FLAG_BRK_PC) && (emu_dev->pc == break_data->pc_break))
		return 1;

	if ((break_data->break_flags & FLAG_BRK_DPTR) && (get_dptr(emu_dev) == break_data->dptr_break))
		return 1;

	return 0;
}

/* Logging related functions:
 * Logs write certain functions out to a logfile, current logs that we're
 * interested in are writes to exram, iram, and the SFR's. Also, log any
 * function calls, and any time that the HIC bus addr/data is set.
 */

void open_log_file(struct emu8051_data *emu_data, char *file_name)
{
	const char *dir_name = "/log-files/";
	FILE *log_file;
	char *pwd, *buf;
	uint32_t i;

	pwd = getenv("PWD");
	if (pwd == NULL)
		return;

	/* Remove trailing spaces. */
	for (i = strlen(file_name) - 1; i != 0; i--) {
		if (file_name[i] == ' ') {
			file_name[i] = '\0';
			continue;
		} else
			break;
	}

	buf = calloc(1, strlen(pwd) + strlen(file_name) + strlen(dir_name) + 1);
	if (buf == NULL)
		return;

	sprintf(buf, "%s%s%s", pwd, dir_name, file_name);

	log_file = fopen(buf, "w+");
	if (log_file == NULL) {
		free(buf);
		return;
	}

	emu_data->log_data.log_file = log_file;
	emu_data->log_data.logging_set = 1;
	emu_data->log_data.number_of_funcs = 0;

	free(buf);
}

void acall_log_write(struct emu8051_data *emu_data)
{
	struct emu8051_dev *emu_dev = emu_data->emu_dev;
	uint16_t ret_addr, jmp_addr, tmp;
	char buf[1024];

	/* Push return address onto the stack, low first, high second. */
	ret_addr = emu_dev->pc + 2;

	/* High-order bits 15-11 come from incremented PC's 15-11 bits */
	jmp_addr = ret_addr & 0xf8;

	/* Bits 10-8 come from 3 high order bits in the opcode. */
	tmp = (get_pmem(emu_dev, emu_dev->pc) >> 5) & 0x07;
	jmp_addr |= (tmp << 8);

	/* Bits 7-0 come from second byte of instruction. */
	jmp_addr |= get_pmem(emu_dev, emu_dev->pc + 1);

	sprintf(buf, "ACALL: op %d, pc 0x%04x, func addr 0x%04x, ret addr 0x%04x.\n", emu_data->number,
			emu_dev->pc, jmp_addr, ret_addr);

	fputs(buf, emu_data->log_data.log_file);
}

void lcall_log_write(struct emu8051_data *emu_data)
{
	struct logfile_data *log_data = &emu_data->log_data;
	struct emu8051_dev *emu_dev = emu_data->emu_dev;
	char buf[1024];
	uint8_t pmem_bank;
	uint16_t addr;
	uint32_t i;

	pmem_bank = direct_read(emu_dev, 0xfa);
	addr = get_pmem(emu_dev, emu_dev->pc + 2);
	addr |= (get_pmem(emu_dev, emu_dev->pc + 1) << 8);

	sprintf(buf, "\nLCALL: op %d, pc 0x%04x, func addr 0x%04x, bank 0x%02x, ret addr 0x%04x.\n", emu_data->number,
			emu_dev->pc, addr, pmem_bank, emu_dev->pc + 3);

	fputs(buf, emu_data->log_data.log_file);

	if (log_data->number_of_funcs) {
		for (i = 0; i < log_data->number_of_funcs; i++) {
			if (log_data->func_addrs[i] == addr
					&& log_data->pmem_bank[i] == pmem_bank) {
				log_data->func_call_cnt[i]++;
				return;
			}
		}

		/* Never found this function, so add it as a new one. */
		log_data->func_addrs = realloc(log_data->func_addrs,
				sizeof(uint16_t) * log_data->number_of_funcs + 1);
		log_data->func_call_cnt = realloc(log_data->func_call_cnt,
				sizeof(uint16_t) * log_data->number_of_funcs + 1);
		log_data->pmem_bank = realloc(log_data->pmem_bank,
				sizeof(uint8_t) * log_data->number_of_funcs + 1);
		log_data->func_addrs[log_data->number_of_funcs] = addr;
		log_data->pmem_bank[log_data->number_of_funcs] = pmem_bank;
		log_data->func_call_cnt[log_data->number_of_funcs] = 1;
		log_data->number_of_funcs++;
	} else {
		log_data->func_addrs = calloc(1, sizeof(uint16_t));
		log_data->func_call_cnt = calloc(1, sizeof(uint16_t));
		log_data->pmem_bank = calloc(1, sizeof(uint8_t));
		log_data->func_addrs[0] = addr;
		log_data->func_call_cnt[0]++;
		log_data->pmem_bank[0] = pmem_bank;
		log_data->number_of_funcs++;
	}
}


uint8_t mov_get_src(struct emu8051_dev *emu_dev, const struct opcode_info *op,
		char *buf)
{
	uint8_t addr, data;

	addr = 0;
	switch (op->src_id) {
	case IMMED_ID:
		if (op->dst_id == DIRECT_ID)
			data = get_pmem(emu_dev, emu_dev->pc + 2);
		else
			data = get_pmem(emu_dev, emu_dev->pc + 1);
		sprintf(buf, "immediate val 0x%02x", data);
		break;
	case DIRECT_ID:
		addr = get_pmem(emu_dev, emu_dev->pc + 1);
		data = direct_read(emu_dev, addr);
		sprintf(buf, "direct read addr 0x%02x, data 0x%02x", addr, data);
		break;
	case I_R0_ID:
		addr = get_reg(emu_dev, 0) & 0xff;
		data = emu_dev->iram[addr];
		sprintf(buf, "indirect read (r0) addr 0x%02x, data 0x%02x", addr, data);
		break;
	case I_R1_ID:
		addr = get_reg(emu_dev, 1) & 0xff;
		data = emu_dev->iram[addr];
		sprintf(buf, "indirect read (r1) addr 0x%02x, data 0x%02x", addr, data);
		break;
	case R0_ID ... R7_ID:
		addr = op->src_id - R0_ID;
		data = get_reg(emu_dev, addr) & 0xff;
		sprintf(buf, "read r%d, data 0x%02x", addr, data);
		break;
	case A_ID:
		data = emu_dev->sfr[ACC];
		sprintf(buf, "read acc, data 0x%02x", data);
		break;
	default:
		sprintf(buf, "no src read");
		data = 0;
		break;
	}

	return data;
}

uint32_t get_current_hic_addr(struct emu8051_dev *emu_dev)
{
	uint32_t addr;

	addr = direct_read(emu_dev, 0xb1) & 0xff;
	addr |= (direct_read(emu_dev, 0xb2) & 0xff) << 8;
	addr |= (direct_read(emu_dev, 0xb3) & 0xff) << 16;

	return addr;
}

uint32_t get_current_hic_data(struct emu8051_dev *emu_dev)
{
	uint32_t data;

	data = direct_read(emu_dev, 0xb4) & 0xff;
	data |= (direct_read(emu_dev, 0xb5) & 0xff) << 8;
	data |= (direct_read(emu_dev, 0xb6) & 0xff) << 16;
	data |= (direct_read(emu_dev, 0xb7) & 0xff) << 24;

	return data;
}

void check_hic_register_set(struct emu8051_data *emu_data,
		uint8_t addr, uint8_t data)
{
	struct emu8051_dev *emu_dev = emu_data->emu_dev;
	struct logfile_data *log_data = &emu_data->log_data;
	uint8_t i, addr_set, data_set;
	uint32_t hic_addr, hic_data;
	char buf[128];

	memset(buf, 0, 128);

	data_set = addr_set = 1;
	if (addr < 0xb4)
		log_data->hic_addr_set[addr - 0xb1] = 1;
	else
		log_data->hic_data_set[addr - 0xb4] = 1;

	for (i = 0; i < 3; i++) {
		if (!log_data->hic_addr_set[i]) {
			addr_set = 0;
			break;
		}
	}

	for (i = 0; i < 4; i++) {
		if (!log_data->hic_data_set[i]) {
			data_set = 0;
			break;
		}
	}

	if (addr_set) {
		hic_addr = get_current_hic_addr(emu_dev) & 0xffff;
		hic_addr |= (data << 16);
		sprintf(buf, "HIC Bus address set to 0x%06x.\n", hic_addr);
		fputs(buf, emu_data->log_data.log_file);
		memset(log_data->hic_addr_set, 0, 3);
	}

	if (data_set) {
		hic_data = get_current_hic_data(emu_dev) & 0xffffff;
		hic_data |= (data << 24);
		sprintf(buf, "HIC Bus data set to 0x%08x.\n", hic_data);
		fputs(buf, emu_data->log_data.log_file);
		memset(log_data->hic_data_set, 0, 4);
	}
}

void mov_log_write(struct emu8051_data *emu_data,
		const struct opcode_info *op)
{
	struct emu8051_dev *emu_dev = emu_data->emu_dev;
	uint8_t log_write = 1;
	char buf[1024], tmp[128];
	uint8_t data, addr, src;

	memset(buf, 0, 1024);
	memset(tmp, 0, 128);

	switch (op->dst_id) {
	case DIRECT_ID:
		src = mov_get_src(emu_dev, op, tmp);
		if (op->src_id == DIRECT_ID)
			addr = get_pmem(emu_dev, emu_dev->pc + 2);
		else
			addr = get_pmem(emu_dev, emu_dev->pc + 1);

		sprintf(buf, "MOV: op %d, pc 0x%04x, write direct addr 0x%02x, %s.\n", emu_data->number,
			emu_dev->pc, addr, tmp);
		break;
	case I_R0_ID:
		addr = get_reg(emu_dev, 0) & 0xff;
		src = mov_get_src(emu_dev, op, tmp);
		sprintf(buf, "MOV: op %d, pc 0x%04x, write iram (r0) addr 0x%02x, %s.\n", emu_data->number,
			emu_dev->pc, addr, tmp);
		break;
	case I_R1_ID:
		addr = get_reg(emu_dev, 1) & 0xff;
		src = mov_get_src(emu_dev, op, tmp);
		sprintf(buf, "MOV: op %d, pc 0x%04x, write iram (r1) addr 0x%02x, %s.\n", emu_data->number,
			emu_dev->pc, addr, tmp);
		break;
	case A_ID:
		src = addr = 0;
		if (op->src_id == DIRECT_ID)
			addr = get_pmem(emu_dev, emu_dev->pc + 1);
		else
			break;
		data = direct_read(emu_dev, addr);
		sprintf(tmp, "direct read addr 0x%02x, data 0x%02x", addr, data);
		sprintf(buf, "MOV: op %d, pc 0x%04x, write ACC, %s.\n", emu_data->number,
			emu_dev->pc, tmp);
		break;
	default:
		return;
	}

	if (log_write)
		fputs(buf, emu_data->log_data.log_file);

	/* Check if all the registers on the HIC Bus have been set in
	 * succession, and print out their value if they have. */
	if (op->dst_id == DIRECT_ID && (addr >= 0xb1 && addr <= 0xb7))
		check_hic_register_set(emu_data, addr, src);

}

void movx_log_write(struct emu8051_data *emu_data,
		const struct opcode_info *op)
{
	struct emu8051_dev *emu_dev = emu_data->emu_dev;
	char buf[1024];
	uint16_t dptr;
	uint8_t data;

	/* Read from exram. */
	if (op->dst_id == A_ID) {
		switch (op->src_id) {
		case IDPTR_ID:
			dptr = get_dptr(emu_dev);
			break;
		case I_R0_ID:
			dptr = get_reg(emu_dev, 0) & 0xff;
			break;
		case I_R1_ID:
			dptr = get_reg(emu_dev, 1) & 0xff;
			break;
		default:
			return;
		}

		data = emu_dev->xram[dptr];

		sprintf(buf, "MOVX: op %d, pc 0x%04x, read xram addr 0x%04x, value 0x%02x.\n", emu_data->number,
			emu_dev->pc, dptr, data);
	} else { /* Write to exram. */
		switch (op->dst_id) {
		case IDPTR_ID:
			dptr = get_dptr(emu_dev);
			break;
		case I_R0_ID:
			dptr = get_reg(emu_dev, 0) & 0xff;
			break;
		case I_R1_ID:
			dptr = get_reg(emu_dev, 1) & 0xff;
			break;
		default:
			return;
		}

		data = emu_dev->sfr[ACC];
		sprintf(buf, "MOVX: op %d, pc 0x%04x, write xram addr 0x%04x, value 0x%02x.\n", emu_data->number,
			emu_dev->pc, dptr, data);
	}

	fputs(buf, emu_data->log_data.log_file);
}

void movc_log_write(struct emu8051_data *emu_data,
		const struct opcode_info *op)
{
	struct emu8051_dev *emu_dev = emu_data->emu_dev;
	uint8_t acc, data;
	char buf[1024];
	uint16_t dptr;

	acc = emu_dev->sfr[ACC];
	if (op->src_id == A_DPTR_ID) {
		dptr = get_dptr(emu_dev);
		data = get_pmem(emu_dev, dptr + acc);
		sprintf(buf, "MOVC: op %d, pc 0x%04x, read pmem (dptr) 0x%04x, value 0x%02x.\n", emu_data->number,
			emu_dev->pc, dptr + acc, data);
	} else {
		dptr = emu_dev->pc + 1;
		data = get_pmem(emu_dev, dptr + acc);
		sprintf(buf, "MOVC: op %d, pc 0x%04x, read pmem (pc) 0x%04x, value 0x%02x.\n", emu_data->number,
			emu_dev->pc, dptr + acc, data);
	}

	fputs(buf, emu_data->log_data.log_file);
}

void ret_log_write(struct emu8051_data *emu_data)
{
	struct emu8051_dev *emu_dev = emu_data->emu_dev;
	char buf[1024];
	uint16_t addr;

	addr = emu_dev->iram[emu_dev->sfr[SP]] << 8;
	addr |= emu_dev->iram[emu_dev->sfr[SP] - 1] & 0xff;

	sprintf(buf, "RET: op %d, pc 0x%04x, ret addr 0x%04x.\n\n", emu_data->number,
			emu_dev->pc, addr);
	fputs(buf, emu_data->log_data.log_file);
}

void logging_print_func_calls(struct emu8051_data *emu_data)
{
	struct logfile_data *log_data = &emu_data->log_data;
	char buf[1024];
	uint32_t i;

	memset(buf, 0, 1024);
	for (i = 0; i < log_data->number_of_funcs; i++) {
		sprintf(buf, "Func[%d]: addr 0x%04x, bank 0x%02x, call count %d.\n",
				i, log_data->func_addrs[i], log_data->pmem_bank[i],
				log_data->func_call_cnt[i]);
		fputs(buf, log_data->log_file);
	}
}

void logging_close_log(struct emu8051_data *emu_data)
{
	struct logfile_data *log_data = &emu_data->log_data;

	if (!log_data->logging_set)
		return;

	logging_print_func_calls(emu_data);

	fclose(log_data->log_file);
	if (log_data->number_of_funcs) {
		if (log_data->func_addrs)
			free(log_data->func_addrs);
		if (log_data->func_call_cnt)
			free(log_data->func_call_cnt);
		if (log_data->pmem_bank)
			free(log_data->pmem_bank);
	}

	memset(log_data, 0, sizeof(*log_data));
}

void logging_check_opcode(struct emu8051_data *emu_data,
		const struct opcode_info *op)
{
	struct emu8051_dev *emu_dev = emu_data->emu_dev;

	switch (op->op_id) {
	case ACALL_ID:
		acall_log_write(emu_data);
		break;
	case LCALL_ID:
		lcall_log_write(emu_data);
		break;
	case MOV_ID:
		mov_log_write(emu_data, op);
		break;
	case MOVC_ID:
		movc_log_write(emu_data, op);
		break;
	case MOVX_ID:
		movx_log_write(emu_data, op);
		break;
	case RET_ID:
		ret_log_write(emu_data);
		break;
	default:
		break;
	}

	if (!emu_dev->iram[0x79] && emu_data->log_data.exit_on_verb) {
		fputs("End of verb handling.\n", emu_data->log_data.log_file);
		logging_close_log(emu_data);
	}
}

