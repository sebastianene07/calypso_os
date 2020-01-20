#ifndef __CORE_CM4
#define __CORE_CM4

#include <stdint.h>
#include <board_cfg.h>
#include <nrf52832.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define RAM_BASE              (void *)(CONFIG_RAM_BASE)
#define RAM_LENGTH            (CONFIG_RAM_LENGTH)

#define STACK_TOP             (void *)(CONFIG_RAM_BASE + CONFIG_RAM_LENGTH)

#define HEAP_BLOCK_SIZE       (16)


void board_init(void);

#endif /* __CORE_CM4 */
