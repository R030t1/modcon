CC=gcc
CF=-std=c11 -Wall -Werror -pedantic
LF=-lmodbus

SRC=$(wildcard *.c)
OBJ=${SRC:.c=.o}
BIN=modcon

.phony: clean

${BIN}: ${OBJ}
	${CC} ${LF} -o $@ $^

%.o: %.c
	${CC} ${CF} -c -o $@ $<

clean:
	rm ${OBJ} ${BIN}
