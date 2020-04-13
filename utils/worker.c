#include <board.h>
#include <semaphore.h>
#include <worker.h>

static LIST_HEAD(g_system_workers);
static sem_t g_lock_system_workers;

int worker_create(int priority, const char *name)
{
  // TODO
  return 0;
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
