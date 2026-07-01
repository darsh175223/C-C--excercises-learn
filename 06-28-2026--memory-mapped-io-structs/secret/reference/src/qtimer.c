#include "qtimer.h"

/*
 * QTimer-16 driver.
 *
 * All hardware access goes through the QTIMER->CTRL bitfields so the register
 * map in qtimer.h is the single source of truth for where each control bit
 * lives. Keep it that way: no manual shifts/masks in here.
 */

void qtimer_config(uint8_t mode, uint8_t prescale, uint16_t reload)
{
    QTIMER->CTRL.EN       = 1;
    QTIMER->CTRL.START    = 0;
    QTIMER->CTRL.MODE     = mode & 0x3u;
    QTIMER->CTRL.PRESCALE = prescale & 0xFu;
    QTIMER->CTRL.RELOAD   = reload;
}

void qtimer_start(void)
{
    QTIMER->CTRL.EN    = 1;
    QTIMER->CTRL.START = 1;
}

void qtimer_stop(void)
{
    QTIMER->CTRL.START = 0;
    QTIMER->CTRL.EN    = 0;
}

uint16_t qtimer_count(void)
{
    return (uint16_t)QTIMER->COUNT;
}

int qtimer_running(void)
{
    return (int)(QTIMER->STATUS & 0x1u);
}

uint16_t qtimer_overflow_count(void)
{
    return (uint16_t)((QTIMER->STATUS >> 16) & 0xFFFFu);
}
