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

static uint8_t read_header(FILE *file_in)
{
	uint8_t i;
	char hdr[5];

	fread(hdr, 4, 1, file_in);
	hdr[4] = '\0';

	for (i = 0; i < NUM_OF_SECS; i++) {
		if (!strcmp(file_sections[i], hdr))
			return i;
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
	FILE *save_file;
	char *pwd, *buf;
	const char *dir_name = "/save-states/";

	pwd = getenv("PWD");
	if (pwd == NULL)
		return;

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

void restore_state_from_file(struct emu8051_data *emu_data, FILE *file_in)
{
	struct emu8051_dev *emu_dev = emu_data->emu_dev;
	uint32_t i;
	uint32_t new_size;
	uint8_t ret;

	//First thing to do, read 8 bytes, compare it to main header.
	ret = read_header(file_in);
	if (ret != FILE_HEADER)
		return;
	fread(&emu_dev->pc, sizeof(uint16_t), 1, file_in);

	while (!feof(file_in)) {
		ret = read_header(file_in);
		switch (ret) {
		case PMEM_SEC:
			fread(emu_dev->pmem, sizeof(emu_dev->pmem), 1, file_in);
			break;
		case PMEM_B0_SEC:
			fread(emu_dev->pmem_b0, sizeof(emu_dev->pmem_b0), 1, file_in);
			break;
		case PMEM_B1_SEC:
			fread(emu_dev->pmem_b1, sizeof(emu_dev->pmem_b1), 1, file_in);
			break;
		case XRAM_SEC:
			fread(emu_dev->xram, sizeof(emu_dev->xram), 1, file_in);
			break;
		case IRAM_SEC:
			fread(emu_dev->iram, sizeof(emu_dev->iram), 1, file_in);
			break;
		case SFR_SEC:
			fread(emu_dev->sfr, sizeof(emu_dev->sfr), 1, file_in);
			break;
		case BKOP_SEC:
			fread(&emu_data->dyn_array.count, sizeof(uint32_t), 1, file_in);
			fread(&new_size, sizeof(uint32_t), 1, file_in);
			resize_dynarray(&emu_data->dyn_array, (new_size - 1) * ARR_BLOCK_SIZE);
			fread(&emu_data->dyn_array.start_offset, sizeof(uint32_t), 1, file_in);

			for (i = 0; i < emu_data->dyn_array.size; i++)
				fread(emu_data->dyn_array.block[i], sizeof(struct arr_block), 1, file_in);
			break;
		default:
			break;
		}
	}
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
