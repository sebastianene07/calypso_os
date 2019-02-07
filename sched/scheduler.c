/*
 * scheduler.c
 *
 * Created: 4/6/2018 3:09:38 AM
 *  Author: sene
 */

#include <scheduler.h>
#include <errno.h>
#include <stdlib.h>
#include <list.h>

/* Current running task */

static volatile uint32_t g_task_index;

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

  return 0;
}

/**************************************************************************
 * Name:
 *  sched_create_task
 *
 * Description:
 *  Create a new task.
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
  task_tcb->stack_ptr_base = task_tcb + sizeof(struct tcb_s);
  task_tcb->stack_ptr_top  = task_tcb + stack_size;
  task_tcb->t_state        = READY;

#ifdef CONFIG_SCHEDULER_TASK_COLORATION
  /* The effective stack size is base - top */

  for (uint32_t *ptr = task_tcb->stack_ptr_base;
     ptr < (uint32_t*)task_tcb->stack_ptr_top;
     ptr++) {
       *ptr = 0xDEADBEEF;
     }
#endif

  /* Initial MCU context */

  task_tcb->mcu_context[0] = NULL;
  task_tcb->mcu_context[1] = NULL;
  task_tcb->mcu_context[2] = NULL;
  task_tcb->mcu_context[3] = NULL;
  task_tcb->mcu_context[4] = NULL;
  task_tcb->mcu_context[5] = NULL;//sched_default_task_exit_point;
  task_tcb->mcu_context[6] = task_entry_point;
  task_tcb->mcu_context[7] = (uint32_t *)0x1000000;

  /* Stack context in interrupt */

  int i = 0;
  void *ptr_after_int = task_tcb->stack_ptr_top -
    sizeof(void *) * MCU_CONTEXT_SIZE;

  for (uint32_t *ptr = ptr_after_int;
     ptr < (uint32_t *)task_tcb->stack_ptr_top;
     ptr++)
  {
    *ptr = (uint32_t)task_tcb->mcu_context[i++];
  }

  task_tcb->sp = ptr_after_int;

  /* TODO: Insert the task in the list */

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
  if (g_task_num == 0)
  {
    return NULL;
  }

  if (g_task_index == g_task_num - 1)
  {
    g_task_index = 0;
  }

  g_task_index++;

  return g_tasks[g_task_index];
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
  if (g_task_num > g_task_index)
    return g_tasks[g_task_index];
  else
    return NULL;
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
