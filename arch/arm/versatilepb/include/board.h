#ifndef __BOARD_H
#define __BOARD_H

#include <stdint.h>
#include <board_cfg.h>
#include <versatilepb.h>

#include <scheduler.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define RAM_BASE              (void *)(0x10000)

/* This is ignored because when the system boots up the _bootload code is
 * executed and the initial stack is copied from _estack (defined in the
 * linker file).
 */

#define STACK_TOP             (void *)(0x1900c)
#define HEAP_BLOCK_SIZE       (16)

/****************************************************************************
 * Peripheral initialization function for the board
 ****************************************************************************/

void board_init(void);

void board_entersleep(void);

/****************************************************************************
 * Task management functions
 ****************************************************************************/

int cpu_inittask(struct tcb_s *tcb, int argc, char **argv);

void cpu_destroytask(tcb_t *tcb);

/****************************************************************************
 * CPU context management functions
 ****************************************************************************/

int cpu_savecontext(void *mcu_context);

void cpu_restorecontext(void *mcu_context);

/****************************************************************************
 * CPU interrupt management functions
 ****************************************************************************/

void cpu_enableint(irq_state_t irq_state);

irq_state_t cpu_disableint(void);

int cpu_getirqnum(void);

#endif /* __BOARD_H */
