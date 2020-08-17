#include <ncurses.h>
#include <panel.h>
#include <form.h>
#include <menu.h>

/*
 * 8051 emu header file.
 */

/*
 * 8051 Assembly opcode macros.
 */
#define NOP		0x00
#define AJMP_01		0x01
#define LJMP		0x02
#define RR		0x03
#define INC_START	0x04
#define INC_END		0x0f
#define JBC		0x10
#define ACALL_11	0x11
#define LCALL		0x12
#define RRC		0x13
#define DEC_START	0x14
#define DEC_END		0x1f
#define JB		0x20
#define AJMP_21		0x21
#define RET		0x22
#define RL		0x23
#define ADD_START	0x24
#define ADD_END		0x2f
#define JNB		0x30
#define ACALL_31	0x31
#define RETI		0x32
#define RLC		0x33
#define ADDC_START	0x34
#define ADDC_END	0x3f
#define JC		0x40
#define AJMP_41		0x41
#define ORL_START	0x42
#define ORL_END		0x4f
#define JNC		0x50
#define ACALL_51	0x51
#define ANL_START	0x52
#define ANL_END		0x5f
#define JZ		0x60
#define AJMP_61		0x61
#define XRL_START	0x62
#define XRL_END		0x6f
#define JNZ		0x70
#define ACALL_71	0x71
#define ORL_72		0x72
#define JMP		0x73
#define MOV_7_START	0x74
#define MOV_7_END	0x7f
#define SJMP		0x80
#define AJMP_81		0x81
#define ANL_82		0x82
#define MOVC_83		0x83
#define DIV		0x84
#define MOV_8_START	0x85
#define MOV_8_END	0x90
#define ACALL_91	0x91
#define MOV_92		0x92
#define MOVC_93		0x93
#define SUBB_START	0x94
#define SUBB_END	0x9f
#define ORL_A0		0xa0
#define AJMP_A1		0xa1
#define MOV_A2		0xa2
#define INC_A3		0xa3
#define MUL		0xa4
#define RESERVED	0xa5
#define MOV_A_START	0xa6
#define MOV_A_END	0xaf
#define ANL_B0		0xb0
#define ACALL_B1	0xb1
#define CPL_B2		0xb2
#define CPL_B3		0xb3
#define CJNE_START	0xb4
#define CJNE_END	0xbf
#define PUSH		0xc0
#define AJMP_C1		0xc1
#define CLR_C2		0xc2
#define CLR_C3		0xc3
#define SWAP		0xc4
#define XCH_START	0xc5
#define XCH_END		0xcf
#define POP		0xd0
#define ACALL_D1	0xd1
#define SETB_D2		0xd2
#define SETB_D3		0xd3
#define DA		0xd4
#define DJNZ_D5		0xd5
#define XCHD_D6		0xd6
#define XCHD_D7		0xd7
#define DJNZ_START	0xd8
#define DJNZ_END	0xdf
#define MOVX_E0		0xe0
#define AJMP_E1		0xe1
#define MOVX_E2		0xe2
#define MOVX_E3		0xe3
#define CLR_E4		0xe4
#define MOV_E_START	0xe5
#define MOV_E_END	0xef
#define MOVX_F0		0xf0
#define ACALL_F1	0xf1
#define MOVX_F2		0xf2
#define MOVX_F3		0xf3
#define CPL_F4		0xf4
#define MOV_F_START	0xf5
#define MOV_F_END	0xff

/*
 * Program Status Word Flags
 */
#define PARITY_FLAG 0x01
#define USER1_FLAG  0x02 // User definable flag.
#define OVFLOW_FLAG 0x04 // Overflow flag.
#define RBSEL0_FLAG 0x08 // Register Bank Select 0
#define RBSEL1_FLAG 0x10 // Register Bank Select 1
#define USER0_FLAG  0x20 // Flag 0, second user definable flag
#define AUX_CY_FLAG 0x40 // Aux carry flag for overflow on bit 4.
#define CARRY_FLAG  0x80 // Regular carry flag.

