#include <board.h>

#include <errno.h>
#include <serial.h>
#include <stdint.h>
#include <scheduler.h>
#include <os_start.h>

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
  uart_low_init();
  printf("\r\nQEMU versatilepb initializing\r\n.");
}

/*
 * cpu_inittask - creates the initial state for a task
 *
 */
int cpu_inittask(struct tcb_s *tcb, int argc, char **argv)
{
  return -ENOSYS;
}

/*
 * cpu_destroytask - creates the initial state for a task
 *
 */
void cpu_destroytask(tcb_t *tcb)
{
}

/*
 * cpu_savecontext - save the task context
 *
 */
int cpu_savecontext(void *mcu_context)
{
  return 0;
}

/*
 * cpu_restorecontext - save the task context
 *
 */
void cpu_restorecontext(void *mcu_context)
{
}

/*
 * cpu_disableint - Disable all the interrupts and return the primask register.
 *
 */
irq_state_t cpu_disableint(void)
{
  return 0;
}

/*
 * cpu_enableint - Enable the interrupts if the first bit of the irq_state is 1.
 *
 */
void cpu_enableint(irq_state_t irq_state)
{
}

/*
 * cpu_getirqnum - Return the interrupt number.
 *
 * Assumptions: This should be called only from ISR handler.
 *
 */
int cpu_getirqnum(void)
{
  return -ENOSYS;
}
