#ifndef __SEMAPHORE_H
#define __SEMAPHORE_H

typedef struct sem_s {
  volatile int count;
} sem_t;

int sem_init(sem_t *sem, int pshared, unsigned int value);

int sem_wait(sem_t *sem);

int sem_trywait(sem_t *sem);

int sem_post(sem_t *sem);

#endif /* __SEMAPHORE_H */