/* Special Function Registers (offset from 0x80) */
#define P0	0x00 // Port 0
#define SP      0x01 // Stack Pointer
#define DPL_0	0x02 // Data pointer Low  0
#define DPH_0	0x03 // Data pointer High 0
#define DPL_1	0x04 // Data pointer Low  1
#define DPH_1	0x05 // Data pointer High 1
#define DP_TGL	0x06 // Data pointer toggle. Used to switch between 0/1.
#define PCON	0x07 // Power control
#define TCON	0x08 // Timer/counter control
#define TMOD	0x09 // Timer/counter mode control
#define TC0L	0x0a // Timer/counter 0 Low Byte
#define TC0H	0x0c // Timer/counter 0 High Byte
#define TC1L	0x0b // Timer/counter 1 Low Byte
#define TC1H	0x0d // Timer/counter 1 High Byte
#define P1	0x10 // Port 1
#define	SCON	0x18 // Serial Control
#define	SBUF	0x19 // Serial Buffer
#define	P2	0x20 // Port 2
#define P3	0x30 // Port 3
#define HICA0	0x31 // Hardware interconnect address 0-7
#define HICA1	0x32 // hardware interconnect address 8-15
#define HICA2	0x33 // Hardware interconnect address 16-23
#define HICD0	0x34 // Hardware interconnect r/w 0-7
#define HICD1	0x35 // hardware interconnect r/w 8-15
#define HICD2	0x36 // Hardware interconnect r/w 16-23
#define HICD3	0x37 // Hardware interconnect r/w 24-31
#define IP	0x38 // Interrupt Priority
#define PSW     0x50 // Program Status Word
#define VRB_LA  0x5c // Last byte of verb incoming/response.
#define VRB_RE  0x5e // Command verb response stack. 
#define VRB_IN  0x5f // Command verb incoming stack. 
#define ACC     0x60 // Accumulator
#define SFR_B	0x70 // B sfr, used in mul/div operations.
#define BANK_SW 0x7a // Bank switching sfr.

struct emu8051_dev {
	uint16_t pc;	
	
	uint8_t pmem[0x8000];
	uint8_t pmem_b0[0x6000];
	uint8_t pmem_b1[0x6000];

	uint8_t xram[0x10000];
	uint8_t iram[0x100];
	uint8_t sfr[0x80];

	uint8_t verb_pos;
	uint8_t verb_sfr[3];

	struct op_change *op_ch;
};


#define NUM_OF_WINDOWS 3
#define NUM_OF_POPUPS 5

enum {
	DISASSEMBLY_WIN,
	IRAM_WIN,
	SFR_WIN,
};

struct sfr_id_info {
	uint8_t addr;
	uint8_t str_id;
};

struct emu_win_info {
	uint16_t height;
	uint16_t width;
	uint16_t startx;
	uint16_t starty;
};

struct popup_field_info {
	char *field_label;
	uint8_t field_id;
	uint8_t in_type;
	uint8_t button_id;

	int off_flags;
	uint32_t field_back;
	uint8_t fore_color;

	uint8_t height;
	uint8_t width;
	uint8_t toprow;
	uint8_t leftcol;
	uint8_t offscreen;
	uint8_t nbuffers;
};

struct popup_win_info {
	char *title;
	uint8_t height;
	uint8_t width;
	uint8_t default_in;
	uint8_t num_of_inputs;

	uint8_t num_of_fields;
	struct popup_field_info field_info[20];
};

struct popup_win_data {
	WINDOW *win, *subwin;
	PANEL *panel;
	FORM *form;

	uint8_t num_of_fields;
	FIELD **field;

	uint8_t rows;
	uint8_t cols;

	uint8_t height;
	uint8_t width;

	uint16_t cur_offset, cur_pos;

	uint8_t default_in;
	uint8_t num_of_inputs;
};

struct field_data {
	uint8_t field_id;
	uint8_t in_type;

	uint8_t toggle_val;
	uint8_t toggle_id;
};

struct emu_win_data {
	WINDOW *win;
	WINDOW *win_ch[4];
	PANEL *panel[4];

	uint16_t height;
	uint16_t width;
	uint16_t x;
	uint16_t y;

	uint8_t cur_pos;
};


