#ifndef QTIMER_H
#define QTIMER_H

#include <stdint.h>

/*
 * ===========================================================================
 *  QTimer-16  —  memory-mapped timer peripheral  (datasheet rev B)
 * ===========================================================================
 *
 *  Base address: 0x4000_0000.  Every register is 32 bits wide.
 *
 *    Offset  Name     Access  Description
 *    ------  -------  ------  --------------------------------------------
 *    0x00    CTRL     R/W     Control register (bit layout below)
 *    0x04    COUNT    RO      Current 16-bit up-counter value
 *    0x08    STATUS   RO      Status / event flags
 *
 *  CTRL register bit layout
 *  ------------------------
 *    Bits    Field      Width  Meaning
 *    -----   --------   -----  ------------------------------------------------
 *    0       EN         1      1 = timer block enabled
 *    1       START      1      write 1 to (re)load COUNT=0 and run
 *    3:2     MODE       2      0 = one-shot, 1 = periodic, 2 = pwm
 *    7:4     PRESCALE   4      clock divider; tick rate = fclk / (PRESCALE + 1)
 *    23:8    RELOAD     16     COUNT wraps to 0 after it passes RELOAD
 *    31:24   reserved   8      write 0
 *
 *  STATUS register bit layout
 *  --------------------------
 *    Bits    Field      Width  Meaning
 *    -----   --------   -----  ------------------------------------------------
 *    0       RUNNING    1      timer is currently counting
 *    1       MATCH      1      set on the most recent wrap event
 *    31:16   OVFCOUNT   16     total number of wrap events since reset
 * ===========================================================================
 */

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

/* The platform maps the peripheral here. In the host simulation the harness
 * provides the backing storage; on silicon this would be 0x4000_0000. */
extern volatile QTimer_Regs* const QTIMER;

/* The register block must stay 12 bytes so COUNT and STATUS land on their
 * documented offsets. */
_Static_assert(sizeof(QTimer_CTRL) == 4,  "CTRL must be exactly one 32-bit register");
_Static_assert(sizeof(QTimer_Regs) == 12, "register block layout drifted");

/* ---- driver API (implemented in src/qtimer.c) ---- */
enum { QT_MODE_ONESHOT = 0, QT_MODE_PERIODIC = 1, QT_MODE_PWM = 2 };

void     qtimer_config(uint8_t mode, uint8_t prescale, uint16_t reload);
void     qtimer_start(void);
void     qtimer_stop(void);
uint16_t qtimer_count(void);
int      qtimer_running(void);
uint16_t qtimer_overflow_count(void);

#endif /* QTIMER_H */
