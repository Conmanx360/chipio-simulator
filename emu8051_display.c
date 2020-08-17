#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <ncurses.h>
#include <string.h>
#include <panel.h>
#include <form.h>
#include <menu.h>
#include "emu8051_defs.h"
#include "emu8051_display.h"

void get_sub_win_dimensions(struct emu_win_data *win_data,
			    const struct popup_win_info *popup_info,
			    struct win_dimensions *win_dim)
{

	if (!popup_info->height)
		win_dim->height = (win_data->height / 3) * 2;
	else
		win_dim->height = popup_info->height;

	if (!popup_info->width)
		win_dim->width = (win_data->width / 3) * 2;
	else
		win_dim->width = popup_info->width;

	win_dim->y = (win_data->height - win_dim->height) / 2;
	win_dim->x = (win_data->width - win_dim->width) / 2;
}

void draw_border_colored(WINDOW *win, enum color_pair color)
{
	wattron(win, COLOR_PAIR(color));
	box(win, 0 , 0);
	wattroff(win, COLOR_PAIR(color));
}

void print_popup_title(WINDOW *win, int width, const char *title, enum color_pair color)
{
	uint16_t length, offset;

	wattron(win, COLOR_PAIR(PAIR_CYAN));
	mvwhline(win, 2, 1, ACS_HLINE, width - 2);
	mvwaddch(win, 2, 0, ACS_LTEE);
	mvwaddch(win, 2, width - 1, ACS_RTEE);
	wattroff(win, COLOR_PAIR(PAIR_CYAN));

	length = strlen(title);
	offset = (width - length) / 2;

	wattron(win, COLOR_PAIR(color));
	mvwprintw(win, 1, offset, "%s", title);
	wattroff(win, COLOR_PAIR(color));

	wrefresh(win);
}

void init_popups(struct emu8051_data *emu_data)
{
	struct emu_win_data *win_data = &emu_data->win_data[DISASSEMBLY_WIN];
	const struct popup_win_info *popup_info;
	const struct popup_field_info *field_info;
	struct popup_win_data *popup_win;
	struct field_data *field_ptr;
	struct win_dimensions win_dim;
	int rows, cols;
	uint8_t i, y;

	for (i = 0; i < NUM_OF_POPUPS; i++) {
		popup_win = &emu_data->popup_data[i];
		popup_info = &popup_defs[i];

		get_sub_win_dimensions(win_data, popup_info, &win_dim);
		/* I think we need to have this window centered on the y axis
		 * depending on it's height, which is now variable. */
		popup_win->win = newwin(win_dim.height, win_dim.width,
					win_dim.y, win_dim.x);
		popup_win->height = win_dim.height;
		popup_win->width  = win_dim.width;
		popup_win->panel = new_panel(popup_win->win);
		draw_border_colored(popup_win->win, PAIR_CYAN);
		print_popup_title(popup_win->win, win_dim.width,
				  popup_info->title, PAIR_RED);
		keypad(popup_win->win, TRUE);

		if (popup_info->num_of_fields > 0) {
			popup_win->field = calloc(popup_info->num_of_fields,
						  sizeof(FIELD));
			popup_win->num_of_fields = popup_info->num_of_fields;
			popup_win->num_of_inputs = popup_info->num_of_inputs;
			popup_win->default_in = popup_info->default_in;
		}

		for (y = 0; y < popup_info->num_of_fields; y++) {
			field_info = &popup_info->field_info[y];
			popup_win->field[y] = new_field(field_info->height,
						        field_info->width,
							field_info->toprow,
							field_info->leftcol,
							field_info->offscreen,
							field_info->nbuffers);
			if (field_info->off_flags != 0xff)
				field_opts_off(popup_win->field[y],
					       field_info->off_flags);

			if (field_info->field_label != NULL)
				set_field_buffer(popup_win->field[y], 0,
						 field_info->field_label);

			set_field_fore(popup_win->field[y], COLOR_PAIR(field_info->fore_color));
			set_field_back(popup_win->field[y], field_info->field_back);

			field_ptr = calloc(1, sizeof(struct field_data));
			field_ptr->field_id = field_info->field_id;
			field_ptr->in_type = field_info->in_type;
			if (field_info->in_type == INPUT_TOGGLE)
				field_ptr->toggle_id = field_info->button_id;

			set_field_userptr(popup_win->field[y], field_ptr);
		}

		if (popup_info->num_of_fields > 0) {
			popup_win->form = new_form(popup_win->field);
			scale_form(popup_win->form, &rows, &cols);
			set_form_win(popup_win->form, popup_win->win);
			set_form_sub(popup_win->form,
				     derwin(popup_win->win, rows, cols, 4,
				     (win_dim.width / 2) - (cols / 2)));
		} else {
			popup_win->subwin = derwin(popup_win->win,
						   win_dim.height - 4,
						   win_dim.width - 2, 3, 1);
			popup_win->cur_offset = 0;
			popup_win->cur_pos = 0;
		}

		hide_panel(popup_win->panel);
		update_panels();
		doupdate();
	}
}

