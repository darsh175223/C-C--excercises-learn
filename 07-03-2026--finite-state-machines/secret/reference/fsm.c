#include "fsm.h"
#include <stddef.h>

typedef struct {
    door_state_t from;
    door_event_t event;
    door_state_t to;
} transition_t;

static const transition_t TRANSITIONS[] = {
    { DOOR_CLOSED,      EV_BUTTON,          DOOR_OPENING    },
    { DOOR_OPENING,     EV_LIMIT_OPEN,      DOOR_OPEN       },
    { DOOR_OPENING,     EV_BUTTON,          DOOR_CLOSING    },
    { DOOR_OPEN,        EV_BUTTON,          DOOR_CLOSING    },
    { DOOR_CLOSING,     EV_LIMIT_CLOSED,    DOOR_CLOSED     },
    { DOOR_CLOSING,     EV_OBSTACLE,        DOOR_OBSTRUCTED },
    { DOOR_CLOSING,     EV_BUTTON,          DOOR_OPENING    },
    { DOOR_OBSTRUCTED,  EV_BUTTON,          DOOR_OPENING    },
    { DOOR_OBSTRUCTED,  EV_OBSTACLE_CLEAR,  DOOR_OPENING    },
};

#define NUM_TRANSITIONS (sizeof(TRANSITIONS) / sizeof(TRANSITIONS[0]))

void door_fsm_init(door_fsm_t *fsm)
{
    fsm->state = DOOR_CLOSED;
}

door_state_t door_fsm_step(door_fsm_t *fsm, door_event_t event)
{
    for (size_t i = 0; i < NUM_TRANSITIONS; i++) {
        if (TRANSITIONS[i].from == fsm->state && TRANSITIONS[i].event == event) {
            fsm->state = TRANSITIONS[i].to;
            return fsm->state;
        }
    }
    /* No matching row: event does not apply in this state, ignore it. */
    return fsm->state;
}

const char *door_state_name(door_state_t state)
{
    switch (state) {
        case DOOR_CLOSED:     return "CLOSED";
        case DOOR_OPENING:    return "OPENING";
        case DOOR_OPEN:       return "OPEN";
        case DOOR_CLOSING:    return "CLOSING";
        case DOOR_OBSTRUCTED: return "OBSTRUCTED";
        default:              return "UNKNOWN";
    }
}

const char *door_event_name(door_event_t event)
{
    switch (event) {
        case EV_BUTTON:         return "BUTTON";
        case EV_OBSTACLE:       return "OBSTACLE";
        case EV_OBSTACLE_CLEAR: return "OBSTACLE_CLEAR";
        case EV_LIMIT_OPEN:     return "LIMIT_OPEN";
        case EV_LIMIT_CLOSED:   return "LIMIT_CLOSED";
        default:                return "UNKNOWN";
    }
}
