#include <board.h>

#include <errno.h>
#include <serial.h>
#include <stdint.h>
#include <scheduler.h>
#include <os_start.h>

#include "bsp.h"

#define __NVIC_PRIO_BITS (0)

#include <core_cm0plus.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/****************************************************************************
 * Private Types
 ****************************************************************************/

/****************************************************************************
 * Public Functions
 ****************************************************************************/

struct uart_lower_s *uart_upper_init(size_t *uart_num);

/*
 * board_init - initialize the board resources
 *
 * Initialize the board specific device drivers and prepare the board.
 */
void board_init(void)
{
  size_t uart_num = 0;

  bsp_gpio_led_init();
  uart_upper_init(&uart_num);
}

/*
 * cpu_inittask - creates the initial state for a task
 *
 */
int cpu_inittask(struct tcb_s *tcb, int argc, char **argv)
{
  void **mcu_context = calloc(REG_NUMS, sizeof(void *));
  if (mcu_context == NULL)
  {
    return -ENOMEM;
  }

  void *bottom_sp =
    (void *)((unsigned int)tcb->stack_ptr_top - 8 * sizeof(void *));

  /* Initial MCU context */

  mcu_context[REG_R0]   = (void *)argc;
  mcu_context[REG_R1]   = (void *)argv;
  mcu_context[REG_LR]   = (void *)sched_default_task_exit_point;
  mcu_context[REG_PC]   = (void *)tcb->entry_point;
  mcu_context[REG_XPSR] = (void *)0x1000000;
  mcu_context[REG_SP]   = bottom_sp;

  /* Setup the initial stack context  */

  tcb->mcu_context = mcu_context;
  return 0;
}

/*
 * cpu_destroytask - creates the initial state for a task
 *
 */
void cpu_destroytask(tcb_t *tcb)
{
  free(tcb->mcu_context);
  free(tcb);
}

/*
 * cpu_disableint - Disable all the interrupts and return the primask register.
 *
 */
irq_state_t cpu_disableint(void)
{
  unsigned short primask;
  __asm volatile("mrs %0, primask\n"
                 "cpsid i\n"
                 : "=r" (primask)
                 :
                 : "memory");
  return (irq_state_t)primask;
}

/*
 * cpu_enableint - Enable the interrupts if the first bit of the irq_state is 1.
 *
 */
void cpu_enableint(irq_state_t irq_state)
{
    __asm volatile(
        "msr primask, %0\n"
        "cpsie i\n"
        :
        : "r" (irq_state));
}

/*
 * cpu_getirqnum - Return the interrupt number.
 *
 * Assumptions: This should be called only from ISR handler.
 *
 */
int cpu_getirqnum(void)
{
  int ipsr;

  __asm volatile("mrs %0, ipsr\n"
                 : "=r" (ipsr)
                 :
                 : "memory");

  ipsr -= 16;
  return ipsr;
}
