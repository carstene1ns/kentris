
CC = gcc
CFLAGS = -Wall -std=c99
LIBS = -lm

OBJ = kentris.o music.o sound.o gfx.o

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

kentris.elf: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f *.o *.elf
