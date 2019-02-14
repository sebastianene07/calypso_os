#ifndef __SEMAPHORE_H
#define __SEMAPHORE_H

#include <scheduler.h>

typedef struct sem_s {
  int count;
} sem_t;

int sem_init(sem_t *sem, int pshared, unsigned int value);

int sem_wait(sem_t *sem);

int sem_trywait(sem_t *sem);

#if 0 /* NOT IMPLEMENTED */

int sem_timedwait(sem_t *sem, const struct timespec *abs_timeout);

#endif

int sem_post(sem_t *sem);

#endif /* __SEMAPHORE_H */
