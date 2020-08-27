#include <form.h>
#include <menu.h>


#define NUM_OF_OPS 45
const char *asm_ops[] = {
	"ACALL", "ADD", "ADDC", "AJMP", "ANL", "CJNE", "CLR", "CPL", "DA",
	"DEC", "DIV", "DJNZ", "INC", "JB", "JBC", "JC", "JMP", "JNB", "JNC",
	"JNZ", "JZ", "LCALL", "LJMP", "MOV", "MOVC", "MOVX", "MUL", "NOP",
	"ORL", "POP", "PUSH", "RSRV", "RET", "RETI", "RL", "RLC", "RR", "RRC",
	"SETB", "SJMP", "SUBB", "SWAP", "XCH", "XCHD", "XRL"
};

#define NUM_OF_ARGS 24
const char *asm_args[] = {
	"A", "addr11", "addr16", "direct", "@R0", "@R1", "R0", "R1", "R2",
	"R3", "R4", "R5", "R6", "R7", "bit", "/bit", "offset", "#immed", "C", "@A+DPTR",
	"DPTR", "@DPTR", "AB", "@A+PC"
};

//#define NUM_OF_SFR_STRS 31
#define NUM_OF_SFR_STRS 34
const char *sfr_strs[] = {
	"P0", "SP", "DPL_0", "DPH_0", "DPL_1", "DPH_1", "DP_TGL", "PCON", "TCON", 
	"TMOD", "TC0L", "TC0H", "TC1L", "TC1H", "P1", "SCON", "SBUF", "P2", "P3",
	"HICA0", "HICA1", "HICA2", "HICD0", "HICD1", "HICD2", "HICD3", "IP",
	"PSW", "ACC", "SFR_B", "BANK_SW", "VRB_LA", "VRB_RE", "VRB_IN",
};
	                                  
