#include <board.h>

#include <assert.h>
#include <errno.h>
#include <semaphore.h>
#include <scheduler.h>
#include <stdbool.h>

extern struct list_head g_tcb_list;
extern struct list_head g_tcb_waiting_list;
extern struct list_head *g_current_tcb;

/*
 * sem_init - initialize the semaphore
 *
 * @sem       - the semaphore address
 * @pshared   - ignored
 * @value     - the semaphore initial value
 *
 * Initialize the semaphore with a specified value.
 *
 */
int sem_init(sem_t *sem, int pshared, unsigned int value)
{
  if (sem == NULL)
  {
    return -1;
  }

  sem->count = value;

  return 0;
}

/*
 * sem_wait - decrement the semaphore
 *
 * @sem       - the semaphore address
 *
 * The call of this function can suspend the current execution if the semaphore
 * value is less than or equal to 0. Use this primitive for signalling purpose
 * or for locking implementation.
 *
 */
int sem_wait(sem_t *sem)
{
  /* Disable context switch by disabling interrupts */

  irq_state_t irq_state = cpu_disableint();

  /* Verify the semaphore value and decrement it if it's > 0 */

  if (sem->count > 0)
  {
    sem->count--;
  }
  else
  {
    struct tcb_s *tcb     = sched_get_current_task();

    /* There was an error or tasks were not initialized.
     * If there is only one task in the ready to run list don't change
     * the state to WAITING_FOR_SEM. Instead, we should return an error
     * like EAGAIN.
     */

    assert(!((tcb->t_state == WAITING_FOR_SEM) ||
           (tcb->t_state == HALTED)));

    if ((tcb == NULL) || (g_current_tcb->next == g_current_tcb->prev))
    {
      cpu_enableint(irq_state);
      return -EAGAIN;
    }

    tcb->t_state          = WAITING_FOR_SEM;
    tcb->waiting_tcb_sema = sem;

    SCHED_DEBUG_INFO("%s WAIT for sema\n", tcb->task_name);

    /* Switch context to the next running task */

    cpu_enableint(irq_state);

    /* Place the current task in the waiting list and remove the task from the
     * running queue. Activate a new task that is prepared to be run.
     */

    sched_preempt_task(tcb);
    return 0;
  }

  /* Re-enable interrupts */

  cpu_enableint(irq_state);
  return 0;
}

/*
 * sem_trywait - decrement the semaphore
 *
 * @sem       - the semaphore address
 *
 * Not implemneted yet.
 *
 */
int sem_trywait(sem_t *sem)
{
  return -1;
}

/*
 * sem_post - increment the value of the semaphore
 *
 * @sem       - the semaphore address
 *
 * This function increments the semaphore value and unblocks all the tasks
 * that are holding it.
 */
int sem_post(sem_t *sem)
{
  /* Disable interrupts for this task */

  irq_state_t irq_state = cpu_disableint();

  sem->count += 1;

  if (sem->count == 1)
  {
    /* Unblock task from waiting list that is blocked by this semaphore and
     * place it in the running list
     */

    struct tcb_s *current = NULL;

    list_for_each_entry(current, &g_tcb_waiting_list, next_tcb)
    {
      if (current != NULL && current->waiting_tcb_sema == sem)
      {
        /* If the task is not in the waiting state then somwthing went
         * wrong.
         */

        assert(current->t_state == WAITING_FOR_SEM); 

        /* Move the task state to ready and remove the semaphore from */

        current->waiting_tcb_sema = NULL;
        current->t_state          = READY;

        SCHED_DEBUG_INFO("%s received POST sema\n", current->task_name);

        /* Let the sched_preempt_task function Delete the task from the
         * waiting list and add it to the ready to run list in front of other
         * tasks.
         */
        break;
      }
    }
  }

  /* Re-enable interrupts for the current task */

  cpu_enableint(irq_state);

  return 0;
}

/*
 * sem_timedwait - wait on a semaphore with timeoute
 *
 * @sem       - the semaphore address
 *
 * Not implemneted yet. It only works with SEM_WAIT_FOREVER.
 *
 */
int sem_timedwait(sem_t *sem, int timeout_abs)
{
  if (timeout_abs == SEM_WAIT_FOREVER)
  {
    return sem_wait(sem);
  }
  else
  {
    return -ENOSYS;
  }
}