/*
 * We want to create three different windows:
 * Main Window (contains disassembly)
 * Iram Window (top right)
 * SFR  Window (bottom right)
 */
void disp_init(struct emu8051_data *emu_data)
{
	uint32_t i;

	initscr();
	cbreak();
	noecho();
	start_color();
	keypad(stdscr, TRUE);

	init_pair(1, COLOR_RED, COLOR_BLACK);
	init_pair(2, COLOR_GREEN, COLOR_BLACK);
	init_pair(3, COLOR_CYAN, COLOR_BLACK);
	init_pair(4, COLOR_WHITE, COLOR_BLACK);
	init_pair(5, COLOR_BLUE, COLOR_BLACK);

	refresh();

	for (i = 0; i < NUM_OF_WINDOWS; i++) {
		switch (i) {
		case DISASSEMBLY_WIN:
			emu_data->win_data[i].height = LINES;
			emu_data->win_data[i].width = COLS - win_info[IRAM_WIN].width;
			emu_data->win_data[i].x = win_info[i].startx;
			emu_data->win_data[i].y = win_info[i].starty;
			break;
		case IRAM_WIN:
			emu_data->win_data[i].height = win_info[i].height;
			emu_data->win_data[i].width = win_info[i].width;
			emu_data->win_data[i].x = COLS - win_info[i].width;
			emu_data->win_data[i].y = win_info[i].starty;
			break;
		case SFR_WIN:
			emu_data->win_data[i].height = win_info[i].height;
			emu_data->win_data[i].width = win_info[i].width;
			emu_data->win_data[i].x = COLS - win_info[i].width;
			emu_data->win_data[i].y = win_info[i].starty;
			break;
		}
		emu_data->win_data[i].cur_pos = 0;
		emu_data->win_data[i].win = newwin(emu_data->win_data[i].height,
							 emu_data->win_data[i].width,
							 emu_data->win_data[i].y,
							 emu_data->win_data[i].x);
		wrefresh(emu_data->win_data[i].win);
	}
	init_popups(emu_data);
	update_win(emu_data);

	refresh();
}

void write_hex(WINDOW *win, uint16_t y_pos, uint16_t x_pos,
		      uint8_t data_len, uint32_t data)
{
	mvwprintw(win, y_pos, x_pos, "0x%0*x", data_len, data);

	wrefresh(win);
}

static char *get_direct_addr_str(uint8_t addr)
{
	char buf[20];
	uint8_t i;

	if (addr < 0x80)
		sprintf(buf, "0x%02x", addr);
	else {
		for (i = 0; i < NUM_OF_SFR_STRS; i++) {
			if (sfr_disp[i].addr == (addr - 0x80))
				return strdup(sfr_strs[i]);
		}
		sprintf(buf, "0x%02x", addr);
	}

	return strdup(buf);
}

static char *get_dst_str(struct emu8051_dev *dev,
			 const struct opcode_info *opcode,
			 uint16_t pc_tmp)
{
	uint16_t tmp1, tmp2;
	int8_t rel_offset;
	char *tmp_ptr;
	char buf[40];

	switch (opcode->dst_id) {
	case ADDR11_ID:
		tmp1 = (pc_tmp + opcode->length) & 0xf8;
		tmp2 = (get_pmem(dev, pc_tmp) >> 5) & 0x07;
		tmp1 |= tmp2 << 8;
		tmp1 |= get_pmem(dev, pc_tmp + 1);
		sprintf(buf, "0x%04x", tmp1);
		break;
	case ADDR16_ID:
		tmp1 = (get_pmem(dev, pc_tmp + 1)) << 8;
		tmp1 |= get_pmem(dev, pc_tmp + 2);
		sprintf(buf, "0x%04x", tmp1);
		break;
	case DIRECT_ID:
		tmp_ptr = get_direct_addr_str(get_pmem(dev, pc_tmp + 1));
		strcpy(buf, tmp_ptr);
		free(tmp_ptr);
		break;
	case BIT_ID:
		tmp1 = get_pmem(dev, pc_tmp + 1);
		if (tmp1 < 0x80) {
			tmp2 = 0x20 + (tmp1 / 8);
			sprintf(buf, "0x%02x.%d", tmp2, tmp1 % 8);
		} else {
			tmp2 = (tmp1 / 8) * 8;
			tmp_ptr = get_direct_addr_str(tmp2);
			sprintf(buf, "%s.%d", tmp_ptr, tmp1 % 8);
			free(tmp_ptr);
		}
		break;
	case IBIT_ID:
		tmp1 = get_pmem(dev, pc_tmp + 1);
		if (tmp1 < 0x80)
			tmp2 = 0x20 + (tmp1 / 8);
		else
			tmp2 = (tmp1 / 8) * 8;

		sprintf(buf, "/0x%02x.%d", tmp2, tmp1 % 8);
		break;
	case OFFSET_ID:
		rel_offset = get_pmem(dev, pc_tmp + 1);

		sprintf(buf, "0x%04x", (pc_tmp + opcode->length) + rel_offset);
		break;
	case IMMED_ID:
		sprintf(buf, " ");
		break;
	default:
		strcpy(buf, asm_args[opcode->dst_id]);
		break;
	}

	return strdup(buf);
}

