/*
 * QTimer-16 "silicon" model  —  GRADING HARNESS, not learner-visible.
 *
 * This file IS the hardware. It owns the backing store for the memory-mapped
 * register block and decodes CTRL strictly from the datasheet bit positions,
 * completely independent of however the driver's C struct happens to lay the
 * bitfields out. If the driver's struct puts a field on the wrong bits, the
 * value this model reads back will be wrong and the timer will misbehave.
 */
#include "qtimer.h"
#include <stdint.h>

/* The register file ("silicon"). 3 x 32-bit registers: CTRL, COUNT, STATUS. */
static volatile uint32_t g_mmio[3];

/* What the driver writes through. */
volatile QTimer_Regs* const QTIMER = (volatile QTimer_Regs*)g_mmio;

static uint32_t s_prescale_acc;

static uint32_t bf(uint32_t v, int lo, int width)
{
    return (v >> lo) & ((width >= 32) ? 0xFFFFFFFFu : ((1u << width) - 1u));
}

void qtimer_model_reset(void)
{
    g_mmio[0] = 0;   /* CTRL   */
    g_mmio[1] = 0;   /* COUNT  */
    g_mmio[2] = 0;   /* STATUS */
    s_prescale_acc = 0;
}

/* Advance the simulated hardware by `cycles` input clock cycles. */
void qtimer_model_step(uint32_t cycles)
{
    uint32_t ctrl = g_mmio[0];

    uint32_t en       = bf(ctrl, 0, 1);
    uint32_t start    = bf(ctrl, 1, 1);
    uint32_t prescale = bf(ctrl, 4, 4);
    uint32_t reload   = bf(ctrl, 8, 16);

    uint32_t running = (en && start) ? 1u : 0u;
    uint32_t count = g_mmio[1] & 0xFFFFu;
    uint32_t ovf   = (g_mmio[2] >> 16) & 0xFFFFu;
    uint32_t match = (g_mmio[2] >> 1) & 0x1u;

    if (!running) {
        g_mmio[2] = (ovf << 16) | (match << 1) | 0u;
        return;
    }

    for (uint32_t i = 0; i < cycles; ++i) {
        if (s_prescale_acc >= prescale) {   /* a COUNT tick every (prescale+1) cycles */
            s_prescale_acc = 0;
            if (count >= reload) {          /* wrap after passing RELOAD */
                count = 0;
                ovf   = (ovf + 1u) & 0xFFFFu;
                match = 1;
            } else {
                count++;
            }
        } else {
            s_prescale_acc++;
        }
    }

    g_mmio[1] = count & 0xFFFFu;
    g_mmio[2] = (ovf << 16) | (match << 1) | running;
}
