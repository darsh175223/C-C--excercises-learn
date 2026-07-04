#include <stdio.h>
#include "fsm.h"

static void fire(door_fsm_t *fsm, door_event_t ev)
{
    door_state_t before = fsm->state;
    door_state_t after = door_fsm_step(fsm, ev);
    printf("  %-16s + %-16s -> %s\n", door_state_name(before), door_event_name(ev), door_state_name(after));
}

int main(void)
{
    door_fsm_t fsm;
    door_fsm_init(&fsm);

    printf("Garage door FSM demo\n");
    printf("Initial state: %s\n\n", door_state_name(fsm.state));

    fire(&fsm, EV_BUTTON);
    fire(&fsm, EV_LIMIT_OPEN);
    fire(&fsm, EV_BUTTON);
    fire(&fsm, EV_OBSTACLE);
    fire(&fsm, EV_OBSTACLE_CLEAR);
    fire(&fsm, EV_LIMIT_CLOSED);

    printf("\nFinal state: %s\n", door_state_name(fsm.state));
    return 0;
}
