#ifndef __CORE_CM4
#define __CORE_CM4

#include <stdint.h>
#include <board_cfg.h>
#include <nrf52840.h>

#include <scheduler.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define RAM_BASE              (void *)(CONFIG_RAM_BASE)
#define RAM_LENGTH            (CONFIG_RAM_LENGTH)

#define STACK_TOP             (void *)(CONFIG_RAM_BASE + CONFIG_RAM_LENGTH)

#define HEAP_BLOCK_SIZE       (16)

#define up_destroy_task_context(x)       {}

void board_init(void);

int up_get_irq_number(void);

int up_initial_task_context(struct tcb_s *tcb, int argc, const char **argv);

#endif /* __CORE_CM4 */
