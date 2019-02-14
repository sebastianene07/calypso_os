/*
 * scheduler.c
 *
 * Created: 4/6/2018 3:09:38 AM
 *  Author: sene
 */

#include <board.h>

#include <scheduler.h>
#include <errno.h>
#include <stdlib.h>

/* Running task list */

LIST_HEAD(g_tcb_list);

/* Waiting for sem task list */

LIST_HEAD(g_tcb_waiting_list);

/* Current running task */

static struct list_head *g_current_tcb = NULL;

/**************************************************************************
 * Name:
 *  sched_idle_task
 *
 * Description:
 *  Idle task.
 *
 * Return Value:
 *  OK in case of success otherwise a negate value.
 *************************************************************************/
static void sched_idle_task(void)
{
  while (1)
  {
    ;;
  }
}

/**************************************************************************
 * Name:
 *  sched_init
 *
 * Description:
 *  Init scheduler resources.
 *
 * Return Value:
 *  OK in case of success otherwise a negate value.
 *************************************************************************/
int sched_init(void)
{
  /* Create idle task */

  int ret = sched_create_task(sched_idle_task,
                              CONFIG_SCHEDULER_IDLE_TASK_STACK_SIZE);
  if (ret < 0)
  {
    return ret;
  }

  g_current_tcb = g_tcb_list.next;

  return 0;
}

/**************************************************************************
 * Name:
 *  sched_create_task
 *
 * Description:
 *  Create a new task.
 *
 * Input Parameters:
 *  task_entry_point - the entry point of a task
 *  stack_size       - the stack size of the new task
 *
 * Assumptions:
 *  Call this function with interrupts disabled.
 *
 * Return Value:
 *  OK in case of success otherwise a negate value.
 *************************************************************************/
int sched_create_task(void (*task_entry_point)(void), uint32_t stack_size)
{
  struct tcb_s *task_tcb = malloc(sizeof(struct tcb_s) + stack_size);
  if (task_tcb == NULL)
  {
    return -ENOMEM;
  }

  task_tcb->entry_point    = task_entry_point;
  task_tcb->stack_ptr_base = (void *)task_tcb + sizeof(struct tcb_s);
  task_tcb->stack_ptr_top  = (void *)task_tcb + stack_size;
  task_tcb->t_state        = READY;

#ifdef CONFIG_SCHEDULER_TASK_COLORATION
  /* The effective stack size is base - top */

  for (uint8_t *ptr = task_tcb->stack_ptr_base;
     ptr < (uint8_t*)task_tcb->stack_ptr_top;
     ptr = ptr + sizeof(uint32_t))
  {
    *((uint32_t *)ptr) = 0xDEADBEEF;
  }
#endif

  /* Initial MCU context */

  task_tcb->mcu_context[0] = NULL;
  task_tcb->mcu_context[1] = NULL;
  task_tcb->mcu_context[2] = NULL;
  task_tcb->mcu_context[3] = NULL;
  task_tcb->mcu_context[4] = NULL;
  task_tcb->mcu_context[5] = (uint32_t *)0xffffffff;//sched_default_task_exit_point;
  task_tcb->mcu_context[6] = task_entry_point;
  task_tcb->mcu_context[7] = (uint32_t *)0x1000000;

  /* Stack context in interrupt */
  const int unstacked_regs = 8;   /* R4-R11 */
  int i = 0;
  void *ptr_after_int = task_tcb->stack_ptr_top -
    sizeof(void *) * MCU_CONTEXT_SIZE;

  for (uint8_t *ptr = ptr_after_int;
     ptr < (uint8_t *)task_tcb->stack_ptr_top;
     ptr += sizeof(uint32_t))
  {
    *((uint32_t *)ptr) = (uint32_t)task_tcb->mcu_context[i++];
  }

  task_tcb->sp = ptr_after_int - unstacked_regs * sizeof(void *);

  /* Insert the task in the list */

  __disable_irq();

  list_add(&task_tcb->next_tcb, &g_tcb_list);

  __enable_irq();

  return 0;
}

/**************************************************************************
* Name:
* sched_get_next_task
*
* Description:
*  Init scheduler resources. Should not be called before sched_init
*
* Return Value:
*  The TCB of the next task or NULL if the scheduler is not initialized.
*************************************************************************/
struct tcb_s *sched_get_next_task(void)
{
  g_current_tcb = g_current_tcb->next;
  if (g_current_tcb == &g_tcb_list)
    {
      g_current_tcb = g_current_tcb->next;
    }

  struct tcb_s *next_tcb = (struct tcb_s *)container_of(g_current_tcb,
    struct tcb_s, next_tcb);
  return next_tcb;
}

/**************************************************************************
* Name:
* sched_run
*
* Description:
*  Pick the next task to be run.
*
* Return Value:
*  The TCB of the next task or NULL if the scheduler is not initialized.
*************************************************************************/
void sched_run(void)
{
  sched_idle_task();
}

/**************************************************************************
* Name:
* sched_get_current_task
*
* Description:
*  Get current runing task.
*
* Return Value:
*  The TCB of the next task or NULL if the scheduler is not initialized.
*************************************************************************/
struct tcb_s *sched_get_current_task(void)
{
  struct tcb_s *next_tcb = (struct tcb_s *)container_of(g_current_tcb,
    struct tcb_s, next_tcb);
  return next_tcb;
}

/**************************************************************************
 * Name:
 *  sched_desroy
 *
 * Description:
 *  Init scheduler resources.
 *
 * Return Value:
 *  OK in case of success otherwise a negate value.
 *************************************************************************/
int sched_desroy(void)
{
  return 0;
}

/**************************************************************************
 * Name:
 *  disable_int
 *
 * Description:
 *  Disable all interrupts.
 *
 *************************************************************************/
void disable_int(void)
{
  __disable_irq();
}

/**************************************************************************
 * Name:
 *  enable_int
 *
 * Description:
 *  Enable all interrupts.
 *
 *************************************************************************/
void enable_int(void)
{
  __enable_irq();
}

