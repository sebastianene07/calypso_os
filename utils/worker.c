#include <board.h>
#include <semaphore.h>
#include <worker.h>

static LIST_HEAD(g_system_workers);
static sem_t g_lock_system_workers;
static volatile uint32_t g_uid_counter;

static int worker_main(int argc, char **argv)
{
  int ret = 0;
  worker_t *worker = (worker_t *)argv;

  while (worker->is_running_enabled)
  {
    worker_cb_t *work_callback;
    list_for_each_entry(work_callback, &work_callback->work_list, work_list) {
    } 
  }

  return ret;
}

int worker_init(void)
{
  g_uid_counter = 0;
  return sem_init(&g_lock_system_workers, 0, 0);
}

int worker_create(int priority, const char *name)
{
  worker_t *new_worker = calloc(1, sizeof(worker_t));
  if (new_worker == NULL) {
    return -ENOMEM;
  }

  sem_init(&new_worker->g_lock_worker_list, 0, 0);

  new_worker->is_running_enabled = true;
  int ret = sched_create_task(worker_main,
                              CONFIG_WORKER_STACK_SIZE,
                              1,
                              (char **)new_worker);
  if (ret != 0) {
    ret = -EINVAL;
    goto free_with_worker;
  }

  sem_wait(&g_lock_system_workers);

  new_worker->workqueue_id = g_uid_counter;

  if (g_uid_counter == 0xFFFFFFFF) {
    ret = -ENOSPC;
    sem_post(&g_lock_system_workers);
    goto free_with_worker;  
  }

  g_uid_counter++;
  list_add(&new_worker->worker_cb, &g_system_workers);

  sem_post(&g_lock_system_workers);

  return new_worker->workqueue_id;

free_with_task:
  new_worker->is_running_enabled = false;

free_with_worker:
  free(new_worker);
  return ret;
}

int worker_enqueue(int worker_id, worker_cb_t *work)
{
  return 0;
}

int worker_cancel_work(int worker_id, int job_uid)
{
  return 0;
}

int worker_destroy(int worker_id)
{
  return 0;
}
