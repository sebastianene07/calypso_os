#ifndef __PMSA003_H
#define __PMSA003_H

#include <serial.h>
#include <semaphore.h>

/* Some ioctls to command the sensor */

#define IO_PMSA003_ENTER_IDLE       (0x00)
#define IO_PMSA003_ENTER_NORMAL     (0x01)

/* The size of the packet without start token */
#define PMSA003_DATA_LEN          (32)

typedef struct pmsa003_sensor_s {
  sem_t lock_sensor;
  struct uart_lower_s *interface;
} pmsa003_sensor_t;

int pmsa_sensor_register(const char *name, struct uart_lower_s *uart_lowerhalf);

#endif /* __PMSA003_H */
