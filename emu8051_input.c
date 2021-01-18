#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <ncurses.h>
#include <string.h>
#include <panel.h>
#include <form.h>
#include <menu.h>
#include "emu8051_defs.h"

/*
 * Generic hex input handler. data_len is in nibbles, not bytes, so it
 * represents each ascii character.
 */
static int8_t hex_input(WINDOW *win, uint16_t y_pos, uint16_t x_pos,
			  uint8_t data_len, uint32_t *data_in)
{
	int ch;
	uint8_t keys;
	int8_t ret;

	keys = 0;
	ret = 0;
	*data_in = 0;

	write_hex(win, y_pos, x_pos, data_len, *data_in);

	while (keys < data_len) {
		ch = getch();
		switch (ch) {
		case '0' ... '9':
			*data_in =  (*data_in << 4) | (ch - 48);
			keys++;
			break;
		case 'a' ... 'f':
			*data_in = (*data_in << 4) | (ch - 97 + 10);
			keys++;
			break;
		case 'A' ... 'F':
			*data_in = (*data_in << 4) | (ch - 65 + 10);
			keys++;
			break;
		case '\n':
			keys = data_len;
			break;
		case KEY_F(1):
		case 27:
			ret = -1;
			break;
		default:
			break;
		}

		write_hex(win, y_pos, x_pos, data_len, *data_in);

		if (keys > (data_len - 1) || ret < 0)
			break;
	}

	return ret;
}

static int check_input(uint8_t in_type, int ch)
{
	int ret;

	switch (in_type) {
	case INPUT_HEX:
		switch (ch) {
		case 'a' ... 'f':
		case 'A' ... 'F':
		case '0' ... '9':
			ret = ch;
			break;
		default:
			ret = 0;
			break;
		}
		break;
	case INPUT_DEC:
		switch (ch) {
		case '0' ... '9':
			ret = ch;
			break;
		default:
			ret = 0;
			break;
		}

		break;
	case INPUT_ALPHANUM:
		ret = ch;
		break;
	case INPUT_TOGGLE:
	case INPUT_NONE:
		ret = 0;
		break;
	default:
		ret = 0;
		break;
	}

	return ret;
}

static void field_switch(struct popup_win_data *popup_win, struct field_data *field_ptr,
			 int direction)
{
	if (field_ptr->in_type == INPUT_TOGGLE)
		toggle_button_highlight(popup_win->field[field_ptr->field_id + 1], 0);
	form_driver(popup_win->form, direction);
}

static int field_input_handler(struct popup_win_data *popup_win)
{
	struct field_data *field_ptr;
        int ch, end_loop;

	show_popup(popup_win->panel);
	post_form(popup_win->form);

	set_current_field(popup_win->form,
			  popup_win->field[popup_win->default_in]);
	field_ptr = (struct field_data*)field_userptr(current_field(popup_win->form));

	//Last button is always OK button.
	ok_button_highlight(popup_win->field[popup_win->num_of_fields - 1],
			field_ptr->field_id);
	if (field_ptr->in_type == INPUT_TOGGLE)
		toggle_button_highlight(popup_win->field[field_ptr->field_id + 1], 1);
	// Some form of (if (field_ptr->in_type == INPUT_TOGGLE), then highlight.
	// Then, a way to un-highlight it if we're no longer selecting it. So,
	// if requesting a new field, and the current field is a toggle, then
	// un-highlight...
	wrefresh(popup_win->win);

	end_loop = 0;
	while((ch = wgetch(popup_win->win)) != KEY_F(1))
	{
		switch(ch) {
		case KEY_DOWN:
			field_switch(popup_win, field_ptr, REQ_DOWN_FIELD);
			break;
		case KEY_UP:
			field_switch(popup_win, field_ptr, REQ_UP_FIELD);
			break;
		case KEY_LEFT:
			field_switch(popup_win, field_ptr, REQ_LEFT_FIELD);
			break;
		case KEY_RIGHT:
			field_switch(popup_win, field_ptr, REQ_RIGHT_FIELD);
			break;
		case '\n':
			if (field_ptr->field_id == OK_BUTTON)
				end_loop = 1;
			else if (field_ptr->in_type == INPUT_TOGGLE) {
				field_ptr->toggle_val = !field_ptr->toggle_val;
				toggle_button_set(current_field(popup_win->form), field_ptr->toggle_val);
			} else
				form_driver(popup_win->form, REQ_NEXT_FIELD);

			break;
		default:
			if (check_input(field_ptr->in_type, ch))
				form_driver(popup_win->form, ch);
			break;
		}

		if (end_loop)
			break;

		field_ptr = (struct field_data*)field_userptr(current_field(popup_win->form));
		ok_button_highlight(popup_win->field[popup_win->num_of_fields - 1],
				field_ptr->field_id);
		if (field_ptr->in_type == INPUT_TOGGLE)
			toggle_button_highlight(popup_win->field[field_ptr->field_id + 1], 1);
	}

	return end_loop;
}

