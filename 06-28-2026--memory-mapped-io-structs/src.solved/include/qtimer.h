#ifndef QTIMER_H
#define QTIMER_H

#include <stdint.h>

/* Reference (fixed) register map. See the learner-visible header for the full
 * datasheet comment. The only change vs. the buggy version is the bitfield
 * storage-unit type: uint16_t -> uint32_t. */

typedef struct {
    volatile uint32_t EN       : 1;
    volatile uint32_t START    : 1;
    volatile uint32_t MODE     : 2;
    volatile uint32_t PRESCALE : 4;
    volatile uint32_t RELOAD   : 16;
} QTimer_CTRL;

typedef struct {
    QTimer_CTRL       CTRL;    /* 0x00 */
    volatile uint32_t COUNT;   /* 0x04 */
    volatile uint32_t STATUS;  /* 0x08 */
} QTimer_Regs;

extern volatile QTimer_Regs* const QTIMER;

_Static_assert(sizeof(QTimer_CTRL) == 4,  "CTRL must be exactly one 32-bit register");
_Static_assert(sizeof(QTimer_Regs) == 12, "register block layout drifted");

enum { QT_MODE_ONESHOT = 0, QT_MODE_PERIODIC = 1, QT_MODE_PWM = 2 };

void     qtimer_config(uint8_t mode, uint8_t prescale, uint16_t reload);
void     qtimer_start(void);
void     qtimer_stop(void);
uint16_t qtimer_count(void);
int      qtimer_running(void);
uint16_t qtimer_overflow_count(void);

#endif /* QTIMER_H */
