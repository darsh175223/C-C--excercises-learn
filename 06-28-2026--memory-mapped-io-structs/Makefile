CC      ?= gcc
CFLAGS  := -std=c11 -Wall -Wextra -O2 -Iinclude
SRC     := src/qtimer.c
HARNESS := secret/harness/qtimer_model.c secret/harness/selftest.c
BIN     := build/selftest.exe

$(BIN): $(SRC) $(HARNESS)
	@mkdir -p build
	$(CC) $(CFLAGS) $(SRC) $(HARNESS) -o $(BIN)

run: $(BIN)
	./$(BIN)

test: run

clean:
	rm -rf build

.PHONY: run test clean