static void pc_win_input(struct emu8051_data *emu_data)
{
	struct popup_win_data *popup_win = &emu_data->popup_data[PC_IN_PANEL];
        int end_loop;

	end_loop = field_input_handler(popup_win);

	if (end_loop)
		emu_data->emu_dev->pc = strtol(field_buffer(popup_win->field[PC_INPUT], 0), NULL, 16);

	set_field_buffer(popup_win->field[PC_INPUT], 0, "");

	unpost_form(popup_win->form);
	hide_popup(popup_win->panel);
}

static void verb_win_input(struct emu8051_data *emu_data)
{
	struct popup_win_data *popup_win = &emu_data->popup_data[VERB_IN_PANEL];
        int end_loop, i;
	uint16_t in_vals[3];


	end_loop = field_input_handler(popup_win);

	if (end_loop) {
		for (i = 0; i < 3; i++) {
			in_vals[i] = strtol(field_buffer(popup_win->field[2 + (i * 3)], 0),
						         NULL, 16);
			// Clear the field after reading from it.
			set_field_buffer(popup_win->field[2 + (i * 3)], 0, "");
		}
		setup_verb_handler(emu_data, in_vals[0], in_vals[1], in_vals[2]);
	}

        unpost_form(popup_win->form);
	hide_popup(popup_win->panel);
}

static void state_save_win_input(struct emu8051_data *emu_data)
{
	struct popup_win_data *popup_win = &emu_data->popup_data[SAVE_STATE_PANEL];
	struct field_data *field_ptr;
        int end_loop;

	/* This is where our non-creation stuff starts. */
	show_popup(popup_win->panel);
	post_form(popup_win->form);

	set_current_field(popup_win->form, popup_win->field[STATE_SAVE_IN]);
	form_driver(popup_win->form, REQ_CLR_FIELD);

	field_ptr = (struct field_data*)field_userptr(current_field(popup_win->form));
	ok_button_highlight(popup_win->field[STATE_SAVE_OK_BUTTON], field_ptr->field_id);

	wrefresh(popup_win->win);

	end_loop = field_input_handler(popup_win);

	if (end_loop)
		save_state_to_file(emu_data,
				field_buffer(popup_win->field[STATE_SAVE_IN], 0));

	unpost_form(popup_win->form);
	hide_popup(popup_win->panel);
}

static void logging_setup_win_input(struct emu8051_data *emu_data)
{
	struct popup_win_data *popup_win = &emu_data->popup_data[LOG_ENABLE_PANEL];
	struct field_data *field_ptr;
        int end_loop;

	/* If we're already logging, stop before opening a new one. */
	if (emu_data->log_data.logging_set)
		logging_close_log(emu_data);

	/* This is where our non-creation stuff starts. */
	show_popup(popup_win->panel);
	post_form(popup_win->form);

	set_current_field(popup_win->form, popup_win->field[LOG_ENABLE_IN]);
	form_driver(popup_win->form, REQ_CLR_FIELD);

	field_ptr = (struct field_data*)field_userptr(current_field(popup_win->form));
	ok_button_highlight(popup_win->field[LOG_ENABLE_OK_BUTTON], field_ptr->field_id);

	wrefresh(popup_win->win);

	end_loop = field_input_handler(popup_win);

	if (end_loop) {
		open_log_file(emu_data,
				field_buffer(popup_win->field[LOG_ENABLE_IN], 0));

		field_ptr = field_userptr(popup_win->field[LOG_ENABLE_VERB_SWITCH]);
		emu_data->log_data.exit_on_verb = field_ptr->toggle_val;
		if (emu_data->log_data.exit_on_verb)
			fputs("Exit on verb handling enabled.\n", emu_data->log_data.log_file);

		/* Check if we need to create a save file as well. */
		field_ptr = field_userptr(popup_win->field[LOG_ENABLE_SAVE_SWITCH]);
		if (field_ptr->toggle_val) {
			emu_data->number = 0;
			save_state_to_file(emu_data,
					field_buffer(popup_win->field[LOG_ENABLE_IN], 0));
		}
	}

	unpost_form(popup_win->form);
	hide_popup(popup_win->panel);
}

