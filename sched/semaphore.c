#include <semaphore.h>
#include <scheduler.h>

extern struct list_head g_tcb_waiting_list;

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

    struct tcb_s *tcb = sched_get_current_task();
    tcb->t_state = WAITING_FOR_SEM;

    list_del(&tcb->next_tcb);
    list_add(&tcb->next_tcb, &g_tcb_waiting_list);

    /* Switch context to the next running task */


  }

  enable_int();
}

int sem_trywait(sem_t *sem)
{
}
