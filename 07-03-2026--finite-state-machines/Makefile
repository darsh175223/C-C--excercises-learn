CC ?= gcc
CFLAGS ?= -std=c11 -Wall -Wextra -Werror -O2

SRC = src/fsm.c
TEST_SRC = tests/test_fsm.c

.PHONY: all run test clean

all: run

run: build/demo
	./build/demo

test: build/test_fsm
	./build/test_fsm

build/demo: src/main.c $(SRC) src/fsm.h | build
	$(CC) $(CFLAGS) -Isrc -o $@ src/main.c $(SRC)

build/test_fsm: $(TEST_SRC) $(SRC) src/fsm.h | build
	$(CC) $(CFLAGS) -Isrc -o $@ $(TEST_SRC) $(SRC)

build:
	mkdir -p build

clean:
	rm -rf build
