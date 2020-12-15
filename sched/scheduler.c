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
#include <stdbool.h>
#include <vfs.h>
#include <assert.h>
#include <string.h>

/****************************************************************************
 * Public variables defintion
 ****************************************************************************/

/* This list holds the ready to run and the runing task */

LIST_HEAD(g_tcb_list);

/* The list holds the waiting for semaphore and the halted tasks */

LIST_HEAD(g_tcb_waiting_list);

/* The current running task */

struct list_head *g_current_tcb = NULL;

/**************************************************************************
 * Name:
 *  sched_idle_task
 *
 * Description:
 *  This task is responsible for cleaning up resources used by the tasks
 *  and to put the board to sleep if CONFIG_BOARD_SLEEP is enabled.
 *  to be executed.
 *
 * Assumptions:
 *  This task should never exit.
 *
 *************************************************************************/

static int sched_idle_task(int argc, char **argv)
{
  tcb_t *current_tcb;
  struct list_head *current, *temp;
  int fd;

  current_tcb = sched_get_current_task();
  SCHED_DEBUG_INFO("[%s] entry point\n", current_tcb->task_name);

  while (1)
  {
    irq_state_t irq_mask = cpu_disableint();

    /* Check if we need to free any HALTED tasks */

    list_for_each_safe(current, temp, &g_tcb_waiting_list)
    {
      if (current == NULL)
        continue;

      current_tcb = container_of(current, tcb_t, next_tcb);
      if (current_tcb == NULL ||
          current_tcb->t_state != HALTED)
      {
        continue;
      }

      SCHED_DEBUG_INFO("[idle_task] tear down task %s\n", current_tcb->task_name);

      /* Remove the task from the waiting list if it's in HALTED state */

      list_del(current);

      /* Close the opened resources */

      for (fd = 0; fd < current_tcb->curr_resource_opened; fd++)
      {
        sched_free_resource(fd);
      }

      /* Tear down the task context */

      cpu_destroytask(current_tcb);
    }

    cpu_enableint(irq_mask);

    /* Run the scheduler */

    sched_run();

#ifdef CONFIG_BOARD_SLEEP

    /* Put the board in sleep */

    board_entersleep();
#endif
  }

  return 0;
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
 *
 *************************************************************************/

int sched_init(void)
{
  int ret = sched_create_task(sched_idle_task,
                              CONFIG_SCHEDULER_IDLE_TASK_STACK_SIZE,
                              0,
                              NULL,
                              "Idle");
  if (ret < 0)
  {
    SCHED_ERROR("failed to create Idle task %d\n", ret);
    return ret;
  }

  return 0;
}

/**************************************************************************
 * Name:
 *  sched_default_task_exit_point
 *
 * Description:
 *  Called when a task has finished executing the task entry point and returns
 *
 * Assumptions:
 *  This function does not exit.
 *
 *************************************************************************/

void sched_default_task_exit_point(void)
{
  irq_state_t irq_state = cpu_disableint();

  /* Move this task in the HALT state and wait for the idle task to clean up
   * it's memory.
   */

  tcb_t *this_tcb             = sched_get_current_task();
  this_tcb->t_state           = HALTED;
  this_tcb->waiting_tcb_sema  = NULL;

  cpu_enableint(irq_state);

  /* Switch context to the next running task */

  sched_preempt_task(this_tcb);
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
 *  argc             - the number of arguments
 *  argv             - the task arguments
 *  task_name        - a NULL terminated string representing the task name
 *
 * Assumptions:
 *  Call this function with interrupts disabled.
 *
 * Return Value:
 *  OK in case of success otherwise a negate value.
 *
 *************************************************************************/

int sched_create_task(int (*task_entry_point)(int argc, char **argv),
                      uint32_t stack_size,
                      int argc,
                      char **argv,
                      const char *task_name)
{
  irq_state_t irq_state = cpu_disableint();
  int ret;

  SCHED_DEBUG_INFO("try create task %s\n", task_name);

  struct tcb_s *task_tcb = calloc(1, sizeof(struct tcb_s) + stack_size);
  if (task_tcb == NULL)
  {
    ret = -ENOMEM;
    goto failed_task_creation;
  }

  task_tcb->entry_point    = task_entry_point;
  task_tcb->stack_ptr_base = (void *)task_tcb + sizeof(struct tcb_s);
  task_tcb->stack_ptr_top  = (void *)task_tcb + stack_size + sizeof(struct tcb_s);
  task_tcb->t_state        = READY;

  if (task_name != NULL)
  {
    strncpy((char *)task_tcb->task_name, task_name, strlen(task_name));
  }

#ifdef CONFIG_SCHEDULER_TASK_COLORATION
  /* The effective stack size is base - top */

  for (uint8_t *ptr = task_tcb->stack_ptr_base;
     ptr < (uint8_t*)task_tcb->stack_ptr_top;
     ptr = ptr + sizeof(uint32_t))
  {
    *((uint32_t *)ptr) = 0xDEADBEEF;
  }
#endif

  ret = cpu_inittask(task_tcb, argc, argv);
  if (ret < 0) {
    free(task_tcb);
    goto failed_task_creation;
  }

  /* Init resource list */

  INIT_LIST_HEAD(&task_tcb->opened_resource);

  /* Insert the task in the list */

  list_add(&task_tcb->next_tcb, &g_tcb_list);
  
  ret = OK;

  SCHED_DEBUG_INFO("created task %s\n", task_name);

failed_task_creation:
  cpu_enableint(irq_state);
  return ret;
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
*
*************************************************************************/

struct tcb_s *sched_get_next_task(void)
{
  g_current_tcb = g_current_tcb->next;
  if (g_current_tcb == &g_tcb_list)
    {
      g_current_tcb = g_current_tcb->next;
    }

  struct tcb_s *next_tcb = (struct tcb_s *)container_of(g_current_tcb,
                                                        struct tcb_s,
                                                        next_tcb);
  return next_tcb;
}

/**************************************************************************
* Name:
* sched_allocate_resource
*
* Description:
*  Allocate a new resource and store the file desscriptor in the resource
*  structure.
*
* Return Value:
*  The opened container resource or NULL in case we are running out of memory.
*
*************************************************************************/

struct opened_resource_s *sched_allocate_resource(const struct vfs_node_s *vfs_node,
                                                  int open_mode)
{
  irq_state_t irq_state = cpu_disableint();
  struct tcb_s *curr_tcb = sched_get_current_task();
  assert(curr_tcb->curr_resource_opened >= 0);

  struct opened_resource_s *new_res = calloc(1,
      sizeof(struct opened_resource_s));
  if (!new_res) {
    cpu_enableint(irq_state);
    return NULL;
  }

  new_res->fd        = curr_tcb->curr_resource_opened;
  new_res->open_mode = open_mode;
  new_res->vfs_node  = (struct vfs_node_s *)vfs_node;

  curr_tcb->curr_resource_opened++;

  list_add(&new_res->node, &curr_tcb->opened_resource);
  cpu_enableint(irq_state);

  return new_res;
}

/**************************************************************************
 * Name:
 *  sched_free_resource
 *
 * Description:
 *  This method tears down the resources allocated for an object that has been
 *  opened. It should be called from close() context.
 *
 * Assumptions:
 *  Call this function with pre-emption disabled.
 *
 * Return Value:
 *  OK or a negative value on failure.
 *
 *************************************************************************/

int sched_free_resource(int fd)
{
  irq_state_t irq_state = cpu_disableint();

  struct tcb_s *curr_tcb = sched_get_current_task();
  assert(curr_tcb->curr_resource_opened >= 0);

  /* Iterate over opened resource list find the fd and free the resource */

  struct opened_resource_s *resource = sched_find_opened_resource(fd);

  if (resource) {
    list_del(&resource->node);

    free(resource);
    cpu_enableint(irq_state);
    return OK;
  }

  cpu_enableint(irq_state);
  return -ENOENT;
}

/**************************************************************************
 * Name:
 *  sched_find_opened_resource
 *
 * Description:
 *  Search for an opened resource with specified fd number in the current tcb.
 *
 * Assumptions:
 *  Call this function with pre-emption disabled.
 *
 * Return Value:
 *  The opened container resource or NULL in case we are running out of memory.
 *
 *************************************************************************/

struct opened_resource_s *sched_find_opened_resource(int fd)
{
  struct tcb_s *curr_tcb = sched_get_current_task();
  assert(curr_tcb->curr_resource_opened >= 0);

  struct opened_resource_s *resource = NULL;
  list_for_each_entry(resource, &curr_tcb->opened_resource, node)
  {
    if (resource && resource->fd == fd) {
      return resource;
    }
  }

  return NULL;
}

/**************************************************************************
* Name:
* sched_run
*
* Description:
*  Pick the next task to be run.
*
*************************************************************************/

void sched_run(void)
{
  irq_state_t irq_mask = cpu_disableint();

  tcb_t *current_task = sched_get_current_task();
  if (g_current_tcb == NULL)
  {
    /* In the initial phase there is no task, it is only the __start
     * entry point which is called after reset.
     */

    g_current_tcb = g_tcb_list.next;
    current_task = container_of(g_current_tcb, tcb_t, next_tcb);

    /* Switch the task state to running */

    current_task->t_state = RUNNING;

    /* Re-enable the interrupts */

    cpu_enableint(irq_mask);

    /* Set the context to the new task */

    cpu_restorecontext(current_task->mcu_context);
    return;
  }

  /* Place the current task in the ready state and let others run  */

  current_task->t_state = READY;

  cpu_enableint(irq_mask);
  sched_preempt_task(current_task);
}

/**************************************************************************
* Name:
*  sched_get_current_task
*
* Description:
*  Get current runing task.
*
* Return Value:
*  The TCB of the next task or NULL if the scheduler is not initialized.
*
* Assumptions:
*  This function is safe to be called when we have context switching off.
*
*************************************************************************/

tcb_t *sched_get_current_task(void)
{
  if (g_current_tcb == NULL)
    return NULL;

  return (tcb_t *)container_of(g_current_tcb, tcb_t, next_tcb);
}

/**************************************************************************
 * Name:
 *  sched_preempt_task
 *
 * Description:
 *  Move the task from running to waiting list and activate the next task.
 *
 * Input Arguments:
 *  to_preempt_tcb - the task to be preempted
 *
 * Assumptions:
 *  The task received as argument should not be in the RUNNING state.
 *
 ************************************************************************/

void sched_preempt_task(tcb_t *to_preempt_tcb)
{
  tcb_t *new_tcb = NULL;
  struct list_head *current, *temp;
  irq_state_t irq_state = cpu_disableint();

  /* Is there any task in the waiting list that needs to be added back
   * in the ready list ?
   */

  list_for_each_safe(current, temp, &g_tcb_waiting_list)
  {
    new_tcb = container_of(current, tcb_t, next_tcb);
    if (new_tcb && new_tcb->t_state == READY)
    {
      list_del(current);
      list_add(current, &g_tcb_list);
    }
  }

  /* If the task to preempt is not in :
   * READY, WAITING_FOR_SEM or HALTED
   * something is wrong.
   */

  assert(to_preempt_tcb->t_state != RUNNING);

  /* Delete the task from the current list */

  list_del(&to_preempt_tcb->next_tcb);

  if (to_preempt_tcb->t_state == READY)
  {
    /* The task slot expired we need to scheduler a new task.
     * Place the task at the end of the list.
     */

    SCHED_DEBUG_INFO("%s preempted\n", to_preempt_tcb->task_name);
    list_add_tail(&to_preempt_tcb->next_tcb, &g_tcb_list);
  }
  else if (to_preempt_tcb->t_state == WAITING_FOR_SEM ||
           to_preempt_tcb->t_state == HALTED)
  {
     /* The task is preempted because it was blocked by a semaphore
      * or it was done executing the entry point and it needs to be halted.
      */

    list_add(&to_preempt_tcb->next_tcb, &g_tcb_waiting_list);
  }

  /* Save the current context in the task that needs to be preempted */

  volatile int is_context_restored = cpu_savecontext(to_preempt_tcb->mcu_context);
  if (is_context_restored)
  {
    SCHED_DEBUG_INFO("%s restored context\n", to_preempt_tcb->task_name);
    return;
  }

  SCHED_DEBUG_INFO("%s saved context\n", to_preempt_tcb->task_name);
  list_for_each_entry(new_tcb, &g_tcb_list, next_tcb)
  {
    /* All the tasks from this list should be in the READY state */

    assert(new_tcb->t_state == READY);

    /* Great, we found a task that it is the ready state.
     * Move the task in the runing state and restore its context.
     */

    new_tcb->t_state = RUNNING;

    g_current_tcb = &new_tcb->next_tcb;
    SCHED_DEBUG_INFO("%s now run\n", new_tcb->task_name);

    /* Re-enable the interrupts */

    cpu_enableint(irq_state);

    /* Switch the context to the new task */

    cpu_restorecontext(new_tcb->mcu_context);
  }

  cpu_enableint(irq_state);
}
