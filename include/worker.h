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

#ifndef __WORKER_H
#define __WORKER_H

#include <board.h>
#include <errno.h>
#include <list.h>
#include <semaphore.h>
#include <stdbool.h>

/****************************************************************************
 * Macros Defintions
 ****************************************************************************/

#ifndef INT_MAX
  # define INT_MAX (2147483647)
#endif

/****************************************************************************
 * Public Types
 ****************************************************************************/

/* The work callback */

typedef void (* worker_cb)(void *arg); 

/* The function that cleans up the arguments to the worker callback */

typedef void (* worker_cleanup_cb)(void *arg);

/* The work structure with the callback and the arguments to the callback */

typedef struct worker_cb_s {
  struct list_head work_list;   /* Ordered list of workers */
  uint32_t wake_ms;             /* Planned to run time */
  void *priv_arg;               /* Arguments to the worker callback */
  int work_uid;                 /* Unique work id assigned during enqueue */
  worker_cb callback;           /* Callback to the work to be done */
  worker_cleanup_cb cleanup_cb; /* Clean up the arguments passed to callback */
} worker_cb_t;

/* Worker structure that contains all the work items list */

typedef struct worker_s {
  struct list_head worker_cb;   /* Ordered list of workers */
  int worker_priority;          /* The priority of the worker task */
  const char *worker_name;      /* Worker friendly name */
  int workqueue_id;             /* The handle of the work queue */
  sem_t g_lock_worker_list;     /* Lock the worker list */
  bool is_running_enabled;      /* Flag that indicates the running status */      
  sem_t shutdown_notify;        /* Notification semaphore for thread shutdown */
} worker_t;

/****************************************************************************
 * Public Prototypes
 ****************************************************************************/

int worker_init(void);
int worker_create(int priority, const char *name);
int worker_enqueue(int worker_id, worker_cb_t *work);
int worker_cancel_work(int worker_id, int job_uid);
int worker_destroy(int worker_id);

#endif /* __WORKER_H */
