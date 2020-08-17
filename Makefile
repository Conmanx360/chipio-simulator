src = $(wildcard *.c)
obj = $(src:.c=.o)

CFLAGS=-g -Wall -Wextra 
LDFLAGS = -l ncurses -l panel -l menu -l form

emu8051: $(obj)
	$(CC) -Wall -g -o $@ $^ $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(obj) emu8051
