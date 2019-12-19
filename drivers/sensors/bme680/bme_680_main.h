#ifndef __BME_680_MAIN_H
#define __BME_680_MAIN_H

#include <spi.h>
#include <semaphore.h>

#include "bme680.h"

typedef struct bme680_sensor_s {
  sem_t lock_sensor; 
  spi_master_dev_t interface; 
  struct bme680_dev dev; 
} bme680_sensor_t;


int bme680_sensor_register(const char *name, spi_master_dev_t *spi);

#endif /* __BME_680_MAIN_H */
