#include <board.h>

#include <serial.h>
#include <stdint.h>
#include <scheduler.h>
#include <os_start.h>

/* Ram based ISR vector */
void (*g_ram_vectors[NUM_IRQS])(void);

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/*
 * board_init - initialize the board resources
 *
 * Initialize the board specific device drivers and prepare the board.
 */
void board_init(void)
{
  printf("\r\nXTENSA lx6 initializing\r\n.");
}

/*
 * up_initial_task_context - creates the initial state for a task
 *
 */
int up_initial_task_context(struct tcb_s *tcb)
{
  /* Initial MCU context */

  task_tcb->mcu_context[0] = (void *)argc;
  task_tcb->mcu_context[1] = (void *)argv;
  task_tcb->mcu_context[5] = (uint32_t *)sched_default_task_exit_point;
  task_tcb->mcu_context[6] = task_entry_point;
  task_tcb->mcu_context[7] = (uint32_t *)0x1000000;

  return 0;
}