static void settings_win_input(struct emu8051_data *emu_data)
{
	struct popup_win_data *popup_win = &emu_data->popup_data[SETTINGS_PANEL];
	struct field_data *field_ptr;
	int end_loop;
	char buf[40];

	sprintf(buf, "%d", emu_data->step_size);
	set_field_buffer(popup_win->field[SETTINGS_STEP_SIZE_IN], 0, buf);
	sprintf(buf, "%d", (emu_data->dyn_array.size - 1) * ARR_BLOCK_SIZE);
	set_field_buffer(popup_win->field[SETTINGS_BACKLOG_IN], 0, buf);
	sprintf(buf, "%04x", emu_data->break_data.pc_break);
	set_field_buffer(popup_win->field[SETTINGS_PC_BRK_IN], 0, buf);
	sprintf(buf, "%04x", emu_data->break_data.dptr_break);
	set_field_buffer(popup_win->field[SETTINGS_DPTR_BRK_IN], 0, buf);


	end_loop = field_input_handler(popup_win);

	if (end_loop) {
		resize_dynarray(&emu_data->dyn_array, strtol(field_buffer(popup_win->field[SETTINGS_BACKLOG_IN], 0), NULL, 10));
		emu_data->step_size = strtol(field_buffer(popup_win->field[SETTINGS_STEP_SIZE_IN], 0), NULL, 10);
	}

	field_ptr = field_userptr(popup_win->field[SETTINGS_PC_BRK_SWITCH]);
	breakpoint_flag_set(emu_data, FLAG_BRK_PC, field_ptr->toggle_val);
	emu_data->break_data.pc_break =
		strtol(field_buffer(popup_win->field[SETTINGS_PC_BRK_IN], 0), NULL, 16);

	field_ptr = field_userptr(popup_win->field[SETTINGS_DPTR_BRK_SWITCH]);
	breakpoint_flag_set(emu_data, FLAG_BRK_DPTR, field_ptr->toggle_val);
	emu_data->break_data.dptr_break =
		strtol(field_buffer(popup_win->field[SETTINGS_DPTR_BRK_IN], 0), NULL, 16);

	unpost_form(popup_win->form);
	hide_popup(popup_win->panel);
}

#define XRAM_DATA_ADDR_OFFSET 8
static void exram_win_input(struct emu8051_data *emu_data)
{
	struct popup_win_data *popup_win = &emu_data->popup_data[XRAM_DISP_PANEL];
	struct emu8051_dev *dev = emu_data->emu_dev;
	uint8_t x_off, y_off;
	uint32_t res;
	int ch;

	/* This is where our non-creation stuff starts. */
	show_popup(popup_win->panel);
	wrefresh(popup_win->win);
	update_exram_win(emu_data);

	while((ch = wgetch(popup_win->win)) != KEY_F(1))
	{
		switch(ch) {
		case KEY_LEFT:
			popup_win->cur_pos--;
			break;
		case KEY_RIGHT:
			popup_win->cur_pos++;
			break;
		case KEY_UP:
			popup_win->cur_pos -= 8;
			break;
		case KEY_DOWN:
			popup_win->cur_pos += 8;
			break;
		case KEY_PPAGE:
//			if (popup_win->cur_offset > 0xff)
			popup_win->cur_offset -= 0x100;
			break;
		case KEY_NPAGE:
			popup_win->cur_offset += 0x100;
			break;
		case '\n':
			wattron(popup_win->subwin, A_BLINK);

			y_off = (popup_win->cur_pos / 8);
			x_off = XRAM_DATA_ADDR_OFFSET + (popup_win->cur_pos % 8) * 5;

			if (hex_input(popup_win->subwin, y_off, x_off, 2, &res) >= 0)
				dev->xram[popup_win->cur_pos + popup_win->cur_offset] = res & 0xff;

			wattroff(popup_win->subwin, A_BLINK);
			break;
		default:
			break;
		}

		if (popup_win->cur_pos > 0xff && popup_win->cur_pos < 0x110) {
			popup_win->cur_offset += 8;
			popup_win->cur_pos -= 8;
		}
		if (popup_win->cur_pos > 0x110) {
			popup_win->cur_pos += 8;
			popup_win->cur_offset -= 8;
		}
		update_exram_win(emu_data);
	}

	hide_popup(popup_win->panel);
}

