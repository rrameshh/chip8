CC = gcc
CFLAGS = -std=c17 -Wall -Wextra -Werror
LIBS = -lSDL2

all: chip8

chip8: chip8.c
	$(CC) $(CFLAGS) chip8.c -o chip8 $(LIBS)

debug: CFLAGS += -DDEBUG
debug: chip8

clean:
	rm -f chip8
