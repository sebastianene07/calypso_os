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

/* Big endian message */
typedef struct pmsa003_msg_s {
  uint16_t start_char;
  uint16_t frame_len;

  /* The below values are in ug/m^3 */
  uint16_t pm1_0;
  uint16_t pm2_5;
  uint16_t pm10;

  uint16_t pm1_0_cu;
  uint16_t pm2_5_cu;
  uint16_t pm10_cu;

  uint16_t num_particles_beyond_0_3_um;  /* in 0.1 L air */
  uint16_t num_particels_beyond_0_5_um;  /* in 0.1 L air */
  uint16_t num_particels_beyond_1_um;    /* in 0.1 L air */
  uint16_t num_particels_beyond_2_5_um;  /* in 0.1 L air */
  uint16_t num_particels_beyond_5_0_um;  /* in 0.1 L air */
  uint16_t num_particels_beyond_1_0_um;  /* in 0.1 L air */
  uint16_t reserved;
  uint16_t crc16;

} __attribute__((packed)) pmsa003_msg_t;

int pmsa_sensor_register(const char *name, struct uart_lower_s *uart_lowerhalf);

#endif /* __PMSA003_H */
