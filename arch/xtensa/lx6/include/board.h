#ifndef __BOARD_H
#define __BOARD_H

#include <stdint.h>
#include <board_cfg.h>
#include <xtensa.h>
#include <soc.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define RAM_BASE              (void *)(0x3FFF0000)
#define STACK_TOP             (void *)(0x3FFFFFFF)

#define HEAP_BLOCK_SIZE       (16)

void board_init(void);

#endif /* __BOARD_H */
