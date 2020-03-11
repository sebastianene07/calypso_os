/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <board.h>

#include <serial.h>
#include <stdint.h>
#include <scheduler.h>
#include <os_start.h>
#include <ucontext.h>
#include <rtc.h>

/****************************************************************************
 * Pre-processor Defintions
 ****************************************************************************/

/* The stack alignment in bytes */

#define STACK_ALIGNMENT               (8)

/****************************************************************************
 * Public Function Prototype
 ****************************************************************************/

/* This function starts to simulate systick events using a host timer */

void host_simulated_systick(void);

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: board_init
 *
 * Description:
 *   Board initialization function, here we do the peripheral initializations
 *   and we setup the host timer to simulate tick events.
 *
 ****************************************************************************/

void board_init(void)
{
  /* Lower level UART should be available to print individual characters to 
   * the sys console.
   */

  printf("\r\n[board_init] Simulation init\r\n");

  /* Initialize the UART simulated driver */

  size_t num_uart = 0;
  uart_init(&num_uart);

  /* Initialize the RTC simulated driver */

  rtc_init();

  /* Start the SysTick simulation using the host timer */ 

  host_simulated_systick();
}

/****************************************************************************
 * Name: up_initial_task_context
 *
 * Description:
 *   This function sets up the task initial context.
 *
 * Input Parameters:
 *   tcb  - the task control block
 *   argc - number of arguments for the entry point
 *   argv - arguments buffer for the entry point
 *
 * Return Value:
 *   On success returns 0 otherwise a negative error code.
 *
 ****************************************************************************/

int up_initial_task_context(struct tcb_s *tcb, int argc, char **argv)
{
  ucontext_t *task_context = &tcb->mcu_context;
  int ret = getcontext(task_context);
  if (ret < 0) {
    return ret;
  }

  size_t stack_size = tcb->stack_ptr_top - tcb->stack_ptr_base;
  uint8_t *sp = tcb->stack_ptr_top;

  /* Align the stack to 8 bytes */

  sp = (uint64_t)sp + (STACK_ALIGNMENT - ((uint64_t)sp % STACK_ALIGNMENT));

  struct tcb_s *current = sched_get_current_task();

  task_context->uc_stack.ss_sp    = sp;
  task_context->uc_stack.ss_size  = stack_size - (uint64_t)sp % STACK_ALIGNMENT;
  task_context->uc_stack.ss_flags = 0;
  task_context->uc_link           = &current->mcu_context;

  makecontext(task_context, (void *)tcb->entry_point, argc, argv);
  return 0;
}

/****************************************************************************
 * Name: sched_context_switch
 *
 * Description:
 *   This function switches the context to the next available & ready to run
 *   task. This function does not return.
 *
 ****************************************************************************/

void sched_context_switch(void)
{
  struct tcb_s *current_task = sched_get_current_task();
  struct tcb_s *next_task = sched_get_next_task(); 

  if (current_task == next_task)
    return;

  if (current_task->t_state != WAITING_FOR_SEM) {
    current_task->t_state = READY;
  }

  next_task->t_state    = RUNNING;   
  swapcontext(&current_task->mcu_context, &next_task->mcu_context);
}
