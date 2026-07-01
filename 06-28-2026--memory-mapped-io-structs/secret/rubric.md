# Rubric — memory-mapped-io-structs (QTimer-16)

## Allowed changes
- Learner may edit only `include/qtimer.h` and `src/qtimer.c`.
- Any modification under `secret/`, to `tests/`, or to the build line = automatic
  fail with: "you modified files outside `include/`+`src/` — revert and re-grade".

## Build command (run from project dir)
```sh
gcc -std=c11 -Wall -Wextra -O2 -Iinclude \
    src/qtimer.c secret/harness/qtimer_model.c secret/harness/selftest.c \
    -o build/selftest.exe
```
- Must compile with **no errors and no warnings** under `-Wall -Wextra`.
- The two `_Static_assert`s in `qtimer.h` must still hold (sizeof CTRL == 4,
  sizeof QTimer_Regs == 12). A "fix" that changes the register block size is wrong.

## Run command
```sh
./build/selftest.exe ; echo "exit=$?"
```

## Pass criteria
- stdout contains exactly `ALL TESTS PASSED`.
- exit code is `0`.
- No `ASSERT FAIL:` line appears.

## Individual checks (all must pass)
| Check                    | What it proves                                   |
|--------------------------|--------------------------------------------------|
| `test_ctrl_layout`       | every CTRL field sits on its datasheet bits, esp. RELOAD at [23:8] and reserved [31:24]==0 |
| `test_periodic_reload`   | period = RELOAD+1; 10 wraps over 1000 cycles, COUNT lands on 0 |
| `test_prescaled_reload`  | prescaler and RELOAD combine: (24+1)*(3+1)=100 → 10 wraps |

## Partial-credit checkpoints (for strike 1/2 feedback only — do not reveal cause)
- **Strike 1:** report which checks failed + the observable symptom only, e.g.
  "`test_ctrl_layout` fails: a control field is not on its documented bits;
  `test_periodic_reload` fails: wrong number of wraps. The prescaler itself is
  fine — the *period* the hardware uses is wrong."
- **Strike 2:** narrow hint — "This is a struct **layout / field-placement**
  problem, not a logic error in `qtimer.c` and not an off-by-one in the period
  math. The driver writes the right value; it lands on the wrong bits. Think
  about what determines where a wide bitfield is allowed to start."
- **Strike 3:** reveal `solution.md`, create `src.solved/`, prove it passes.

## Common wrong "fixes" to watch for
- Splitting RELOAD into LO/HI but keeping `uint16_t` storage AND reordering so
  the struct grows to 6 bytes → `_Static_assert` / offset checks fail.
- Hand-masking RELOAD in `qtimer.c` to compensate for the bad struct → may pass
  behaviour checks but `test_ctrl_layout`'s reserved-bits/RELOAD-position asserts
  still fail because the raw bits are still wrong. This is not a valid fix.
- Changing only some bitfield members' type and not RELOAD's storage unit.
