# Solution — finite-state-machines (garage door FSM)

## Root cause

`src/fsm.c`'s `TRANSITIONS[]` table is missing one row:

    { DOOR_OBSTRUCTED, EV_OBSTACLE_CLEAR, DOOR_OPENING },

Every other transition needed for normal operation — including the
*manual* recovery path (`DOOR_OBSTRUCTED + EV_BUTTON -> DOOR_OPENING`) — is
present. That's why the door "mostly works": a human can always get it
moving again by pressing the button. But the safety requirement stated in
`PROBLEM.md` (the door must automatically resume opening the instant the
obstruction clears, with no button press required) silently fails.
`door_fsm_step()` scans the table, finds no row matching
`(DOOR_OBSTRUCTED, EV_OBSTACLE_CLEAR)`, falls through to its "no match ->
ignore the event" branch, and the FSM sits in `DOOR_OBSTRUCTED` forever
until a human intervenes.

## Minimal fix

Add one row to `TRANSITIONS[]` in `src/fsm.c` (position doesn't matter,
conventionally placed next to the other `DOOR_OBSTRUCTED` row):

```c
static const transition_t TRANSITIONS[] = {
    { DOOR_CLOSED,      EV_BUTTON,          DOOR_OPENING    },
    { DOOR_OPENING,     EV_LIMIT_OPEN,      DOOR_OPEN       },
    { DOOR_OPENING,     EV_BUTTON,          DOOR_CLOSING    },
    { DOOR_OPEN,        EV_BUTTON,          DOOR_CLOSING    },
    { DOOR_CLOSING,     EV_LIMIT_CLOSED,    DOOR_CLOSED     },
    { DOOR_CLOSING,     EV_OBSTACLE,        DOOR_OBSTRUCTED },
    { DOOR_CLOSING,     EV_BUTTON,          DOOR_OPENING    },
    { DOOR_OBSTRUCTED,  EV_BUTTON,          DOOR_OPENING    },
    { DOOR_OBSTRUCTED,  EV_OBSTACLE_CLEAR,  DOOR_OPENING    },  /* <-- added */
};
```

No other file needs to change.

## Why it's subtle

- It's a missing *row* in a data table, not a missing `case` in a `switch`.
  `-Wall -Wextra -Werror` has nothing to warn about: the table is just data,
  and the lookup function's fallthrough ("no matching row => ignore the
  event") is valid, *intentional* behavior for genuinely stray events (see
  `test_stray_events_ignored`). The compiler cannot distinguish "this
  fallthrough is correct here" from "this fallthrough is hiding a missing
  transition."
- The FSM has a *working* recovery path (the manual button override) that
  masks the missing *automatic* one. A learner who only smoke-tests "does
  the door still work" will not notice anything wrong — you have to exercise
  the exact event sequence `OBSTACLE` then `OBSTACLE_CLEAR` with no button
  press in between to observe the stuck state.
- The bug never crashes, never trips an assert, never produces a
  compiler/lint warning — it just silently does nothing, which is the most
  common real-world shape of a missing-transition FSM bug in firmware.
