/*
 * scheduler.h
 *
 * Created: 4/6/2018 3:04:40 AM
 *  Author: sene
 */


#ifndef __SCHEDULER_H
#define __SCHEDULER_H

#include <board.h>

#include <list.h>
#include <stdint.h>
#include <semaphore.h>
#include <stdio.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* Scheduler debug macro */

#ifndef CONFIG_SCHEDULER_DEBUG
  #define SCHED_DEBUG_INFO(msg, ...)
#else
  #define SCHED_DEBUG_INFO(msg, ...)  printf("[SCHED] "msg, __VA_ARGS__)
#endif

/****************************************************************************
 * Public Types
 ****************************************************************************/

struct opened_resource_s {
  int open_mode;            /* Currently not used */
  int fd;                   /* OPened resources file descriptor */
  struct vfs_node_s *vfs_node;  /* The node from the virtual file system */
  struct list_head node;    /* The list of the opened resources ina task */
};

/* The task can be in one of the following states */

enum task_state_e {
  READY,            /* It is not currenly running on the CPU */
  RUNNING,          /* The task was plannend on the CPU and is running */
  WAITING_FOR_SEM,  /* The task is waiting for an event to wake up */
  HALTED            /* NOT USED currently */
};

/* Task container that holds the entry point and other resources */

typedef struct tcb_s {
  struct list_head next_tcb;        /* The task list          */
  int (*entry_point)(int, char **); /* The task entry point   */
  enum task_state_e t_state;        /* The task state         */
  void *stack_ptr_base;             /* Bootom stack pointer      */
  void *stack_ptr_top;              /* Top stack pointer         */
  void *sp;                         /* Current stack pointer     */
  void *mcu_context;                /* The CPU context           */
  sem_t *waiting_tcb_sema;          /* The waiting semaphore     */
  struct list_head opened_resource; /* Opened task resources     */
  uint32_t curr_resource_opened;    /* Num of opened resources   */
  const char task_name[CONFIG_TASK_NAME_LEN];
} tcb_t __attribute__((aligned(16)));

/****************************************************************************
 * Public Scheduler Functions 
 ****************************************************************************/

int sched_init(void);

int sched_create_task(int (*task_entry_point)(int argc, char **argv),
                      uint32_t stack_size,
                      int argc,
                      char **argv,
                      const char *task_name);

void sched_run(void);

struct tcb_s *sched_get_current_task(void);

struct tcb_s *sched_get_next_task(void);

struct tcb_s *sched_preempt_task(tcb_t *to_preempt_tcb);

void sched_context_switch(void);

void sched_default_task_exit_point(void);

struct opened_resource_s *sched_allocate_resource(const struct vfs_node_s *node,
                                                  int open_mode);
int sched_free_resource(int fd);

struct opened_resource_s *sched_find_opened_resource(int fd);

#endif /* __SCHEDULER_H */
