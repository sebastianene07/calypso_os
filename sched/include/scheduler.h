/*
 * scheduler.h
 *
 * Created: 4/6/2018 3:04:40 AM
 *  Author: sene
 */


#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include <stdint.h>
#include <list.h>
#include <semaphore.h>

/* Fill the stack with 0xDEADBEEF */

#define CONFIG_SCHEDULER_TASK_COLORATION      (1)

/* The idle task stack size */

#define CONFIG_SCHEDULER_IDLE_TASK_STACK_SIZE (1024)

/* Used when we start a task */

#define MCU_CONTEXT_SIZE                      (8)

/* The interrupt callback type */

typedef void (* irq_cb)(void);

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

struct tcb_s {
  void (*entry_point)(void);
  enum task_state_e t_state;
  void *stack_ptr_base;
  void *stack_ptr_top;
  void *sp;
  void *mcu_context[MCU_CONTEXT_SIZE];
  struct list_head next_tcb;
  sem_t *waiting_tcb_sema;
  struct list_head opened_resource;
  uint32_t curr_resource_opened;
} __attribute__((aligned(4)));


int sched_init(void);

int sched_create_task(void (*task_entry_point)(void), uint32_t stack_size);

void sched_run(void);

struct tcb_s *sched_get_current_task(void);

struct tcb_s *sched_get_next_task(void);

int sched_desroy(void);

void disable_int(void);

void enable_int(void);

void attach_int(IRQn_Type irq_num, irq_cb handler);

void sched_context_switch(void);

struct opened_resource_s *sched_allocate_resource(const struct vfs_node_s *node,
                                                  int open_mode);
int sched_free_resource(int fd);

struct opened_resource_s *sched_find_opened_resource(int fd);

#endif /* SCHEDULER_H_ */
