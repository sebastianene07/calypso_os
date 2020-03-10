#include <board.h>

#include <serial.h>
#include <stdint.h>
#include <scheduler.h>
#include <os_start.h>
#include <ucontext.h>

/****************************************************************************
 * Public Function Prototype
 ****************************************************************************/

void host_simulated_systick(void);

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
  printf("\r\nSimulator initializing\r\n.");

  host_simulated_systick();
}

/*
 * up_initial_task_context - creates the initial state for a task
 *
 */
int up_initial_task_context(struct tcb_s *tcb)
{
  ucontext_t *task_context = &tcb->mcu_context;
  int ret = getcontext(task_context);
  if (ret < 0) {
    return ret;
  }

  size_t stack_size = tcb->stack_ptr_top - tcb->stack_ptr_base;
  task_context->uc_stack.ss_sp    = calloc(stack_size, 1);
  task_context->uc_stack.ss_size  = tcb->stack_ptr_top - tcb->stack_ptr_base;
  task_context->uc_stack.ss_flags = 0;

  makecontext(task_context, (void *)tcb->entry_point, 1);

  return 0;
}

void sched_context_switch(void)
{
  struct tcb_s *current_task = sched_get_current_task();
  struct tcb_s *next_task = sched_get_next_task(); 

  if (current_task->t_state != WAITING_FOR_SEM) {
    current_task->t_state = READY;
  }

  printf("\nOld task : %x new task %x\n", current_task, next_task); 
  next_task->t_state    = RUNNING; 
  
  swapcontext(&current_task->mcu_context, &next_task->mcu_context);
}
