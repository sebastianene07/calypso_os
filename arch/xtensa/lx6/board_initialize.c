#include <board.h>

#include <errno.h>
#include <serial.h>
#include <stdint.h>
#include <scheduler.h>
#include <os_start.h>

/* Ram based ISR vector */
void (*g_ram_vectors[NUM_IRQS])(void);

/****************************************************************************
 * Public Functions
 ****************************************************************************/

__attribute__((section(".boot_entry"))) void bootloader_entry(void)
{
  __start();
}

/* __assert_func - create a dump 
 *
 */
void __assert_func(bool assert_cond)
{
} 

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
