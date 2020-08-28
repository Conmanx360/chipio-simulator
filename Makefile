CC      := gcc
CCFLAGS := -g -Wall -Wextra -O2
LDFLAGS := -l ncurses -l panel -l menu -l form

DEPS   := emu8051_defs.h emu8051_display.h
OBJ    := emu8051_dev.o emu8051_display.o emu8051_input.o \
	  emu8051_op_handlers.o emu8051_op_info.o \
	  emu8051_utils.o emu8051_main.o
%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CCFLAGS) $(LDFLAGS)

emu8051: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

clean:
	rm -f emu8051 $(OBJ)
