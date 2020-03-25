/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <board.h>

#ifdef CONFIG_SIMULATED_FLASH
  #include <storage/simulated_flash.h>
#endif

#include <serial.h>
#include <stdint.h>
#include <scheduler.h>
#include <os_start.h>
#include <ucontext.h>
#include <rtc.h>
#include <errno.h>

/****************************************************************************
 * Pre-processor Defintions
 ****************************************************************************/

/* The stack alignment in bytes */

#define STACK_ALIGNMENT               (8)

/* The default stack size for the xist point */

#define STACK_DEFAULT_EXIT_POINT      (128000)

/****************************************************************************
 * Private Types
 ****************************************************************************/

typedef struct sim_mcu_context_s {
  ucontext_t *task_mcu_context;
  ucontext_t *exit_mcu_context;
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
 * Name: up_create_exit_point
 *
 * Description:
 *   Create the exit point structure.
 *
 * Return Value:
 *   Return the address of the exit context.
 *
 ****************************************************************************/

static ucontext_t *up_create_exit_point(void)
{
  ucontext_t *exit_context = calloc(1, sizeof(ucontext_t));
  if (exit_context == NULL) {
    return NULL;
  }

  uint8_t *exit_stack = calloc(1, STACK_DEFAULT_EXIT_POINT);
  if (exit_stack == NULL) {
    free(exit_context);
    return NULL;
  }

  getcontext(exit_context);

  exit_context->uc_stack.ss_sp    = exit_stack;
  exit_context->uc_stack.ss_size  = STACK_DEFAULT_EXIT_POINT;
  exit_context->uc_stack.ss_flags = 0;
  exit_context->uc_link           = NULL;

  makecontext(exit_context, sched_default_task_exit_point, 0);

  return exit_context;
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
  size_t stack_size = tcb->stack_ptr_top - tcb->stack_ptr_base;
  uint8_t *sp = tcb->stack_ptr_base;

  sim_mcu_context_t *mcu_context = calloc(1, sizeof(sim_mcu_context_t));
  if (mcu_context == NULL) {
    return -ENOMEM;
  }
  tcb->mcu_context = mcu_context;

  mcu_context->task_mcu_context = calloc(1, sizeof(ucontext_t));
  if (mcu_context->task_mcu_context == NULL) {
    free(mcu_context);
    return -ENOMEM;
  }

  ucontext_t *task_context = mcu_context->task_mcu_context;
  int ret = getcontext(task_context);
  if (ret < 0) {
    return ret;
  }

  mcu_context->exit_mcu_context = up_create_exit_point();
  if (mcu_context->exit_mcu_context == NULL) {
    free(mcu_context->task_mcu_context);
    free(mcu_context);
    return -ENOMEM;
  }

  struct tcb_s *current = sched_get_current_task();
  task_context->uc_stack.ss_sp    = sp;
  task_context->uc_stack.ss_size  = stack_size;
  task_context->uc_stack.ss_flags = 0;
  task_context->uc_link           = mcu_context->exit_mcu_context;

  mcu_context->argv = 0;

  if (argc != 0) {
    mcu_context->argv = calloc(argc, sizeof(char *));
    if (mcu_context->argv == NULL)
      argc = 0;
    else {

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

  makecontext(task_context, (void *)tcb->entry_point, 2, mcu_context->argc,
              mcu_context->argv);

  return 0;
}

/****************************************************************************
 * Name: up_destroy_task_context
 *
 * Description:
 *   This function destroys the task context and it's associated resource..
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

  free(mcu_context->exit_mcu_context->uc_stack.ss_sp);
  free(mcu_context->exit_mcu_context);

  free(mcu_context->task_mcu_context);
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
    ((sim_mcu_context_t *)current_task->mcu_context)->task_mcu_context;
  ucontext_t *next_mcu_ctxt =
    ((sim_mcu_context_t *)next_task->mcu_context)->task_mcu_context;

  swapcontext(old_mcu_ctxt, next_mcu_ctxt);
}
