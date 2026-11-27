CC = gcc
CFLAGS = -Wall -Wextra -std=c99

SRC = src/scanner.c src/scanner_main.c
OUT = scanner

all: $(OUT)

$(OUT): $(SRC) src/token.h
	$(CC) $(CFLAGS) -o $(OUT) $(SRC)

run: $(OUT)
	./$(OUT)

clean:
	rm -f $(OUT)