#define ARR_BLOCK_SIZE 100
//#define BACK_OPS 100
#define BACK_OPS 1000

/*
 * Breakpoint flags, set in the settings popup window.
 * They are as follows:
 * 0 - PC breakpoint
 * 1 - DPTR breakpoint
 * 2 - Custom instruction breakpoint
 */
#define FLAG_BRK_PC   0x01
#define FLAG_BRK_DPTR 0x02
#define FLAG_BRK_INST 0x04

struct breakpoint_data {
	uint8_t break_flags;

	uint16_t pc_break;
	uint16_t dptr_break;
};

/*
 * Need to have system of flags for change type, upper 8 bits should probably
 * be something like this:
 * 0 - direct write
 * 1 - indirect write
 * 2 - register?
 * 3 - exram -- ignores lower 8 bits, just takes dptr as address.
 */
#define D_WRITE 0x0100 // Direct write
#define I_WRITE 0x0200 // Indirect write (IRAM)
#define R_WRITE 0x0400 // Register write
#define E_WRITE 0x0800 // Exram write
#define S_WRITE 0x1000 // SFR Write
#define ER_WRITE 0x2000 // Exram write, with register as addr.

struct op_change {
	uint16_t pc;

	uint8_t changes;
	uint16_t change_type[6];
	uint16_t change_val[6];
};

struct arr_block {
        struct op_change op[ARR_BLOCK_SIZE];
};

struct dynamic_array {
        uint32_t count;
        uint32_t size;
	uint32_t start_offset;

        struct arr_block **block;
};

struct opcode_info {
	uint8_t op_id;
	uint8_t dst_id;
	uint8_t src_id;

	uint8_t args;
	uint8_t length;
	void (*handler)(struct emu8051_dev *emu_dev);
};

struct emu8051_data {
	struct emu8051_dev *emu_dev;
	struct emu_win_data win_data[NUM_OF_WINDOWS];
	struct popup_win_data popup_data[NUM_OF_POPUPS];
	struct dynamic_array dyn_array;

	uint8_t cur_window;

	uint32_t number;

	uint32_t max_backops;
	uint32_t step_size;
	uint8_t breakpoint_flags;

	struct breakpoint_data break_data;
};

struct win_dimensions {
	uint16_t height;
	uint16_t width;
	uint16_t y;
	uint16_t x;
};

/* Enumerations */

enum file_ids {
	FILE_ROM0,
	FILE_ROM1,
	FILE_ROM2,
	FILE_XRAM,
	FILE_IRAM,
};

enum color_pair {
	PAIR_RED = 1,
	PAIR_GREEN,
	PAIR_CYAN,
	PAIR_WHITE,
	PAIR_BLUE
};

enum asm_arg_ids {
        A_ID, 
	ADDR11_ID, 
	ADDR16_ID, 
	DIRECT_ID, 
	I_R0_ID, 
	I_R1_ID, 
	R0_ID, 
	R1_ID, 
	R2_ID,
        R3_ID, 
	R4_ID, 
	R5_ID, 
	R6_ID, 
	R7_ID, 
	BIT_ID, 
	IBIT_ID, 
	OFFSET_ID, 
	IMMED_ID, 
	C_ID, 
	A_DPTR_ID,
        DPTR_ID, 
	IDPTR_ID, 
	AB_ID, 
	A_PC_ID,
	NO_SRC
};

enum asm_op_ids {
	ACALL_ID,
	ADD_ID,
	ADDC_ID,
	AJMP_ID,
	ANL_ID,
	CJNE_ID,
	CLR_ID,
	CPL_ID,
	DA_ID,
	DEC_ID,
	DIV_ID,
	DJNZ_ID,
	INC_ID,
	JB_ID,
	JBC_ID,
	JC_ID,
	JMP_ID,
	JNB_ID,
	JNC_ID,
	JNZ_ID,
	JZ_ID,
	LCALL_ID,
	LJMP_ID,
	MOV_ID,
	MOVC_ID,
	MOVX_ID,
	MUL_ID,
	NOP_ID,
	ORL_ID,
	POP_ID,
	PUSH_ID,
	RSRV_ID,
	RET_ID,
	RETI_ID,
	RL_ID,
	RLC_ID,
	RR_ID,
	RRC_ID,
	SETB_ID,
	SJMP_ID,
	SUBB_ID,
	SWAP_ID,
	XCH_ID,
	XCHD_ID,
	XRL_ID,
};

