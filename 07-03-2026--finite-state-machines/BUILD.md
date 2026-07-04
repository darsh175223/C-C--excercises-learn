# Build & Run — finite-state-machines

## Prereqs

Any C11 compiler works. Check you have one:

    gcc --version
    # or: clang --version

If you only have `clang`, either `make CC=clang test` or edit nothing —
most systems alias `gcc` to `clang` already.

On this Windows/MSYS2 host, `make` is not on PATH by default — use
`mingw32-make` instead (it reads the same `Makefile`, e.g.
`mingw32-make test`). Everywhere below that says `make`, substitute
`mingw32-make` if plain `make` isn't found.

## Run the demo

    make run

This builds `src/main.c` + `src/fsm.c` and fires a scripted sequence of
events at the FSM, printing each `state + event -> new_state` transition
plus the final state. Watch the trace after the `OBSTACLE` /
`OBSTACLE_CLEAR` pair closely — does the final behavior match the safety
spec in `PROBLEM.md`?

## Run the tests (this is what grading uses)

    make test

This builds `tests/test_fsm.c` + `src/fsm.c` and runs the resulting binary.

### How to interpret the output

- Each test prints its name, then either nothing else (pass) or one or more
  `FAIL: <message> -- expected X, got Y` lines.
- At the end:
  - `ALL TESTS PASSED` + exit code `0` → everything is correct.
  - `N TEST(S) FAILED` + non-zero exit code → still broken. Read the `FAIL:`
    line(s) — they tell you the expected vs. actual state after a specific
    event sequence, which is your best clue.

## Clean rebuild

    make clean
    make test

## Optional: step through with gdb

    gcc -std=c11 -g -Isrc -o build/test_fsm_dbg tests/test_fsm.c src/fsm.c
    gdb ./build/test_fsm_dbg
    (gdb) break door_fsm_step
    (gdb) run
