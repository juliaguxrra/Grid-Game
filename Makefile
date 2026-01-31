.PHONY: clean
CC=gcc
CFLAGS=-g -Wall -lncurses

all: game

game: game.c
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f game 