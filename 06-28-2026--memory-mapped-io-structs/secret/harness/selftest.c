/*
 * QTimer-16 self-test  —  GRADING HARNESS, not learner-visible.
 *
 * Exercises the driver against the silicon model and asserts observable
 * behaviour. Exit code 0 + "ALL TESTS PASSED" means the rubric is satisfied.
 */
#include "qtimer.h"
#include <stdint.h>
#include <stdio.h>

void qtimer_model_reset(void);
void qtimer_model_step(uint32_t cycles);

static int g_failures = 0;

#define CHECK(cond, msg) do {                                  \
        if (!(cond)) { printf("ASSERT FAIL: %s\n", (msg));     \
                       g_failures++; }                         \
    } while (0)

static uint32_t raw_ctrl(void)
{
    return ((volatile uint32_t*)QTIMER)[0];
}

/* ---- Check 1: control fields land on the datasheet bit positions ---- */
static void check_ctrl_layout(void)
{
    qtimer_model_reset();
    qtimer_config(QT_MODE_PERIODIC, /*prescale=*/5, /*reload=*/0xABCD);

    uint32_t c = raw_ctrl();
    CHECK(((c >> 0) & 0x1u)    == 1u,      "test_ctrl_layout: EN not at bit 0");
    CHECK(((c >> 1) & 0x1u)    == 0u,      "test_ctrl_layout: START not at bit 1");
    CHECK(((c >> 2) & 0x3u)    == 1u,      "test_ctrl_layout: MODE not at bits [3:2]");
    CHECK(((c >> 4) & 0xFu)    == 5u,      "test_ctrl_layout: PRESCALE not at bits [7:4]");
    CHECK(((c >> 8) & 0xFFFFu) == 0xABCDu, "test_ctrl_layout: RELOAD not at bits [23:8]");
    CHECK(((c >> 24) & 0xFFu)  == 0u,      "test_ctrl_layout: reserved bits [31:24] not zero");
}

/* ---- Check 2: periodic wraps at the configured RELOAD ---- */
static void check_periodic_reload(void)
{
    qtimer_model_reset();
    qtimer_config(QT_MODE_PERIODIC, /*prescale=*/0, /*reload=*/99); /* period = 100 cycles */
    qtimer_start();
    qtimer_model_step(1000);

    CHECK(qtimer_overflow_count() == 10,
          "test_periodic_reload: expected 10 wraps over 1000 cycles");
    CHECK(qtimer_count() == 0,
          "test_periodic_reload: counter should land on 0 after a whole number of periods");
}

/* ---- Check 3: prescaler and reload combine correctly ---- */
static void check_prescaled_reload(void)
{
    qtimer_model_reset();
    qtimer_config(QT_MODE_PERIODIC, /*prescale=*/3, /*reload=*/24); /* period = (24+1)*(3+1)=100 */
    qtimer_start();
    qtimer_model_step(1000);

    CHECK(qtimer_overflow_count() == 10,
          "test_prescaled_reload: expected 10 wraps over 1000 cycles");
}

int main(void)
{
    check_ctrl_layout();
    check_periodic_reload();
    check_prescaled_reload();

    if (g_failures == 0) {
        printf("ALL TESTS PASSED\n");
        return 0;
    }
    printf("%d CHECK(S) FAILED\n", g_failures);
    return 1;
}
