#ifndef __BOARD_H
#define __BOARD_H

#include <stdint.h>
#include <board_cfg.h>
#include <simulator.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define RAM_BASE              (void *)(0x10000)
#define STACK_TOP             (void *)(0x11000)

#define HEAP_BLOCK_SIZE       (32)

void board_init(void);

#endif /* __BOARD_H */
