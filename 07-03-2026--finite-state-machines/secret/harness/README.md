# Harness note

This topic runs entirely on the host (per the topic catalog: `host exe`, no
embedded simulator required). The harness *is* `tests/test_fsm.c`, built and
run via `make test` (see `Makefile` at the project root).

Grading procedure:

1. Compile+run `make test` against the learner's `src/fsm.c` — expect
   `test_obstacle_auto_recovery` to be the failing check on unmodified code
   (proves the bug reproduces).
2. Compile+run the same test file against `secret/reference/fsm.c` instead
   of `src/fsm.c` — expect `ALL TESTS PASSED` (proves the rubric is
   satisfiable). Example:

       gcc -std=c11 -Wall -Wextra -Werror -O2 -Isecret/reference \
           -o /tmp/test_fsm_ref tests/test_fsm.c secret/reference/fsm.c
       /tmp/test_fsm_ref

No Renode/QEMU/valgrind/etc. needed for this topic.
