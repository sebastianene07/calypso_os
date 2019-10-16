#ifndef __CORE_CM4
#define __CORE_CM4

#include <stdint.h>
#include <board_cfg.h>
#include <nrf52840.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define RAM_BASE              (void *)(0x20000000)
#define STACK_TOP             (void *)(0xDEADBEEF)

#define HEAP_BLOCK_SIZE       (16)


void board_init(void);

#endif /* __CORE_CM4 */
