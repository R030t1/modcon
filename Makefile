CC=gcc
CF=-std=c11 -Wall -Werror -pedantic
LF=-L/usr/lib/x86_64-linux-gnu -L/usr/local/lib -lmodbus

SRC=$(wildcard *.c)
OBJ=${SRC:.c=.o}
BIN=modcon

.phony: clean

${BIN}: ${OBJ}
	${CC} -o $@ $^ ${LF}

%.o: %.c
	${CC} ${CF} -c -o $@ $<

clean:
	rm ${OBJ} ${BIN}
