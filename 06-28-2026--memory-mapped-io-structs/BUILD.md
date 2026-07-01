# Build & Run — QTimer-16

Host simulation. No physical board, no cross-toolchain — just `gcc`. The
peripheral is modeled in software and linked into the self-test.

## 0. Prereqs

```sh
gcc --version      # any reasonably recent gcc/MinGW; this project uses C11
```

This machine: MSYS2 `gcc` (UCRT64) works. `mingw32-make` is optional.

## 1. Build the self-test

Run from the project directory
(`06-28-2026--memory-mapped-io-structs/`):

```sh
gcc -std=c11 -Wall -Wextra -O2 -Iinclude \
    src/qtimer.c \
    secret/harness/qtimer_model.c \
    secret/harness/selftest.c \
    -o build/selftest.exe
```

> You pass `secret/...` paths on the build line, but you are **not** to open or
> read those files — that's the grading harness. Just build and run.

## 2. Run

```sh
./build/selftest.exe
```

## 3. Interpreting the output

- **PASS:** the final line is

  ```
  ALL TESTS PASSED
  ```

  and the exit code is `0` (`echo $?`).

- **STILL BROKEN:** one or more lines like

  ```
  ASSERT FAIL: test_ctrl_layout: RELOAD not at bits [23:8]
  ASSERT FAIL: test_periodic_reload: expected 10 wraps over 1000 cycles
  ```

  followed by `N CHECK(S) FAILED` and a non-zero exit code.

## 4. Optional: Makefile shortcut

If you have `make` / `mingw32-make`:

```sh
mingw32-make run      # builds build/selftest.exe and runs it
mingw32-make clean
```

## 5. Debugging tip (not required)

To see where each CTRL field actually lands, program a known value and print the
raw register word, e.g. temporarily in your own scratch `main`:

```c
qtimer_config(QT_MODE_PERIODIC, 5, 0xABCD);
printf("CTRL = 0x%08X\n", ((volatile uint32_t*)QTIMER)[0]);
```

Compare each nibble against the datasheet bit table in `include/qtimer.h`.
