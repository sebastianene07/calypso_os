#ifndef __WORKER_H
#define __WORKER_H

#include <board.h>
#include <errno.h>
#include <list.h>
#include <semaphore.h>
#include <stdbool.h>

/* The work callback */

typedef void (* worker_cb)(void *arg); 

/* The work structure with the callback and the arguments to the callback */

typedef struct worker_cb_s {
  struct list_head work_list;   /* Ordered list of workers */
  uint32_t wake_ms;             /* Planned to run time */
  void *priv_arg;               /* Arguments to the worker callback */
  int work_uid;                 /* Unique work id assigned during enqueue */
  worker_cb callback;           /* Callback to the work to be done */
} worker_cb_t;

/* Worker structure that contains all the work items list */

typedef struct worker_s {
  struct list_head worker_cb;   /* Ordered list of workers */
  int worker_priority;          /* The priority of the worker task */
  const char *worker_name;      /* Worker friendly name */
  uint32_t workqueue_id;        /* The handle of the work queue */
  sem_t g_lock_worker_list;     /* Lock the worker list */
  bool is_running_enabled;      /* Flag that indicates the running status */      
} worker_t;

int worker_init(void);
int worker_create(int priority, const char *name);
int worker_enqueue(int worker_id, worker_cb_t *work);
int worker_cancel_work(int worker_id, int job_uid);
int worker_destroy(int worker_id);

#endif /* __WORKER_H */