static char *get_src_str(struct emu8051_dev *dev,
			 const struct opcode_info *opcode,
			 uint16_t pc_tmp)
{
	uint16_t tmp1, tmp2;
	uint8_t tmp8;
	int8_t rel_offset;
	char *tmp_ptr;
	char buf[40];

	switch (opcode->src_id) {
	case ADDR11_ID:
	case ADDR16_ID:
		sprintf(buf, " ");
		break;
	case DIRECT_ID:
		tmp_ptr = get_direct_addr_str(get_pmem(dev, pc_tmp + (opcode->length - 1)));
		strcpy(buf, tmp_ptr);
		free(tmp_ptr);
		break;
	case BIT_ID:
		/* Should probably get direct_addr_str here... */
		tmp1 = get_pmem(dev, pc_tmp + (opcode->length - 1));
		if (tmp1 < 0x80) {
			tmp2 = 0x20 + (tmp1 / 8);
			sprintf(buf, "0x%02x.%d", tmp2, tmp1 % 8);
		} else {
			tmp2 = (tmp1 / 8) * 8;
			tmp_ptr = get_direct_addr_str(tmp2);
			sprintf(buf, "%s.%d", tmp_ptr, tmp1 % 8);
			free(tmp_ptr);
		}
		break;
	case IBIT_ID:
		tmp1 = get_pmem(dev, pc_tmp + (opcode->length - 1));
		if (tmp1 < 0x80)
			tmp2 = 0x20 + (tmp1 / 8);
		else
			tmp2 = (tmp1 / 8) * 8;

		sprintf(buf, "/0x%02x.%d", tmp2, tmp1 % 8);
		break;
	case OFFSET_ID:
		rel_offset = get_pmem(dev, pc_tmp + (opcode->length - 1));

		sprintf(buf, "0x%04x", (pc_tmp + opcode->length) + rel_offset);
		break;
	case IMMED_ID:
		switch (opcode->op_id) {
		case MOV_ID:
			if (opcode->dst_id == DPTR_ID) {
				tmp1 = get_pmem(dev, pc_tmp + (opcode->length - 2));
				tmp2 = get_pmem(dev, pc_tmp + (opcode->length - 1));
				tmp2 |= tmp1 << 8;
				sprintf(buf, "#0x%04x", tmp2);
			} else {
				tmp8 = get_pmem(dev, pc_tmp + (opcode->length - 1));
				sprintf(buf, "#0x%02x", tmp8);
			}
			break;
		case CJNE_ID:
			tmp8 = get_pmem(dev, pc_tmp + (opcode->length - 2));
			sprintf(buf, "#0x%02x", tmp8);
			break;
		default:
			tmp8 = get_pmem(dev, pc_tmp + (opcode->length - 1));
			sprintf(buf, "#0x%02x", tmp8);
			break;
		}
		break;
	default:
		strcpy(buf, asm_args[opcode->src_id]);
		break;
	}

	return strdup(buf);
}

