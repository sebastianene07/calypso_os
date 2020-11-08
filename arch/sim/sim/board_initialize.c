/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <board.h>

#include <errno.h>
#ifdef CONFIG_SIMULATED_FLASH
  #include <storage/simulated_flash.h>
#endif
#include <scheduler.h>
#include <serial.h>
#include <stdint.h>
#include <string.h>
#include <os_start.h>
#include <ucontext.h>
#include <rtc.h>

/****************************************************************************
 * Pre-processor Defintions
 ****************************************************************************/

/* The stack alignment in bytes */

#define STACK_ALIGNMENT               (8)

/****************************************************************************
 * Private Types
 ****************************************************************************/

/* This is an arch specific container used to keep the task context and
 * the arguments to the task.
 */

typedef struct sim_mcu_context_s {
  ucontext_t *task_ucontext;
  char **argv;
  int argc;
} sim_mcu_context_t;

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

#ifdef CONFIG_SIMULATED_FLASH
  /* Initialize the simulated flash */

  sim_flash_init();
#endif

  /* Start the SysTick simulation using the host timer */

  host_simulated_systick();
}

/****************************************************************************
 * Name: task_default_entry_point
 *
 * Description:
 *   This is the default entry point for each new task. After this
 *   function finishes it calls into the sched_context_switch
 *   to switch the context to the next running task.
 *
 * Input Parameters:
 *   argc - the number of the arguments to the task
 *   argv - the argument of the task
 *
 ****************************************************************************/

static void task_default_entry_point(int argc, char **argv)
{
  disable_int();
  struct tcb_s *curr_tcb = sched_get_current_task();
  sim_mcu_context_t *mcu_context = curr_tcb->mcu_context;
  enable_int();

  curr_tcb->entry_point(mcu_context->argc, mcu_context->argv);
  curr_tcb->t_state = HALTED;
  curr_tcb->waiting_tcb_sema = NULL;
   
  sched_context_switch();
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
  size_t stack_size = tcb->stack_ptr_top - tcb->stack_ptr_base -
    sizeof(struct tcb_s);

  sim_mcu_context_t *mcu_context = calloc(1, sizeof(sim_mcu_context_t));
  if (mcu_context == NULL) {
    return -ENOMEM;
  }
  tcb->mcu_context = mcu_context;

  mcu_context->task_ucontext = calloc(1, sizeof(ucontext_t));
  if (mcu_context->task_ucontext == NULL) {
    free(mcu_context);
    return -ENOMEM;
  }

  ucontext_t *task_context = mcu_context->task_ucontext;
  int ret = getcontext(task_context);
  if (ret < 0) {
    return ret;
  }

  /* When the task has done running it should be chainned to jump to a new
   * ucontext structure.
   */

  task_context->uc_stack.ss_sp    = tcb->stack_ptr_top;// + stack_size;
  task_context->uc_stack.ss_size  = stack_size;
  task_context->uc_stack.ss_flags = 0;

  mcu_context->argv = 0;

  if (argc != 0) {
    mcu_context->argv = calloc(argc, sizeof(char *));
    if (mcu_context->argv == NULL) {
      argc = 0;
    } else {

      /* Copy the arguments */

      for (int i = 0; i < argc; i++) {
        mcu_context->argv[i] = calloc(1, strlen(argv[i]) + 1);
        if (mcu_context->argv[i] == NULL)
          break;

        memcpy(mcu_context->argv[i], argv[i], strlen(argv[i]));
        ++mcu_context->argc;
      }
    } 
  }

  /* Create the ucontext structure */

  makecontext(task_context,
              (void (*)(void))task_default_entry_point,
              2,
              mcu_context->argc,
              mcu_context->argv);

  return 0;
}

/****************************************************************************
 * Name: up_destroy_task_context
 *
 * Description:
 *   This function destroys the task context and it's associated resources.
 *   It is called from the Idle task when the Idle task detects that we
 *   have pending tasks that need to be destroyed.
 *
 * Input Parameters:
 *   tcb  - the task control block
*
 * Return Value:
 *   On success returns 0 otherwise a negative error code.
 *
 ****************************************************************************/

int up_destroy_task_context(struct tcb_s *tcb)
{
  sim_mcu_context_t *mcu_context = tcb->mcu_context;

  /* Free the arguments */

  for (int i = 0; i < mcu_context->argc; i++) {
    free(mcu_context->argv[i]);
  }

  if (mcu_context->argc > 0)
    free(mcu_context->argv);

  /* Free the ucontext stack */

  free(mcu_context->task_ucontext);
  free(mcu_context);
  return OK;
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
  extern struct list_head g_tcb_waiting_list;

  if (current_task == next_task)
    return;

  if (current_task->t_state == WAITING_FOR_SEM ||
      current_task->t_state == HALTED) {

    /* We should not change it's state */

    if (current_task->t_state == HALTED) {
      list_del(&current_task->next_tcb);
      list_add(&current_task->next_tcb, &g_tcb_waiting_list);
    }
  } else {

    /* Move the task to READY state */

    current_task->t_state = READY;
  }

  /* Switch task state to running */

  next_task->t_state = RUNNING;

  ucontext_t *old_mcu_ctxt =
    ((sim_mcu_context_t *)current_task->mcu_context)->task_ucontext;
  ucontext_t *next_mcu_ctxt =
    ((sim_mcu_context_t *)next_task->mcu_context)->task_ucontext;

  swapcontext(old_mcu_ctxt, next_mcu_ctxt);
}
