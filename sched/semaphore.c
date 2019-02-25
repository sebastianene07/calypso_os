#include <semaphore.h>
#include <scheduler.h>
#include <stdbool.h>

extern struct list_head g_tcb_list;
extern struct list_head g_tcb_waiting_list;
extern struct list_head *g_current_tcb;

int sem_init(sem_t *sem, int pshared, unsigned int value)
{
  if (sem == NULL)
  {
    return -1;
  }

  sem->count = value;
}

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
    tcb->t_state          = WAITING_FOR_SEM;
    tcb->waiting_tcb_sema = sem;

    /* Switch context to the next running task */

    sched_context_switch();
  }
}

int sem_trywait(sem_t *sem)
{
  return -1;
}

int sem_post(sem_t *sem)
{
  /* Disable context switch */

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

  enable_int();
}
