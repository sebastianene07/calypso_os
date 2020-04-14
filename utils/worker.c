/*
 * This file is part of the Calypso OS distribution
 * https://github.com/calypso_os.
 *
 * Copyright (c) 2020 Sebastian Ene.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <board.h>
#include <semaphore.h>
#include <string.h>
#include <worker.h>

/****************************************************************************
 * Private Data
 ****************************************************************************/

/* System workers list head */

static LIST_HEAD(g_system_workers);

/* Exclusive access to the system workers list */

static sem_t g_lock_system_workers;

/* A counter used to identify the system worker thread */

static volatile int g_uid_counter;

/****************************************************************************
 * Private Methods
 ****************************************************************************/

/**
 * worker_main - entry point for the worker thread
 *
 * @argc: the number of argumentsd
 * @argv: arguments array
 *
 * Iterate through the worker callbacks and execute them. Pass the required
 * arguments.
 *
 * Returns 0 (OK) on successful completion otherwise a negative error code.
 */
static int worker_main(int argc, char **argv)
{
  int ret = 0;
  worker_t *worker = (worker_t *)argv;

  while (worker->is_running_enabled)
  {
    sem_wait(&worker->g_lock_worker_list);

    worker_cb_t *work_callback = NULL;
    list_for_each_entry(work_callback, &work_callback->work_list, work_list) {
      // TODO
    }

    sem_post(&worker->g_lock_worker_list);
  }

  sem_post(&worker->shutdown_notify);

  return ret;
}

/**
 * worker_get - get thw worker from the worker id
 *
 * @worker_id - the unique identifier for a worker
*
 * Assumption: lock with g_lock_system_workers before calling this function
 */
static worker_t *worker_get(int worker_id)
{
  worker_t *worker = NULL;
  list_for_each_entry(worker, &g_system_workers, worker_cb) {

    if (worker->workqueue_id == worker_id) {
      return worker;
    }
  }

  return NULL;
}

/****************************************************************************
 * Public Methods
 ****************************************************************************/

/**
 * worker_init - initialize the worker thred resources
 */
int worker_init(void)
{
  g_uid_counter = 0;
  return sem_init(&g_lock_system_workers, 0, 0);
}

/**
 * worker_create - create a new worker thread with the specified priorityd
 *
 * @priority: the prioroty of the threadd
 * @name: friendly name of the worker thread
 *
 * Create a new worker thread and return the handle to the worker thread on
 * success.
 *
 * Assumption: priority is ignored if the scheduler is FIFO without priority
 * scheduling.
 */
int worker_create(int priority, const char *name)
{
  worker_t *new_worker = calloc(1, sizeof(worker_t));
  if (new_worker == NULL) {
    return -ENOMEM;
  }

  sem_init(&new_worker->g_lock_worker_list, 0, 0);
  sem_init(&new_worker->shutdown_notify, 0, 0);

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

  if (g_uid_counter == INT_MAX) {
    ret = -ENOSPC;
    sem_post(&g_lock_system_workers);
    goto free_with_task;
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

/**
 * worker_enqueue - enqueue a work structure in the specified worker
 *
 * @worker_id: the id of the worker threadd
 * @work: the work structure
 *
 * Create a new worker thread and return the handle to the worker thread on
 * success.
 *
 * Assumption: priority is ignored if the scheduler is FIFO without priority
 * scheduling.
 */
int worker_enqueue(int worker_id, worker_cb_t *work)
{
  sem_wait(&g_lock_system_workers);

  /* Get the system worker from the worker_id */

  worker_t *worker = worker_get(worker_id);
  if (worker == NULL) {
    sem_post(&g_lock_system_workers);
    return -EINVAL;
  }

  sem_post(&g_lock_system_workers);

  /* Create a copy of the object */

  worker_cb_t *work_copy = calloc(1, sizeof(worker_cb_t));
  if (work_copy == NULL) {
    return -ENOMEM;
  }

  memcpy(work_copy, work, sizeof(worker_cb_t));

  /* Find a place where to insert the work item */

  sem_wait(&worker->g_lock_worker_list);

  struct list_head *pos, *temp;
  list_for_each_safe(pos, temp, &worker->worker_cb) {
    worker_cb_t *current_work = container_of(pos, worker_cb_t, work_list);

    /* Work that need to be done first is placed at the beggining of the list
     */

    if (current_work->wake_ms > work_copy->wake_ms) {
      list_del(pos);
      list_add(&work_copy->work_list, &worker->worker_cb);
      list_add(pos, &worker->worker_cb);

      sem_post(&worker->g_lock_worker_list);
      return 0;
    }
  }

  list_add(&work_copy->work_list, &worker->worker_cb);
  sem_post(&worker->g_lock_worker_list);

  return 0;
}

/**
 * worker_cancel_work - cancel work that was enqueued on a worker threadr
 *
 * @worker_id: the id of the worker threadd
 * @job_uid: the job that we wish to cancele
 *
 * Cancels a job that was enqueued on a worker thread. This function does not
 * guarantee that the cancellation will occur because a work item can be 
 * scheduled to run by the time we call this function.
 *
 * Returns 0(OK) in case the job was cancelled otherwise -EINVAL.
 */
int worker_cancel_work(int worker_id, int job_uid)
{
  sem_wait(&g_lock_system_workers);

  /* Get the system worker from the worker_id */

  worker_t *worker = worker_get(worker_id);
  if (worker == NULL) {
    sem_post(&g_lock_system_workers);
    return -EINVAL;
  }

  sem_post(&g_lock_system_workers);

  sem_wait(&worker->g_lock_worker_list);

  struct list_head *pos, *temp;
  list_for_each_safe(pos, temp, &worker->worker_cb) {
    worker_cb_t *current_work = container_of(pos, worker_cb_t, work_list);

    /* If we found the job unique identifier call the clean up callback
     * remove the work item from the list and free the work item.
     */

    if (current_work->work_uid == job_uid) {
      list_del(pos);
      if (current_work->cleanup_cb) {
        current_work->cleanup_cb(current_work->priv_arg);
      }

      free(current_work);

      sem_post(&worker->g_lock_worker_list);
      return 0;
    }
  }

  sem_post(&worker->g_lock_worker_list);

  return -EINVAL;
}

/**
 * worker_destroy - destroy a worker with all the associated resources
 *
 * @worker_id: the id of the worker threadd
 *
 * Cancels a job that was enqueued on a worker thread. This function does not
 * guarantee that the cancellation will occur because a work item can be 
 * scheduled to run by the time we call this function.
 *
 */
int worker_destroy(int worker_id)
{
  sem_wait(&g_lock_system_workers);

  /* Get the system worker from the worker_id */

  worker_t *worker = worker_get(worker_id);
  if (worker == NULL) {
    sem_post(&g_lock_system_workers);
    return -EINVAL;
  }

  sem_post(&g_lock_system_workers);

  /* Shutdown the worker task */

  worker->is_running_enabled = false;
  sem_wait(&worker->shutdown_notify);

  /* Free the resources - CAUTIO
   *
   * If there are work items that have arguments allocated on the heap
   * freeing the work item may cause leaking the arguments if no cleanup
   * callback was assigned to the work item.
   */

  struct list_head *pos, *temp;
  list_for_each_safe(pos, temp, &worker->worker_cb) {
    worker_cb_t *current_work = container_of(pos, worker_cb_t, work_list);
    list_del(pos);

    if (current_work->cleanup_cb) {
      current_work->cleanup_cb(current_work->priv_arg);
    }

    free(current_work);
  }

  return 0;
}