static void write_asm_str(struct emu8051_data *emu_data, uint16_t pc_tmp,
		uint16_t line)
{
	WINDOW *win = emu_data->win_data[DISASSEMBLY_WIN].win;
	struct emu8051_dev *dev = emu_data->emu_dev;
	const struct opcode_info *opcode;
	uint8_t i, op;
	int8_t offset;
	const char *op_str;
	char *asm_args[3];

	op = get_pmem(dev, pc_tmp);
	opcode = get_op_info(op);
	op_str = asm_ops[opcode->op_id];

	switch (opcode->args) {
	case 1:
		asm_args[0] = get_dst_str(dev, opcode, pc_tmp);
		asm_args[1] = NULL;
		asm_args[2] = NULL;

		mvwprintw(win, line, 9, "%s", op_str);
		mvwprintw(win, line, 15, "%s", asm_args[0]);
		break;
	case 2:
	case 3:
		asm_args[0] = get_dst_str(dev, opcode, pc_tmp);
		asm_args[1] = get_src_str(dev, opcode, pc_tmp);
		asm_args[2] = NULL;

		mvwprintw(win, line, 9, "%s", op_str);
		if (op != 0x85) {
			mvwprintw(win, line, 15, "%s", asm_args[0]);
			mvwprintw(win, line, 23, "%s", asm_args[1]);
		} else {
			mvwprintw(win, line, 15, "%s", asm_args[1]);
			mvwprintw(win, line, 23, "%s", asm_args[0]);
		}
		if (opcode->op_id == CJNE_ID) {
			offset = get_pmem(dev, pc_tmp + (opcode->length - 1));
			mvwprintw(win, line, 31, "0x%04x", pc_tmp = (pc_tmp + offset) + 3);
		}
		break;
	default:
		asm_args[0] = NULL;
		asm_args[1] = NULL;
		asm_args[2] = NULL;
		mvwprintw(win, line, 9, "%s", op_str);
		break;
	}

	for (i = 0; i < 3; i++) {
		if (asm_args[i] != NULL)
			free(asm_args[i]);
	}
}

static void update_disassembly_win(struct emu8051_data *emu_data)
{
	WINDOW *win = emu_data->win_data[DISASSEMBLY_WIN].win;
	struct emu8051_dev *dev = emu_data->emu_dev;
	uint16_t height = emu_data->win_data[DISASSEMBLY_WIN].height;
	uint16_t width = emu_data->win_data[DISASSEMBLY_WIN].width;
	uint16_t i, pc_tmp;
	uint8_t op;

	pc_tmp = emu_data->emu_dev->pc;

	for (i = 1; i < height - 1; i++)
		mvwhline(win, i, 1, ' ', width - 2);

	wattron(win, COLOR_PAIR(4));
	mvwprintw(win, 1, 1, "PC: 0x%04x", emu_data->emu_dev->pc);
	mvwprintw(win, 1, 20, "Ops: %d", emu_data->number);
	mvwprintw(win, 1, 35, "Back_ops: %d", emu_data->dyn_array.count);
	wattroff(win, COLOR_PAIR(4));

	for (i = 2; i < height - 1; i++) {
		wattron(win, COLOR_PAIR(3));
		mvwprintw(win, i, 1, "0x%04x:", pc_tmp);
		wattroff(win, COLOR_PAIR(3));

		if (emu_data->cur_window == DISASSEMBLY_WIN && i == 2)
			wattron(win, A_REVERSE);
		wattron(win, COLOR_PAIR(2));
		write_asm_str(emu_data, pc_tmp, i);
		wattron(win, COLOR_PAIR(2));
		if (emu_data->cur_window == DISASSEMBLY_WIN && i == 2)
			wattroff(win, A_REVERSE);

		op = get_pmem(dev, pc_tmp);
		pc_tmp += get_op_info(op)->length;
	}

	wrefresh(win);
}

/* Offset from the addr in IRAM and SFR windows. */

static void update_iram_win(struct emu8051_data *emu_data)
{
	struct emu_win_data *win_data = &emu_data->win_data[IRAM_WIN];
	WINDOW *win = win_data->win;
	struct emu8051_dev *dev = emu_data->emu_dev;
	uint16_t height = emu_data->win_data[IRAM_WIN].height;
	uint16_t i, y;
	uint8_t is_cur_win, iram_addr;

	if (emu_data->cur_window == IRAM_WIN)
		is_cur_win = 1;
	else
		is_cur_win = 0;

	for (i = 0; i < height - 2; i++) {
		wattron(win, COLOR_PAIR(3));
		mvwprintw(win, i + 1, 1, "0x%02x:", i * 8);
		wattroff(win, COLOR_PAIR(3));

		wattron(win, COLOR_PAIR(2));
		for (y = 0; y < 8; y++) {
			iram_addr = (i * 8) + y;
			if (win_data->cur_pos == iram_addr && is_cur_win)
				wattron(win, A_REVERSE);

			mvwprintw(win, i + 1, 7 + (y * 5), "0x%02x", dev->iram[iram_addr]);

			if (win_data->cur_pos == iram_addr && is_cur_win)
				wattroff(win, A_REVERSE);

		}
		wattroff(win, COLOR_PAIR(2));
	}

	wrefresh(win);
}

