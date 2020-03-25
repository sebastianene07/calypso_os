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
#define STACK_TOP             (void *)(0x11000)

#define HEAP_BLOCK_SIZE       (16)
#define up_destroy_task_context(x)       {}

void board_init(void);

int up_initial_task_context(struct tcb_s *tcb, int argc, char **argv);

#endif /* __BOARD_H */
