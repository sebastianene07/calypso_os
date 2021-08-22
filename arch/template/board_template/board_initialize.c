#include <board.h>

#include <errno.h>
#include <serial.h>
#include <stdint.h>
#include <scheduler.h>
#include <os_start.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/****************************************************************************
 * Private Types
 ****************************************************************************/

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
  size_t num_uarts;

  uart_low_init();
  printf("\r\nTemplate initializing\r\n.");

  uart_init(&num_uarts);
}

/*
 * cpu_inittask - creates the initial state for a task
 *
 */
int cpu_inittask(struct tcb_s *tcb, int argc, char **argv)
{
  /* TODO  */
  return 0;
}

/*
 * cpu_destroytask - creates the initial state for a task
 *
 */
void cpu_destroytask(tcb_t *tcb)
{
  /* TODO  */
}

/*
 * cpu_disableint - Disable all the interrupts and return the primask register.
 *
 */
irq_state_t cpu_disableint(void)
{
  /* TODO  */
  return 0;
}

/*
 * cpu_enableint - Enable the interrupts if the first bit of the irq_state is 1.
 *
 */
void cpu_enableint(irq_state_t irq_state)
{
  /* TODO  */
}

/*
 * cpu_getirqnum - Return the interrupt number.
 *
 * Assumptions: This should be called only from ISR handler.
 *
 */
int cpu_getirqnum(void)
{
  /* TODO  */
  return 0;
}
