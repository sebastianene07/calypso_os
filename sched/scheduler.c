/*
 * scheduler.c
 *
 * Created: 4/6/2018 3:09:38 AM
 *  Author: sene
 */

#include <board.h>

#include <scheduler.h>
#include <semaphore.h>
#include <errno.h>
#include <stdlib.h>
#include <stdbool.h>
#include <vfs.h>
#include <assert.h>

/* Running task list */

LIST_HEAD(g_tcb_list);

/* Waiting for sem task list */

LIST_HEAD(g_tcb_waiting_list);

/* Current running task */

struct list_head *g_current_tcb = NULL;

/* The interrupt vector table */

extern void (*g_vectors[NUM_IRQS])(void);
extern void (*g_ram_vectors[NUM_IRQS])(void);

/**************************************************************************
 * Name:
 *  sched_idle_task
 *
 * Description:
 *  This task is responsible for cleaning up resources used by the tasks. It
 *  can also be used to monitor the HEAP usage, scan for corruptions and
 *  asking the CPU to enter deep sleep mode if no other operation is pending
 *  to be executed.
 *
 * Assumptions:
 *  This task should never exit.
 *
 *************************************************************************/
static int sched_idle_task(int argc, char **argv)
{
  while (1)
  {

    /* Check if we need to free any HALTED tasks */

    disable_int();

    bool is_halt_task;
    do {
      is_halt_task = false;
      struct tcb_s *current = NULL;

      list_for_each_entry(current, &g_tcb_waiting_list, next_tcb)
      {
        if (current != NULL && current->t_state == HALTED)
        {
          list_del(&current->next_tcb);

          /* Does this task have opened resources ? */

          for (int fd = 0; fd < current->curr_resource_opened; fd++) {
            sched_free_resource(fd);
          }

          free(current);
          is_halt_task = true;
          break;
        }
      }
    } while (is_halt_task);

    enable_int();
#ifdef CONFIG_WFI_ENABLE
    __WFI();
#endif
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
 *
 *************************************************************************/
int sched_init(void)
{
  /* Create idle task */

  int ret = sched_create_task(sched_idle_task,
                              CONFIG_SCHEDULER_IDLE_TASK_STACK_SIZE,
                              0,
                              NULL);
  if (ret < 0)
  {
    return ret;
  }

  g_current_tcb = g_tcb_list.next;

  return 0;
}

/**************************************************************************
 * Name:
 *  sched_default_task_exit_point
 *
 * Description:
 *  Called when a task has finished executing the task entry point and it's
 *  about to tear down it's resources. This function should do the cleanup
 *  look for opened file descriptors and release the accessed resources.
 *
 * Assumptions:
 *  This function does not exit.
 *
 *************************************************************************/
void sched_default_task_exit_point(void)
{
  __disable_irq();

  /* Move this task in the HALT state and wait for the idle task to clean up
   * it's memory.
   */

  struct tcb_s *this_tcb = sched_get_current_task();
  this_tcb->t_state           = HALTED;
  this_tcb->waiting_tcb_sema  = NULL;

  /* Switch context to the next running task */

  NVIC_TriggerSysTick();
  sched_context_switch();
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
 *
 *************************************************************************/
int sched_create_task(int (*task_entry_point)(int argc, char **argv),
  uint32_t stack_size, int argc, char **argv)
{
  struct tcb_s *task_tcb = calloc(1, sizeof(struct tcb_s) + stack_size);
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

  int ret = up_initial_task_context(task_tcb);
  if (ret < 0) {
    free(task_tcb);
    return ret;
  }

  /* Init resource list */
  INIT_LIST_HEAD(&task_tcb->opened_resource);

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
  disable_int();
  struct tcb_s *curr_tcb = sched_get_current_task();
  assert(curr_tcb->curr_resource_opened >= 0);

  struct opened_resource_s *new_res =
    calloc(1, sizeof(struct opened_resource_s));
  if (!new_res) {
    enable_int();
    return NULL;
  }

  new_res->fd        = curr_tcb->curr_resource_opened;
  new_res->open_mode = open_mode;
  new_res->vfs_node  = (struct vfs_node_s *)vfs_node;

  curr_tcb->curr_resource_opened++;

  list_add(&new_res->node, &curr_tcb->opened_resource);
  enable_int();

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
  disable_int();

  struct tcb_s *curr_tcb = sched_get_current_task();
  assert(curr_tcb->curr_resource_opened >= 0);

  /* Iterate over opened resource list find the fd and free the resource */

  struct opened_resource_s *resource = sched_find_opened_resource(fd);

  if (resource) {
    list_del(&resource->node);

    free(resource);
    enable_int();
    return OK;
  }

  enable_int();
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
* Return Value:
*  The TCB of the next task or NULL if the scheduler is not initialized.
*
*************************************************************************/
void sched_run(void)
{
  sched_idle_task(0, NULL);
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
*
*************************************************************************/
struct tcb_s *sched_get_current_task(void)
{
  if (g_current_tcb == NULL)
    return NULL;

  struct tcb_s *current_tcb = (struct tcb_s *)container_of(g_current_tcb,
    struct tcb_s, next_tcb);
  return current_tcb;
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
 *
 *************************************************************************/
int sched_desroy(void)
{
  return 0;
}

/**************************************************************************
 * Name:
 *  sched_preempt_task
 *
 * Description:
 *  Move the task from running to waiting list.
 *
 * Return Value:
 *  The task that was moved from running to ready.
 *
 *************************************************************************/
struct tcb_s *sched_preempt_task(void)
{
  struct tcb_s *tcb = sched_get_current_task();

  g_current_tcb = &sched_get_next_task()->next_tcb;

  list_del(&tcb->next_tcb);
  list_add(&tcb->next_tcb, &g_tcb_waiting_list);

  return tcb;
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

/**************************************************************************
 * Name:
 *  attach_int
 *
 * Description:
 *  Attach an interrupt callback. If handler is NULL, the dummy
 *  interrupt handler should be installed and the function should
 *  act as detach.
 *
 *************************************************************************/
void attach_int(IRQn_Type irq_num, irq_cb handler)
{
  g_ram_vectors[irq_num] = handler;
}
