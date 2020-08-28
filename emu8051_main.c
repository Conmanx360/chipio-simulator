#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <ncurses.h>
#include <panel.h>
#include "emu8051_defs.h"

static int parse_file(FILE *file_in, struct emu8051_dev *emu_dev, uint8_t file_id)
{
	switch (file_id) {
	case FILE_ROM0:
		if (!fread(emu_dev->pmem, sizeof(emu_dev->pmem), 1, file_in))
			return -1;
		break;
	case FILE_ROM1:
		if (!fread(emu_dev->pmem_b0, sizeof(emu_dev->pmem_b0), 1, file_in))
			return -1;
		break;
	case FILE_ROM2:
		if (!fread(emu_dev->pmem_b1, sizeof(emu_dev->pmem_b1), 1, file_in))
			return -1;
		break;
	case FILE_XRAM:
		if (!fread(emu_dev->xram, sizeof(emu_dev->xram), 1, file_in))
			return -1;
		break;
	case FILE_IRAM:
		if (!fread(emu_dev->iram, sizeof(emu_dev->iram), 1, file_in))
			return -1;
		break;
	default:
		break;
	}

	return 0;
}

void emu_main(struct emu8051_data *emu_data)
{
	int ch;

	while((ch = getch()) != KEY_F(1)) {
		emu8051_in_handler(emu_data, ch);

		update_win(emu_data);
		refresh();
	}
}

int parse_commands(int argc, char **argv, struct emu8051_data *emu_data)
{
	struct emu8051_dev *emu_dev = emu_data->emu_dev;
	char *file_name = NULL;
	FILE *file_in = NULL;
	int c, ret;

	ret = 0;
	while ((c = getopt (argc, argv, "0:1:2:i:e:s:")) != -1) {
		switch (c) {
		case '0':
			file_name = optarg;
			file_in = fopen(file_name, "r");
			ret = parse_file(file_in, emu_dev, FILE_ROM0);
			if (ret < 0)
				goto exit;
			break;
		case '1':
			file_name = optarg;
			file_in = fopen(file_name, "r");
			ret = parse_file(file_in, emu_dev, FILE_ROM1);
			if (ret < 0)
				goto exit;
			break;
		case '2':
			file_name = optarg;
			file_in = fopen(file_name, "r");
			ret = parse_file(file_in, emu_dev, FILE_ROM2);
			if (ret < 0)
				goto exit;
			break;
		case 'e':
			file_name = optarg;
			file_in = fopen(file_name, "r");
			ret = parse_file(file_in, emu_dev, FILE_XRAM);
			if (ret < 0)
				goto exit;
			break;
		case 'i':
			file_name = optarg;
			file_in = fopen(file_name, "r");
			ret = parse_file(file_in, emu_dev, FILE_IRAM);
			if (ret < 0)
				goto exit;
			break;
		case 's':
			file_name = optarg;
			file_in = fopen(file_name, "r");
			ret = restore_state_from_file(emu_data, file_in);
			if (ret < 0)
				goto exit;
			break;
		}

		if (file_in != NULL) {
			fclose(file_in);
			file_in = NULL;
		}
	}
exit:
	if (file_in != NULL)
		fclose(file_in);

	return ret;
}

int main(int argc, char **argv)
{
	struct emu8051_data *emu_data;

	emu_data = calloc(1, sizeof(struct emu8051_data));
	emu_data->emu_dev = calloc(1, sizeof(struct emu8051_dev));

	allocate_blocks(&emu_data->dyn_array, BACK_OPS);

	emu_data->emu_dev->op_ch = NULL;
	emu_data->dyn_array.count = 0;
	emu_data->dyn_array.start_offset = 0;
	emu_data->max_backops = BACK_OPS;
	emu_data->step_size = 100;

	/* Parse command line arguments */
	if (parse_commands(argc, argv, emu_data) < 0) {
		printf("Parse failed!\n");
		return 0;
	}

	direct_write(emu_data->emu_dev, 0xf9, 0x40);

	disp_init(emu_data);

	emu_main(emu_data);

	endwin();

	if (emu_data->log_data.logging_set)
		logging_close_log(emu_data);

	if (emu_data->dyn_array.count)
		free_blocks(&emu_data->dyn_array);

	free(emu_data->emu_dev);
	free(emu_data);

	return 0;
}
