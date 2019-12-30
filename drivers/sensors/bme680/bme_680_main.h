#ifndef __BME_680_MAIN_H
#define __BME_680_MAIN_H

#include <spi.h>
#include <semaphore.h>

#include "bme680.h"

#define IO_BME680_SET_CONFIG                  (0u)
#define IO_BME680_BUS_READ                    (1u)
#define IO_BME680_BUS_WRITE                   (2u)

typedef struct bme680_sensor_spi_transaction_s {
  uint8_t dev_addr;
  uint8_t reg_addr;
  uint8_t *reg_data_ptr;
  uint16_t data_len;
} bme680_sensor_spi_transaction_t;

typedef struct bme680_sensor_s {
  sem_t lock_sensor;
  spi_master_dev_t interface;
  struct bme680_dev dev;
} bme680_sensor_t;


int bme680_sensor_register(const char *name, spi_master_dev_t *spi);

#endif /* __BME_680_MAIN_H */
