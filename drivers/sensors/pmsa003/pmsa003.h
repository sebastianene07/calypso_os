#ifndef __PMSA003_H
#define __PMSA003_H

#include <serial.h>
#include <semaphore.h>

typedef struct pmsa003_sensor_s {
  sem_t lock_sensor;
  struct uart_lower_s *interface;
} pmsa003_sensor_t;

#endif /* __PMSA003_H */
