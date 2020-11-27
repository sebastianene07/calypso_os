#ifndef __BOARD_H
#define __BOARD_H

#include <board_cfg.h>
#include <stdint.h>
#include <irq_manager.h>
#include <simulator.h>
#include <scheduler.h>

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

int cpu_savecontext(void **task_sp);

void cpu_restorecontext(void *task_sp);

/****************************************************************************
 * CPU interrupt management functions
 ****************************************************************************/

void cpu_enableint(irq_state_t irq_state);

irq_state_t cpu_disableint(void);

int cpu_getirqnum(void);

void cpu_attachint(int irq_num, irq_cb handler);

int cpu_detachint(int irq_num);

#endif /* __BOARD_H */
