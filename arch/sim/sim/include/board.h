#ifndef __BOARD_H
#define __BOARD_H

#include <stdint.h>
#include <board_cfg.h>
#include <simulator.h>

#include <scheduler.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

void board_init(void);

int up_initial_task_context(struct tcb_s *tcb, int argc, char **argv);

#endif /* __BOARD_H */
