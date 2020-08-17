#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <ncurses.h>
#include <panel.h>
#include "emu8051_defs.h"

void parse_file(FILE *file_in, struct emu8051_dev *emu_dev, uint8_t file_id)
{
	switch (file_id) {
	case FILE_ROM0:
		fread(emu_dev->pmem, sizeof(emu_dev->pmem), 1, file_in);
		break;
	case FILE_ROM1:
		fread(emu_dev->pmem_b0, sizeof(emu_dev->pmem_b0), 1, file_in);
		break;
	case FILE_ROM2:
		fread(emu_dev->pmem_b1, sizeof(emu_dev->pmem_b1), 1, file_in);
		break;
	case FILE_XRAM:
		fread(emu_dev->xram, sizeof(emu_dev->xram), 1, file_in);
		break;
	case FILE_IRAM:
		fread(emu_dev->iram, sizeof(emu_dev->iram), 1, file_in);
		break;
	default:
		return;
	}
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
	FILE *file_in;
	int c;

	while ((c = getopt (argc, argv, "0:1:2:i:e:s:")) != -1) {
		switch (c) {
		case '0':
			file_name = optarg;
			file_in = fopen(file_name, "r");
			parse_file(file_in, emu_dev, FILE_ROM0);
			break;
		case '1':
			file_name = optarg;
			file_in = fopen(file_name, "r");
			parse_file(file_in, emu_dev, FILE_ROM1);
			break;
		case '2':
			file_name = optarg;
			file_in = fopen(file_name, "r");
			parse_file(file_in, emu_dev, FILE_ROM2);
			break;
		case 'e':
			file_name = optarg;
			file_in = fopen(file_name, "r");
			parse_file(file_in, emu_dev, FILE_XRAM);
			break;
		case 'i':
			file_name = optarg;
			file_in = fopen(file_name, "r");
			parse_file(file_in, emu_dev, FILE_IRAM);
			break;
		case 's':
			file_name = optarg;
			file_in = fopen(file_name, "r");
			restore_state_from_file(emu_data, file_in);
			break;
		}

		if (file_in != NULL)
			fclose(file_in);
	}

	return 0;
}

int main(int argc, char **argv)
{
	struct emu8051_data *emu_data;

//	if (argc < 4) {
//		printf("Usage: %s <rom0> <rom1> <rom2> <exram>\n", argv[0]);
//		return -1;
//	}

	emu_data = calloc(1, sizeof(struct emu8051_data));
	emu_data->emu_dev = calloc(1, sizeof(struct emu8051_dev));

	allocate_blocks(&emu_data->dyn_array, BACK_OPS);

	emu_data->emu_dev->op_ch = NULL;
	emu_data->dyn_array.count = 0;
	emu_data->dyn_array.start_offset = 0;
	emu_data->max_backops = BACK_OPS;
	emu_data->step_size = 100;

	/* Parse command line arguments */
	parse_commands(argc, argv, emu_data);

	direct_write(emu_data->emu_dev, 0xf9, 0x40);

	disp_init(emu_data);

	emu_main(emu_data);

	endwin();

	return 0;
}
