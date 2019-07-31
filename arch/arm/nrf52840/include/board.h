#ifndef __CORE_CM4
#define __CORE_CM4

#include <stdint.h>
#include <board_cfg.h>
#include <nrf52840.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define RAM_BASE              (void *)(0x20000000)
#define STACK_TOP             (void *)(RAM_BASE + 0x5000)

#define HEAP_START            (void *)(RAM_BASE + 0x5004)
#define HEAP_END              (void *)(RAM_BASE + 0x19000)
#define HEAP_BLOCK_SIZE       (16)


void board_init(void);

#endif /* __CORE_CM4 */