const struct emu_win_info win_info[NUM_OF_WINDOWS] = {
	// Disassembly window.
	{ .height = 50,
	  .width  = 35,
	  .startx = 0,
	  .starty = 0,
	},
	// IRAM window.
	{ .height = 34,
	  .width  = 48,
	  .startx = 35,
	  .starty = 0,
	},
	// SFR window.
	{ .height = 18,
	  .width  = 48,
	  .startx = 35,
	  .starty = 35,
	},
};
/*
enum pc_in_field_ids {
	PC_INPUT,
	PC_OK_BUTTON,
	PC_LABEL,
	PC_HEX_LABEL,
};

enum verb_in_field_ids {
	NID_LABEL,
	NID_HEX_LABEL,
	NID_IN,
	VERB_LABEL,
	VERB_HEX_LABEL,
	VERB_IN,
	VAL_LABEL,
	VAL_HEX_LABEL,
	VAL_IN,
	VERB_OK_BUTTON,
};

enum state_save_field_ids {
	STATE_SAVE_NAME_LABEL,
	STATE_SAVE_IN,
	STATE_SAVE_OK_BUTTON,
};

#define OK_BUTTON 0x80
*/
#define AUTO_ADJUST 0
const struct popup_win_info popup_defs[NUM_OF_POPUPS] = {
	/* Program counter input popup */
	{ .title = "Program Counter Input",
	  .height = 8,
	  .width = AUTO_ADJUST,
	  .num_of_fields = 4,
	  .num_of_inputs = 1, 
	  .default_in    = PC_INPUT,
	  .field_info = {
	     { .field_label = "PC: ",
	       .field_id    = PC_LABEL,
	       .in_type     = INPUT_NONE,
	       .off_flags   = O_ACTIVE,
	       .field_back  = A_NORMAL,
	       .fore_color  = PAIR_RED,
	       .height    = 1,
	       .width     = 4,
	       .toprow    = 0,
	       .leftcol   = 1,
	       .offscreen = 0,
	       .nbuffers  = 0,
	     },
	     { .field_label = "0x",
	       .field_id    = PC_HEX_LABEL,
	       .in_type     = INPUT_NONE,
	       .off_flags   = O_ACTIVE,
	       .field_back  = A_NORMAL,
	       .fore_color  = PAIR_WHITE,
	       .height    = 1,
	       .width     = 2,
	       .toprow    = 0,
	       .leftcol   = 5,
	       .offscreen = 0,
	       .nbuffers  = 0,
	     },
	     { .field_label = NULL,
	       .field_id    = PC_INPUT,
	       .in_type     = INPUT_HEX,
	       .off_flags   = 0xff,
	       .field_back  = A_NORMAL,
	       .fore_color  = PAIR_WHITE,
	       .height    = 1,
	       .width     = 4,
	       .toprow    = 0,
	       .leftcol   = 7,
	       .offscreen = 0,
	       .nbuffers  = 0,
	     },
	     { .field_label = "[OK]",
	       .field_id    = OK_BUTTON,
	       .in_type     = INPUT_NONE,
	       .off_flags   = O_AUTOSKIP,
	       .fore_color  = PAIR_RED,
	       .height    = 1,
	       .width     = 4,
	       .toprow    = 2,
	       .leftcol   = 3,
	       .offscreen = 0,
	       .nbuffers  = 0,
	     },
	  },
	},
	{ .title = "Verb Input",
	  .height = 8,
	  .width = AUTO_ADJUST,
	  .num_of_fields = 10,
	  .num_of_inputs = 3, 
	  .default_in    = NID_IN,
	  .field_info = {
	     { .field_label = "NID: ",
	       .field_id    = NID_LABEL,
	       .in_type     = INPUT_NONE,
	       .off_flags   = O_ACTIVE,
	       .field_back  = A_NORMAL,
	       .fore_color  = PAIR_RED,
	       .height    = 1,
	       .width     = 5,
	       .toprow    = 0,
	       .leftcol   = 1,
	       .offscreen = 0,
	       .nbuffers  = 0,
	     },
	     { .field_label = "0x",
	       .field_id    = NID_HEX_LABEL,
	       .in_type     = INPUT_NONE,
	       .off_flags   = O_ACTIVE,
	       .field_back  = A_NORMAL,
	       .fore_color  = PAIR_WHITE,
	       .height    = 1,
	       .width     = 2,
	       .toprow    = 0,
	       .leftcol   = 6,
	       .offscreen = 0,
	       .nbuffers  = 0,
	     },
	     { .field_label = NULL,
	       .field_id    = NID_IN,
	       .in_type     = INPUT_HEX,
	       .off_flags   = 0xff,
	       .field_back  = A_NORMAL,
	       .fore_color  = PAIR_WHITE,
	       .height    = 1,
	       .width     = 2,
	       .toprow    = 0,
	       .leftcol   = 8,
	       .offscreen = 0,
	       .nbuffers  = 0,
	     },
	     { .field_label = "Verb: ",
	       .field_id    = VERB_LABEL,
	       .in_type     = INPUT_NONE,
	       .off_flags   = O_ACTIVE,
	       .field_back  = A_NORMAL,
	       .fore_color  = PAIR_RED,
	       .height    = 1,
	       .width     = 6,
	       .toprow    = 0,
	       .leftcol   = 11,
	       .offscreen = 0,
	       .nbuffers  = 0,
	     },
	     { .field_label = "0x",
	       .field_id    = VERB_HEX_LABEL,
	       .in_type     = INPUT_NONE,
	       .off_flags   = O_ACTIVE,
	       .field_back  = A_NORMAL,
	       .fore_color  = PAIR_WHITE,
	       .height    = 1,
	       .width     = 2,
	       .toprow    = 0,
	       .leftcol   = 17,
	       .offscreen = 0,
	       .nbuffers  = 0,
	     },
	     { .field_label = NULL,
	       .field_id    = VERB_IN,
	       .in_type     = INPUT_HEX,
	       .off_flags   = 0xff,
	       .field_back  = A_NORMAL,
	       .fore_color  = PAIR_WHITE,
	       .height    = 1,
	       .width     = 3,
	       .toprow    = 0,
	       .leftcol   = 19,
	       .offscreen = 0,
	       .nbuffers  = 0,
	     },
	     { .field_label = "Val: ",
	       .field_id    = VAL_LABEL,
	       .in_type     = INPUT_NONE,
	       .off_flags   = O_ACTIVE,
	       .field_back  = A_NORMAL,
	       .fore_color  = PAIR_RED,
	       .height    = 1,
	       .width     = 5,
	       .toprow    = 0,
	       .leftcol   = 23,
	       .offscreen = 0,
	       .nbuffers  = 0,
	     },
	     { .field_label = "0x",
	       .field_id    = VAL_HEX_LABEL,
	       .in_type     = INPUT_NONE,
	       .off_flags   = O_ACTIVE,
	       .field_back  = A_NORMAL,
	       .fore_color  = PAIR_WHITE,
	       .height    = 1,
	       .width     = 2,
	       .toprow    = 0,
	       .leftcol   = 28,
	       .offscreen = 0,
	       .nbuffers  = 0,
	     },
	     { .field_label = NULL,
	       .field_id    = VAL_IN,
	       .in_type     = INPUT_HEX,
	       .off_flags   = 0xff,
	       .field_back  = A_NORMAL,
	       .fore_color  = PAIR_WHITE,
	       .height    = 1,
	       .width     = 2,
	       .toprow    = 0,
	       .leftcol   = 30,
	       .offscreen = 0,
	       .nbuffers  = 0,
	     },
	     { .field_label = "[OK]",
	       .field_id    = OK_BUTTON,
	       .in_type     = INPUT_NONE,
	       .off_flags   = O_AUTOSKIP,
	       .field_back  = A_NORMAL,
	       .fore_color  = PAIR_RED,
	       .height    = 1,
	       .width     = 4,
	       .toprow    = 2,
	       .leftcol   = 14,
	       .offscreen = 0,
	       .nbuffers  = 0,
	     },
	  },
	},
	{ .title = "Save State",
	  .height = 8,
	  .width = AUTO_ADJUST,
	  .num_of_fields = 3,
	  .num_of_inputs = 1, 
	  .default_in    = STATE_SAVE_IN,
	  .field_info = {
	     { .field_label = "Filename: ",
	       .field_id    = STATE_SAVE_NAME_LABEL,
	       .in_type     = INPUT_NONE,
	       .off_flags   = O_ACTIVE,
	       .field_back  = A_NORMAL,
	       .fore_color  = PAIR_RED,
	       .height    = 1,
	       .width     = 10,
	       .toprow    = 0,
	       .leftcol   = 1,
	       .offscreen = 0,
	       .nbuffers  = 0,
	     },
	     { .field_label = NULL,
	       .field_id    = STATE_SAVE_IN,
	       .in_type     = INPUT_ALPHANUM,
	       .off_flags   = O_STATIC,
	       .field_back  = A_UNDERLINE,
	       .fore_color  = PAIR_WHITE,
	       .height    = 1,
	       .width     = 8,
	       .toprow    = 0,
	       .leftcol   = 11,
	       .offscreen = 0,
	       .nbuffers  = 0,
	     },
	     { .field_label = "[OK]",
	       .field_id    = OK_BUTTON,
	       .in_type     = INPUT_NONE,
	       .off_flags   = O_AUTOSKIP,
	       .field_back  = A_NORMAL,
	       .fore_color  = PAIR_RED,
	       .height    = 1,
	       .width     = 4,
	       .toprow    = 2,
	       .leftcol   = 8,
	       .offscreen = 0,
	       .nbuffers  = 0,
	     },
	  },
	},
	{ .title  = "XRAM",
	  .height = AUTO_ADJUST,
	  .width  = 50,
	  .num_of_fields = 0,
	},
	{ .title = "Settings",
	  .height = AUTO_ADJUST,
	  .width = AUTO_ADJUST,
	  .num_of_fields = SETTINGS_OK_BUTTON + 1,
	  .num_of_inputs = 4, 
	  .default_in    = SETTINGS_STEP_SIZE_IN,
	  .field_info = {
	     { .field_label = "   Step size: ",
	       .field_id    = SETTINGS_STEP_SIZE_LABEL,
	       .in_type     = INPUT_NONE,
	       .off_flags   = O_ACTIVE,
	       .field_back  = A_NORMAL,
	       .fore_color  = PAIR_RED,
	       .height    = 1,
	       .width     = 14,
	       .toprow    = 0,
	       .leftcol   = 1,
	       .offscreen = 0,
	       .nbuffers  = 0,
	     },
	     { .field_label = NULL,
	       .field_id    = SETTINGS_STEP_SIZE_IN,
	       .in_type     = INPUT_DEC,
	       .off_flags   = 0xff,
	       .field_back  = A_NORMAL,
	       .fore_color  = PAIR_WHITE,
	       .height    = 1,
	       .width     = 8,
	       .toprow    = 0,
	       .leftcol   = 15,
	       .offscreen = 0,
	       .nbuffers  = 0,
	     },
	     { .field_label = "Backlog size: ",
	       .field_id    = SETTINGS_BACKLOG_LABEL,
	       .in_type     = INPUT_NONE,
	       .off_flags   = O_ACTIVE,
	       .field_back  = A_NORMAL,
	       .fore_color  = PAIR_RED,
	       .height    = 1,
	       .width     = 14,
	       .toprow    = 1,
	       .leftcol   = 1,
	       .offscreen = 0,
	       .nbuffers  = 0,
	     },
	     { .field_label = NULL,
	       .field_id    = SETTINGS_BACKLOG_IN,
	       .in_type     = INPUT_DEC,
	       .off_flags   = 0xff,
	       .field_back  = A_NORMAL,
	       .fore_color  = PAIR_WHITE,
	       .height    = 1,
	       .width     = 8,
	       .toprow    = 1,
	       .leftcol   = 15,
	       .offscreen = 0,
	       .nbuffers  = 0,
	     },
	     { .field_label = "Breakpoints:",
	       .field_id    = SETTINGS_BREAKPOINTS_LABEL,
	       .in_type     = INPUT_NONE,
	       .off_flags   = O_ACTIVE,
	       .field_back  = A_NORMAL,
	       .fore_color  = PAIR_RED,
	       .height    = 1,
	       .width     = 14,
	       .toprow    = 3,
	       .leftcol   = 5,
	       .offscreen = 0,
	       .nbuffers  = 0,
	     },
	     { .field_label = "[ ]",
	       .field_id    = SETTINGS_PC_BRK_SWITCH,
	       .button_id   = BUTTON_TOGGLE_PC,
	       .in_type     = INPUT_TOGGLE,
	       .off_flags   = O_AUTOSKIP,
	       .field_back  = A_NORMAL,
	       .fore_color  = PAIR_WHITE,
	       .height    = 1,
	       .width     = 3,
	       .toprow    = 5,
	       .leftcol   = 0,
	       .offscreen = 0,
	       .nbuffers  = 0,
	     },
	     { .field_label = "PC: ",
	       .field_id    = SETTINGS_PC_BRK_LABEL,
	       .in_type     = INPUT_NONE,
	       .off_flags   = O_ACTIVE,
	       .field_back  = A_NORMAL,
	       .fore_color  = PAIR_RED,
	       .height    = 1,
	       .width     = 6,
	       .toprow    = 5,
	       .leftcol   = 4,
	       .offscreen = 0,
	       .nbuffers  = 0,
	     },
	     { .field_label = "0x",
	       .field_id    = SETTINGS_PC_BRK_HEX_LABEL,
	       .in_type     = INPUT_NONE,
	       .off_flags   = O_ACTIVE,
	       .field_back  = A_NORMAL,
	       .fore_color  = PAIR_WHITE,
	       .height    = 1,
	       .width     = 2,
	       .toprow    = 5,
	       .leftcol   = 11,
	       .offscreen = 0,
	       .nbuffers  = 0,
	     },
	     { .field_label = NULL,
	       .field_id    = SETTINGS_PC_BRK_IN,
	       .in_type     = INPUT_HEX,
	       .off_flags   = 0xff,
	       .field_back  = A_NORMAL,
	       .fore_color  = PAIR_WHITE,
	       .height    = 1,
	       .width     = 4,
	       .toprow    = 5,
	       .leftcol   = 13,
	       .offscreen = 0,
	       .nbuffers  = 0,
	     },
	     { .field_label = "[ ]",
	       .field_id    = SETTINGS_DPTR_BRK_SWITCH,
	       .button_id   = BUTTON_TOGGLE_DPTR,
	       .in_type     = INPUT_TOGGLE,
	       .off_flags   = O_AUTOSKIP,
	       .field_back  = A_NORMAL,
	       .fore_color  = PAIR_WHITE,
	       .height    = 1,
	       .width     = 3,
	       .toprow    = 6,
	       .leftcol   = 0,
	       .offscreen = 0,
	       .nbuffers  = 0,
	     },
	     { .field_label = "DPTR: ",
	       .field_id    = SETTINGS_DPTR_BRK_LABEL,
	       .in_type     = INPUT_NONE,
	       .off_flags   = O_ACTIVE,
	       .field_back  = A_NORMAL,
	       .fore_color  = PAIR_RED,
	       .height    = 1,
	       .width     = 6,
	       .toprow    = 6,
	       .leftcol   = 4,
	       .offscreen = 0,
	       .nbuffers  = 0,
	     },
	     { .field_label = "0x",
	       .field_id    = SETTINGS_DPTR_BRK_HEX_LABEL,
	       .in_type     = INPUT_NONE,
	       .off_flags   = O_ACTIVE,
	       .field_back  = A_NORMAL,
	       .fore_color  = PAIR_WHITE,
	       .height    = 1,
	       .width     = 2,
	       .toprow    = 6,
	       .leftcol   = 11,
	       .offscreen = 0,
	       .nbuffers  = 0,
	     },
	     { .field_label = NULL,
	       .field_id    = SETTINGS_DPTR_BRK_IN,
	       .in_type     = INPUT_HEX,
	       .off_flags   = 0xff,
	       .field_back  = A_NORMAL,
	       .fore_color  = PAIR_WHITE,
	       .height    = 1,
	       .width     = 4,
	       .toprow    = 6,
	       .leftcol   = 13,
	       .offscreen = 0,
	       .nbuffers  = 0,
	     },
	     { .field_label = "[OK]",
	       .field_id    = OK_BUTTON,
	       .in_type     = INPUT_NONE,
	       .off_flags   = O_AUTOSKIP,
	       .field_back  = A_NORMAL,
	       .fore_color  = PAIR_RED,
	       .height    = 1,
	       .width     = 4,
	       .toprow    = 9,
	       .leftcol   = 9,
	       .offscreen = 0,
	       .nbuffers  = 0,
	     },
	   },
	},
	{ .title = "Log File Setup",
	  .height = 10,
	  .width = AUTO_ADJUST,
	  .num_of_fields = 5,
	  .num_of_inputs = 1,
	  .default_in    = LOG_ENABLE_IN,
	  .field_info = {
	     { .field_label = "Filename: ",
	       .field_id    = LOG_ENABLE_NAME_LABEL,
	       .in_type     = INPUT_NONE,
	       .off_flags   = O_ACTIVE,
	       .field_back  = A_NORMAL,
	       .fore_color  = PAIR_RED,
	       .height    = 1,
	       .width     = 10,
	       .toprow    = 0,
	       .leftcol   = 0,
	       .offscreen = 0,
	       .nbuffers  = 0,
	     },
	     { .field_label = NULL,
	       .field_id    = LOG_ENABLE_IN,
	       .in_type     = INPUT_ALPHANUM,
	       .off_flags   = O_STATIC,
	       .field_back  = A_UNDERLINE,
	       .fore_color  = PAIR_WHITE,
	       .height    = 1,
	       .width     = 19,
	       .toprow    = 0,
	       .leftcol   = 10,
	       .offscreen = 0,
	       .nbuffers  = 0,
	     },
	     { .field_label = "[ ]",
	       .field_id    = LOG_ENABLE_VERB_SWITCH,
	       .button_id   = BUTTON_TOGGLE_VERB_LOG,
	       .in_type     = INPUT_TOGGLE,
	       .off_flags   = O_AUTOSKIP,
	       .field_back  = A_NORMAL,
	       .fore_color  = PAIR_WHITE,
	       .height    = 1,
	       .width     = 3,
	       .toprow    = 2,
	       .leftcol   = 0,
	       .offscreen = 0,
	       .nbuffers  = 0,
	     },
	     { .field_label = "End on verb handler exit.",
	       .field_id    = LOG_ENABLE_VERB_LABEL,
	       .in_type     = INPUT_NONE,
	       .off_flags   = O_ACTIVE,
	       .field_back  = A_NORMAL,
	       .fore_color  = PAIR_RED,
	       .height    = 1,
	       .width     = 25,
	       .toprow    = 2,
	       .leftcol   = 4,
	       .offscreen = 0,
	       .nbuffers  = 0,
	     },
	     { .field_label = "[OK]",
	       .field_id    = OK_BUTTON,
	       .in_type     = INPUT_NONE,
	       .off_flags   = O_AUTOSKIP,
	       .field_back  = A_NORMAL,
	       .fore_color  = PAIR_RED,
	       .height    = 1,
	       .width     = 4,
	       .toprow    = 4,
	       .leftcol   = 12,
	       .offscreen = 0,
	       .nbuffers  = 0,
	     },
	  },
	},
};

