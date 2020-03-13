#include <board.h>

#include <errno.h>
#include <semaphore.h>
#include <scheduler.h>
#include <stdbool.h>

extern struct list_head g_tcb_list;
extern struct list_head g_tcb_waiting_list;
extern struct list_head *g_current_tcb;

/* Extern function implemented by the context switch mechanism. This method
 * suspends the execution for the current process saves it's context on the
 * stack and triggers an interrupt to begin the context switch.
 */

void sched_context_switch(void);

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
  /* Disable context switch */

  disable_int();

  /* Verify the semaphore value and decrement it if it's > 0 */

  if (sem->count > 0)
  {
    sem->count--;
  }
  else
  {
    /* Place the current task in the waiting list and remove the task from the
     * running queue.
     */

    struct tcb_s *tcb     = sched_get_current_task();

    /* There was an error or tasks were not initialized.
     * If there is only one task in the ready to run list don't change
     * the state to WAITING_FOR_SEM. Instead, we should return an error
     * like EAGAIN.
     */


    if ((tcb == NULL) ||
        (g_current_tcb->next == g_current_tcb->prev)) {
      enable_int();
      return -EAGAIN;
    }

    tcb->t_state          = WAITING_FOR_SEM;
    tcb->waiting_tcb_sema = sem;

    /* Switch context to the next running task */

    sched_context_switch();
    enable_int();

    return 0;
  }

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

  disable_int();

  sem->count += 1;

  if (sem->count == 1)
  {
    /* Unblock task from waiting list that is blocked by this semaphore and
     * place it in the running list
     */
    bool is_waiting_for_sem;

    do {
          is_waiting_for_sem = false;
          struct tcb_s *current = NULL;

          list_for_each_entry(current, &g_tcb_waiting_list, next_tcb)
          {
            if (current != NULL && current->waiting_tcb_sema == sem)
            {
              current->waiting_tcb_sema = NULL;
              current->t_state          = READY;

              list_del(&current->next_tcb);
              list_add(&current->next_tcb, &g_tcb_list);
              is_waiting_for_sem = true;
              break;
            }
          }
      } while (is_waiting_for_sem);
  }

  /* Re-enable interrupts for the current task */

  enable_int();

  return 0;
}
