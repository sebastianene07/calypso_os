#include <board.h>

#include <errno.h>
#include <serial.h>
#include <stdint.h>
#include <scheduler.h>
#include <os_start.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* The MCU context registers and their indexes for stacking */

#define REG_R0                (13)
#define REG_R1                (14)

#define REG_SP                (10)
#define REG_LR                (11)
#define REG_PC                (0)
#define REG_XPSR              (16)

#define REG_NUMS              (18)

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
  printf("\r\nQEMU versatilepb initializing\r\n.");

  uart_init(&num_uarts);
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
  irq_state_t reg = PIC_IntEnable;
  PIC_IntEnClear = 0xFFFFFF;
  return reg;
}

/*
 * cpu_enableint - Enable the interrupts if the first bit of the irq_state is 1.
 *
 */
void cpu_enableint(irq_state_t irq_state)
{
  PIC_IntEnable = irq_state;
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

  __asm volatile("mrs %0, apsr\n"
                 : "=r" (ipsr)
                 :
                 : "memory");

  ipsr -= 16;
  return ipsr;
}