/*
enum {
	PC_IN_PANEL,
	VERB_IN_PANEL,
	SAVE_STATE_PANEL,
	END_PANELS,
};
#define NUM_OF_PANELS END_PANELS
*/
const struct sfr_id_info sfr_disp[] = {
	{ .addr   = P0,
	  .str_id = 0,
	},
	{ .addr   = SP,
	  .str_id = 1,
	},
	{ .addr   = DPL_0,
	  .str_id = 2,
	},
	{ .addr   = DPH_0,
	  .str_id = 3,
	},
	{ .addr   = DPL_1,
	  .str_id = 4,
	},
	{ .addr   = DPH_1,
	  .str_id = 5,
	},
	{ .addr   = DP_TGL,
	  .str_id = 6,
	},
	{ .addr   = PCON,
	  .str_id = 7,
	},
	{ .addr   = TCON,
	  .str_id = 8,
	},
	{ .addr   = TMOD,
	  .str_id = 9,
	},
	{ .addr   = TC0L,
	  .str_id = 10,
	},
	{ .addr   = TC0H,
	  .str_id = 11,
	},
	{ .addr   = TC1L,
	  .str_id = 12,
	},
	{ .addr   = TC1H,
	  .str_id = 13,
	},
	{ .addr   = P1,
	  .str_id = 14,
	},
	{ .addr   = SCON,
	  .str_id = 15,
	},
	{ .addr   = SBUF,
	  .str_id = 16,
	},
	{ .addr   = P2,
	  .str_id = 17,
	},
	{ .addr   = P3,
	  .str_id = 18,
	},
	{ .addr   = HICA0,
	  .str_id = 19,
	},
	{ .addr   = HICA1,
	  .str_id = 20,
	},
	{ .addr   = HICA2,
	  .str_id = 21,
	},
	{ .addr   = HICD0,
	  .str_id = 22,
	},
	{ .addr   = HICD1,
	  .str_id = 23,
	},
	{ .addr   = HICD2,
	  .str_id = 24,
	},
	{ .addr   = HICD3,
	  .str_id = 25,
	},
	{ .addr   = IP,
	  .str_id = 26,
	},
	{ .addr   = PSW,
	  .str_id = 27,
	},
	{ .addr   = ACC,
	  .str_id = 28,
	},
	{ .addr   = SFR_B,
	  .str_id = 29,
	},
	{ .addr   = BANK_SW,
	  .str_id = 30,
	},
	{ .addr   = VRB_LA,
	  .str_id = 31,
	},
	{ .addr   = VRB_RE,
	  .str_id = 32,
	},
	{ .addr   = VRB_IN,
	  .str_id = 33,
	},
};