static void update_sfr_win(struct emu8051_data *emu_data)
{
	struct emu_win_data *win_data = &emu_data->win_data[SFR_WIN];
	WINDOW *win = win_data->win;
	struct emu8051_dev *dev = emu_data->emu_dev;
	uint16_t height = emu_data->win_data[SFR_WIN].height;
	uint16_t i, y;
	uint8_t is_cur_win, sfr_addr;

	if (emu_data->cur_window == SFR_WIN)
		is_cur_win = 1;
	else
		is_cur_win = 0;

	for (i = 0; i < height - 2; i++) {
		wattron(win, COLOR_PAIR(3));
		mvwprintw(win, i + 1, 1, "0x%02x:", i * 8 + 0x80);
		wattroff(win, COLOR_PAIR(3));

		wattron(win, COLOR_PAIR(2));
		for (y = 0; y < 8; y++) {
			sfr_addr = (i * 8) + y;
			if (win_data->cur_pos == sfr_addr && is_cur_win)
				wattron(win, A_REVERSE);

			mvwprintw(win, i + 1, 7 + (y * 5), "0x%02x", dev->sfr[(i * 8) + y]);

			if (win_data->cur_pos == sfr_addr && is_cur_win)
				wattroff(win, A_REVERSE);
		}
		wattroff(win, COLOR_PAIR(2));
	}

	wrefresh(win);
}

void update_exram_win(struct emu8051_data *emu_data)
{
	struct popup_win_data *popup_win = &emu_data->popup_data[XRAM_DISP_PANEL];
	struct emu8051_dev *dev = emu_data->emu_dev;
	WINDOW *win = popup_win->subwin;
	uint16_t height = popup_win->height;
	uint16_t exram_addr, i, y, tmp;
	uint32_t cur_row;

	for (i = 0; i < height - 4; i++) {
		cur_row = (i * 8) + popup_win->cur_offset;
		if (cur_row > 0xffff) {
			cur_row -= 0x10000;
		}

		wattron(win, COLOR_PAIR(3));
		mvwprintw(win, i, 0, "0x%04x:", cur_row);
		wattroff(win, COLOR_PAIR(3));

		wattron(win, COLOR_PAIR(2));
		for (y = 0; y < 8; y++) {
			exram_addr = cur_row + y;
			tmp = popup_win->cur_pos + popup_win->cur_offset;
			if (tmp == exram_addr)
				wattron(win, A_REVERSE);

			mvwprintw(win, i, 8 + (y * 5), "0x%02x", dev->xram[exram_addr]);

			if (tmp == exram_addr)
				wattroff(win, A_REVERSE);
		}
		wattroff(win, COLOR_PAIR(2));
	}

	wrefresh(win);
}

void update_win(struct emu8051_data *emu_data)
{
	WINDOW *win;
	uint16_t i;

	for (i = 0; i < NUM_OF_WINDOWS; i++) {
		win = emu_data->win_data[i].win;
		if (i == emu_data->cur_window)
			draw_border_colored(win, PAIR_BLUE);
		else
			draw_border_colored(win, PAIR_WHITE);
	}

	update_disassembly_win(emu_data);
	update_iram_win(emu_data);
	update_sfr_win(emu_data);
}

void show_popup(PANEL *panel)
{
	show_panel(panel);
	top_panel(panel);
	update_panels();
	doupdate();
}

void hide_popup(PANEL *panel)
{
	hide_panel(panel);
	update_panels();
	doupdate();
}

void ok_button_highlight(FIELD *ok_button, uint8_t cur_id)
{
	if (cur_id == OK_BUTTON) {
		curs_set(0);
		set_field_fore(ok_button, A_STANDOUT);
		set_field_back(ok_button, A_NORMAL);
	} else {
		curs_set(1);
		set_field_fore(ok_button, COLOR_PAIR(PAIR_RED));
		set_field_back(ok_button, COLOR_PAIR(PAIR_GREEN));
	}

}

void toggle_button_highlight(FIELD *toggle_label, uint8_t highlight)
{
	if (highlight) {
		curs_set(0);
		set_field_fore(toggle_label, A_STANDOUT);
		set_field_back(toggle_label, A_NORMAL);
	} else {
		curs_set(1);
		set_field_fore(toggle_label, COLOR_PAIR(PAIR_RED));
		set_field_back(toggle_label, COLOR_PAIR(PAIR_GREEN));
	}

}

void toggle_button_set(FIELD *toggle, uint8_t toggle_val)
{
	if (toggle_val)
		set_field_buffer(toggle, 0, "[x]");
	else
		set_field_buffer(toggle, 0, "[ ]");
}
