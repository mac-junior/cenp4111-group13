CC = gcc
CFLAGS = -Wall -Wextra -std=c99

SRC = src/main.c src/scanner.c src/parser.c src/ast.c

OUT = compiler

all: $(OUT)

$(OUT): $(SRC) src/token.h src/ast.h src/parser.h src/scanner.h
	$(CC) $(CFLAGS) -o $(OUT) $(SRC)

run: $(OUT)
	./$(OUT) tests/valid1.rpn

clean:
	rm -f $(OUT) scanner

.PHONY: all run clean