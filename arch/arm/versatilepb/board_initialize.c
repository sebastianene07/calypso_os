#include <board.h>

#include <errno.h>
#include <serial.h>
#include <stdint.h>
#include <scheduler.h>
#include <os_start.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* The MCU context registers */

#define REG_R0                (0)
#define REG_R1                (1)
#define REG_R2                (2)
#define REG_R3                (3)
#define REG_R4                (4)
#define REG_R5                (5)
#define REG_R6                (6)
#define REG_R7                (7)
#define REG_R8                (8)
#define REG_R9                (9)

#define REG_R10               (10)
#define REG_R11               (11)
#define REG_R12               (12)
#define REG_SP                (13)
#define REG_LR                (14)
#define REG_PC                (15)
#define REG_XPSR              (16)
#define REG_FP                (17)

#define REG_NUMS              (18)

/****************************************************************************
 * Private Types
 ****************************************************************************/

/* The CPU stacking registers  */

typedef struct cpu_stacking_s
{
  void *fp;
  void *r0;
  void *r1;
  void *r2;
  void *r3;
  void *r12;
  void *lr;
  void *pc;
  void *xpsr;
} __attribute__((packed)) cpu_stacking_s;

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
 * task_entry_point - a trampoline used to setup task arguments
 */
void task_entry_point(void)
{
  tcb_t *tcb = sched_get_current_task();
  void **mcu_context = (void **)tcb->mcu_context;
  tcb->entry_point((int)mcu_context[REG_R0], mcu_context[REG_R1]);
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
  mcu_context[REG_PC]   = (void *)task_entry_point;
  mcu_context[REG_XPSR] = (void *)0x1000000;
  mcu_context[REG_SP]   = bottom_sp;

  /* Setup the initial stack */

  cpu_stacking_s *initial_stack = (cpu_stacking_s *)bottom_sp;
  initial_stack->fp = bottom_sp;
  initial_stack->r0 = (void *)argc;
  initial_stack->r1 = (void *)argv;
  initial_stack->lr = (void *)sched_default_task_exit_point;
  initial_stack->pc = (void *)task_entry_point;
  initial_stack->xpsr = (void *)0x1000000;

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
