#ifndef FSM_H
#define FSM_H

typedef enum {
    DOOR_CLOSED,
    DOOR_OPENING,
    DOOR_OPEN,
    DOOR_CLOSING,
    DOOR_OBSTRUCTED,
    DOOR_STATE_COUNT
} door_state_t;

typedef enum {
    EV_BUTTON,
    EV_OBSTACLE,
    EV_OBSTACLE_CLEAR,
    EV_LIMIT_OPEN,
    EV_LIMIT_CLOSED,
    EV_COUNT
} door_event_t;

typedef struct {
    door_state_t state;
} door_fsm_t;

void door_fsm_init(door_fsm_t *fsm);
door_state_t door_fsm_step(door_fsm_t *fsm, door_event_t event);
const char *door_state_name(door_state_t state);
const char *door_event_name(door_event_t event);

#endif
