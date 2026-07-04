# Grading Rubric — finite-state-machines (garage door FSM)

## Test command

Run from the project root:

    make test

This builds `tests/test_fsm.c` against `src/fsm.c` and runs the resulting
binary.

## Pass criteria

- Process exit code is `0`.
- stdout contains the exact line `ALL TESTS PASSED`.
- No line in stdout matches `FAIL:`.

If either condition is violated, the attempt fails.

## Also check

- Sanity build: `make run` must also compile and execute without error
  (confirms the learner didn't break compilation while chasing the bug).
- Forbidden-change check: diff the learner's `tests/test_fsm.c`, `src/main.c`,
  and `Makefile` against the versions generated for this project. Any
  difference is an automatic fail with the message: "you modified
  `tests/test_fsm.c` [or `src/main.c` / `Makefile`] which is forbidden —
  revert." The learner may only change `src/fsm.c` (and `src/fsm.h` if truly
  needed — the intended fix does not require touching it).

## Checkpoints (for strike 1/2 feedback only — never reveal test-to-bug mapping directly, only report which named test failed and its printed symptom)

- `test_basic_open_close` — normal open/close cycle. Failing here means the
  learner broke the transition table broadly, not just the target bug.
- `test_reverse_while_opening` — button-triggered mid-travel reversal.
- `test_manual_override_from_obstruction` — manual recovery from
  `OBSTRUCTED` via the button. This test **passes on the original, unmodified
  buggy code** — if it starts failing after a learner's edit, their change
  regressed something that already worked.
- `test_obstacle_auto_recovery` — **this is the target bug.** Fails on
  unmodified `src/`. Must flip to PASS for the exercise to be considered
  solved.
- `test_stray_events_ignored` — unexpected event in a state with no matching
  transition must be silently ignored, not crash or move the FSM.

## Strike-1 wording guidance

Report only the failing test name(s) and the printed assertion message
(the "expected X, got Y" line), e.g.:

> `test_obstacle_auto_recovery` failed: expected OPENING, got OBSTRUCTED
> after an OBSTACLE_CLEAR event. Keep digging.

Do not say "table", "row", "transition list", or name `fsm.c` by
line/function beyond what the test output already reveals.

## Strike-2 wording guidance

One narrow hint, still no code/location: "the FSM has two different ways to
recover from `OBSTRUCTED` — one of them isn't wired up."