enum field_in_types {
	INPUT_HEX,
	INPUT_DEC,
	INPUT_ALPHANUM,
	INPUT_TOGGLE,
	INPUT_NONE,
};

enum pc_in_field_ids {
	PC_LABEL,
	PC_HEX_LABEL,
	PC_INPUT,
	PC_OK_BUTTON,
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

enum settings_field_ids {
	SETTINGS_STEP_SIZE_LABEL,
	SETTINGS_STEP_SIZE_IN,
	SETTINGS_BACKLOG_LABEL,
	SETTINGS_BACKLOG_IN,
	SETTINGS_BREAKPOINTS_LABEL,
	SETTINGS_PC_BRK_SWITCH,
	SETTINGS_PC_BRK_LABEL,
	SETTINGS_PC_BRK_HEX_LABEL,
	SETTINGS_PC_BRK_IN,
	SETTINGS_DPTR_BRK_SWITCH,
	SETTINGS_DPTR_BRK_LABEL,
	SETTINGS_DPTR_BRK_HEX_LABEL,
	SETTINGS_DPTR_BRK_IN,
	SETTINGS_OK_BUTTON,
};

enum button_ids {
	BUTTON_TOGGLE_PC,
	BUTTON_TOGGLE_DPTR,
};

enum {
	PC_IN_PANEL,
	VERB_IN_PANEL,
	SAVE_STATE_PANEL,
	XRAM_DISP_PANEL,
	SETTINGS_PANEL,
	END_PANELS,
};
#define NUM_OF_PANELS END_PANELS



#define OK_BUTTON 0x80

/* function declarations */
/* emu8051_input.c */
void emu8051_in_handler(struct emu8051_data *emu_data, int in_ch);

/* emu8051_display.c */
void disp_init(struct emu8051_data *emu_data);
void update_win(struct emu8051_data *emu_data);
void show_popup(PANEL *panel);
void hide_popup(PANEL *panel);
void ok_button_highlight(FIELD *ok_button, uint8_t cur_id);
void write_hex(WINDOW *win, uint16_t y_pos, uint16_t x_pos,
		      uint8_t data_len, uint32_t data);
void update_exram_win(struct emu8051_data *emu_data);
void toggle_button_highlight(FIELD *toggle_label, uint8_t highlight);
void toggle_button_set(FIELD *toggle, uint8_t toggle_val);

/* emu8051_main.c */

/* emu8051_opcodes.c */
void acall_handler(struct emu8051_dev *emu_dev);
void add_handler(struct emu8051_dev *emu_dev) ;
void addc_handler(struct emu8051_dev *emu_dev);
void ajmp_handler(struct emu8051_dev *emu_dev);
void anl_handler(struct emu8051_dev *emu_dev);
void cjne_handler(struct emu8051_dev *emu_dev);
void clr_handler(struct emu8051_dev *emu_dev);
void cpl_handler(struct emu8051_dev *emu_dev);
void da_handler(struct emu8051_dev *emu_dev);
void dec_handler(struct emu8051_dev *emu_dev);
void div_handler(struct emu8051_dev *emu_dev);
void djnz_handler(struct emu8051_dev *emu_dev);
void inc_handler(struct emu8051_dev *emu_dev);
void jb_handler(struct emu8051_dev *emu_dev);
void jbc_handler(struct emu8051_dev *emu_dev);
void jc_handler(struct emu8051_dev *emu_dev);
void jmp_handler(struct emu8051_dev *emu_dev);
void jnb_handler(struct emu8051_dev *emu_dev);
void jnc_handler(struct emu8051_dev *emu_dev);
void jnz_handler(struct emu8051_dev *emu_dev);
void jz_handler(struct emu8051_dev *emu_dev);
void lcall_handler(struct emu8051_dev *emu_dev); 
void ljmp_handler(struct emu8051_dev *emu_dev);
void mov_handler(struct emu8051_dev *emu_dev);
void movc_handler(struct emu8051_dev *emu_dev);
void movx_handler(struct emu8051_dev *emu_dev);
void mul_handler(struct emu8051_dev *emu_dev);
void orl_handler(struct emu8051_dev *emu_dev);
void pop_handler(struct emu8051_dev *emu_dev);
void push_handler(struct emu8051_dev *emu_dev);
void ret_handler(struct emu8051_dev *emu_dev);
void reti_handler(struct emu8051_dev *emu_dev);
void rl_handler(struct emu8051_dev *emu_dev);
void rlc_handler(struct emu8051_dev *emu_dev);
void rr_handler(struct emu8051_dev *emu_dev);
void rrc_handler(struct emu8051_dev *emu_dev);
void setb_handler(struct emu8051_dev *emu_dev);
void sjmp_handler(struct emu8051_dev *emu_dev);
void subb_handler(struct emu8051_dev *emu_dev);
void swap_handler(struct emu8051_dev *emu_dev);
void xch_handler(struct emu8051_dev *emu_dev);
void xchd_handler(struct emu8051_dev *emu_dev);
void xrl_handler(struct emu8051_dev *emu_dev);

/* emu8051_op_info.c */
const struct opcode_info *get_op_info(uint8_t op);

/* emu8051_dev.c */
uint8_t handle_opcode(struct emu8051_data *emu_data);
uint8_t get_pmem(struct emu8051_dev *emu_dev, uint16_t addr);
void indirect_write(struct emu8051_dev *emu_dev, uint8_t addr, uint8_t data);
void direct_write(struct emu8051_dev *emu_dev, uint8_t addr, uint8_t data);
void write_sfr(struct emu8051_dev *emu_dev, uint8_t sfr, uint8_t data);
uint8_t get_pmem_bank(struct emu8051_dev *emu_dev, uint16_t addr);
uint8_t get_psw_flag(struct emu8051_dev *emu_dev, uint8_t flag);
void set_psw_flag(struct emu8051_dev *emu_dev, uint8_t flag);
void clr_psw_flag(struct emu8051_dev *emu_dev, uint8_t flag);
void push(struct emu8051_dev *emu_dev, uint8_t data);
uint8_t pop(struct emu8051_dev *emu_dev);
uint8_t get_reg_bank(struct emu8051_dev *emu_dev);
void set_reg(struct emu8051_dev *emu_dev, uint8_t reg, uint8_t data);
uint8_t get_reg(struct emu8051_dev *emu_dev, uint8_t reg);
uint8_t direct_read(struct emu8051_dev *emu_dev, uint8_t addr);
uint8_t get_bit(struct emu8051_dev *emu_dev, uint8_t addr);
void set_bit(struct emu8051_dev *emu_dev, uint8_t addr, uint8_t set);
uint16_t get_dptr(struct emu8051_dev *emu_dev);
void set_dptr(struct emu8051_dev *emu_dev, uint16_t dptr);
void setup_verb_handler(struct emu8051_data *emu_data, uint8_t nid,
			uint16_t verb, uint8_t val);

/* emu8051_utils.c */
void save_state_to_file(struct emu8051_data *emu_data, char *file_name);
void restore_state_from_file(struct emu8051_data *emu_data, FILE *file_in);
void allocate_blocks(struct dynamic_array *dyn_array, uint32_t size);
struct op_change *pop_front(struct dynamic_array *dyn_array);
void write_front(struct dynamic_array *dyn_array, struct op_change op);
void op_change_write(struct emu8051_dev *emu_dev, uint16_t addr_type, uint8_t addr);
void reverse_op(struct emu8051_dev *emu_dev, struct op_change *op_ch);
struct op_change *get_front(struct dynamic_array *dyn_array);
void breakpoint_flag_set(struct emu8051_data *emu_data, uint8_t flag_id, uint8_t set);
uint8_t breakpoint_check(struct emu8051_data *emu_data);
void resize_dynarray(struct dynamic_array *dyn_array, uint32_t size);
void save_backlog_state_to_file(struct emu8051_data *emu_data, char *file_name);