static void disassembly_win_input(struct emu8051_data *emu_data, int in_ch)
{
	struct emu8051_dev *emu_dev = emu_data->emu_dev;
	struct op_change op_ch;
	uint8_t ret;
	uint32_t y;

	emu_dev->op_ch = &op_ch;
	switch (in_ch) {
	case KEY_RIGHT:
	case '\n':
		handle_opcode(emu_data);
		write_front(&emu_data->dyn_array, op_ch);
		emu_data->number++;
		break;
	case KEY_LEFT:
		emu_dev->op_ch = pop_front(&emu_data->dyn_array);
		if (emu_dev->op_ch == NULL)
			break;
		reverse_op(emu_dev, emu_dev->op_ch);
		emu_data->number--;
		break;
	case KEY_UP:
		for (y = 0; y < emu_data->step_size; y++) {
			ret = handle_opcode(emu_data);
			write_front(&emu_data->dyn_array, op_ch);
			emu_data->number++;
			if (ret)
				return;
		}
		break;
	case KEY_DOWN:
		for (y = 0; y < emu_data->step_size; y++) {
			emu_dev->op_ch = pop_front(&emu_data->dyn_array);
			if (emu_dev->op_ch == NULL)
				break;
			reverse_op(emu_dev, emu_dev->op_ch);
			emu_data->number--;
		}
		break;
	case 'e':
		exram_win_input(emu_data);
		break;
	case 'f':
		settings_win_input(emu_data);
		break;
	case 'o':
		pc_win_input(emu_data);
		break;
	case 's':
		state_save_win_input(emu_data);
		break;
	case 'l':
		logging_setup_win_input(emu_data);
		break;
	case 'v':
		verb_win_input(emu_data);
		break;
	}

	emu_dev->op_ch = NULL;
}

/*
 * For these two windows, need to keep track of current position with the key
 * press. Then, with that data, if enter is pressed, we can then edit the data
 * of the currently selected item. This mechanism can be pretty much the same
 * on both windows, with different limits on the range.
 */
#define DATA_ADDR_OFFSET 7
static void iram_win_input(struct emu8051_data *emu_data, int in_ch)
{
	struct emu_win_data *win_data = &emu_data->win_data[IRAM_WIN];
	struct emu8051_dev *dev = emu_data->emu_dev;
	int ch, keys = 0;
	uint32_t res;
	uint8_t x_off, y_off;

	switch (in_ch) {
	case KEY_LEFT:
		win_data->cur_pos--;
		break;
	case KEY_RIGHT:
		win_data->cur_pos++;
		break;
	case KEY_UP:
		win_data->cur_pos -= 8;
		break;
	case KEY_DOWN:
		win_data->cur_pos += 8;
		break;
	case KEY_ENTER:
		while((ch = getch()) != KEY_F(1)) {
			keys++;
			switch (ch) {
			default:
				break;
			}
			if (keys == 2)
				break;
		}

		break;
	case '+':
		dev->iram[win_data->cur_pos]++;
		break;
	case '-':
		dev->iram[win_data->cur_pos]--;
		break;
	case '\n':
		wattron(win_data->win, A_BLINK);

		y_off = (win_data->cur_pos / 8) + 1;
		x_off = DATA_ADDR_OFFSET + (win_data->cur_pos % 8) * 5;

		if (hex_input(win_data->win, y_off, x_off, 2, &res) >= 0)
			dev->iram[win_data->cur_pos] = res & 0xff;

		wattroff(win_data->win, A_BLINK);

		break;
	}
}

static void sfr_win_input(struct emu8051_data *emu_data, int in_ch)
{
	struct emu_win_data *win_data = &emu_data->win_data[SFR_WIN];
	struct emu8051_dev *dev = emu_data->emu_dev;
	uint32_t res;
	uint8_t y_off, x_off;

	switch (in_ch) {
	case KEY_LEFT:
		win_data->cur_pos--;
		break;
	case KEY_RIGHT:
		win_data->cur_pos++;
		break;
	case KEY_UP:
		win_data->cur_pos -= 8;
		break;
	case KEY_DOWN:
		win_data->cur_pos += 8;
		break;
	case '+':
		write_sfr(dev, win_data->cur_pos, dev->sfr[win_data->cur_pos] + 1);
		break;
	case '-':
		write_sfr(dev, win_data->cur_pos, dev->sfr[win_data->cur_pos] - 1);
		break;
	case '\n':
		wattron(win_data->win, A_BLINK);

		y_off = (win_data->cur_pos / 8) + 1;
		x_off = DATA_ADDR_OFFSET + (win_data->cur_pos % 8) * 5;

		if (hex_input(win_data->win, y_off, x_off, 2, &res) >= 0)
			dev->sfr[win_data->cur_pos] = res & 0xff;

		wattroff(win_data->win, A_BLINK);
		break;
	}

	if (win_data->cur_pos > 0x7f)
		win_data->cur_pos -= 0x80;
}

void emu8051_in_handler(struct emu8051_data *emu_data, int in_ch)
{
	switch (in_ch) {
	case '\t':
		emu_data->cur_window++;

		if (emu_data->cur_window >= NUM_OF_WINDOWS)
			emu_data->cur_window = 0;

		break;
	default:
		switch (emu_data->cur_window) {
		case DISASSEMBLY_WIN:
			disassembly_win_input(emu_data, in_ch);
			break;
		case IRAM_WIN:
			iram_win_input(emu_data, in_ch);
			break;
		case SFR_WIN:
			sfr_win_input(emu_data, in_ch);
			break;
		}
		break;
	}
}
