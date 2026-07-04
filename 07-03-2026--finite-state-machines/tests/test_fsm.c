#include <stdio.h>
#include <stdlib.h>
#include "../src/fsm.h"

static int g_failures = 0;

#define CHECK_STATE(fsm, expected, msg) \
    do { \
        if ((fsm).state != (expected)) { \
            printf("  FAIL: %s -- expected %s, got %s\n", msg, door_state_name(expected), door_state_name((fsm).state)); \
            g_failures++; \
            return; \
        } \
    } while (0)

static void test_basic_open_close(void)
{
    printf("test_basic_open_close...\n");
    door_fsm_t fsm;
    door_fsm_init(&fsm);
    door_fsm_step(&fsm, EV_BUTTON);
    CHECK_STATE(fsm, DOOR_OPENING, "button from CLOSED should start opening");
    door_fsm_step(&fsm, EV_LIMIT_OPEN);
    CHECK_STATE(fsm, DOOR_OPEN, "reaching the top limit switch should reach OPEN");
    door_fsm_step(&fsm, EV_BUTTON);
    CHECK_STATE(fsm, DOOR_CLOSING, "button from OPEN should start closing");
    door_fsm_step(&fsm, EV_LIMIT_CLOSED);
    CHECK_STATE(fsm, DOOR_CLOSED, "reaching the bottom limit switch should reach CLOSED");
    printf("  PASS\n");
}

static void test_reverse_while_opening(void)
{
    printf("test_reverse_while_opening...\n");
    door_fsm_t fsm;
    door_fsm_init(&fsm);
    door_fsm_step(&fsm, EV_BUTTON);
    CHECK_STATE(fsm, DOOR_OPENING, "button from CLOSED should start opening");
    door_fsm_step(&fsm, EV_BUTTON);
    CHECK_STATE(fsm, DOOR_CLOSING, "pressing the button again mid-open should reverse to closing");
    door_fsm_step(&fsm, EV_LIMIT_CLOSED);
    CHECK_STATE(fsm, DOOR_CLOSED, "reversing should still be able to reach CLOSED");
    printf("  PASS\n");
}

static void test_manual_override_from_obstruction(void)
{
    printf("test_manual_override_from_obstruction...\n");
    door_fsm_t fsm;
    door_fsm_init(&fsm);
    door_fsm_step(&fsm, EV_BUTTON);
    door_fsm_step(&fsm, EV_LIMIT_OPEN);
    door_fsm_step(&fsm, EV_BUTTON);
    door_fsm_step(&fsm, EV_OBSTACLE);
    CHECK_STATE(fsm, DOOR_OBSTRUCTED, "an obstacle during closing should stop the door");
    door_fsm_step(&fsm, EV_BUTTON);
    CHECK_STATE(fsm, DOOR_OPENING, "pressing the button while obstructed should manually resume opening");
    printf("  PASS\n");
}

static void test_obstacle_auto_recovery(void)
{
    printf("test_obstacle_auto_recovery...\n");
    door_fsm_t fsm;
    door_fsm_init(&fsm);
    door_fsm_step(&fsm, EV_BUTTON);
    door_fsm_step(&fsm, EV_LIMIT_OPEN);
    door_fsm_step(&fsm, EV_BUTTON);
    door_fsm_step(&fsm, EV_OBSTACLE);
    CHECK_STATE(fsm, DOOR_OBSTRUCTED, "an obstacle during closing should stop the door");
    door_fsm_step(&fsm, EV_OBSTACLE_CLEAR);
    CHECK_STATE(fsm, DOOR_OPENING, "door must automatically resume opening once the obstruction clears, with no button press needed");
    printf("  PASS\n");
}

static void test_stray_events_ignored(void)
{
    printf("test_stray_events_ignored...\n");
    door_fsm_t fsm;
    door_fsm_init(&fsm);
    door_fsm_step(&fsm, EV_LIMIT_OPEN);
    CHECK_STATE(fsm, DOOR_CLOSED, "an unexpected limit-switch event while CLOSED must be ignored, not crash or move the FSM");
    printf("  PASS\n");
}

int main(void)
{
    test_basic_open_close();
    test_reverse_while_opening();
    test_manual_override_from_obstruction();
    test_obstacle_auto_recovery();
    test_stray_events_ignored();

    if (g_failures == 0) {
        printf("\nALL TESTS PASSED\n");
        return 0;
    }

    printf("\n%d TEST(S) FAILED\n", g_failures);
    return 1;
}
