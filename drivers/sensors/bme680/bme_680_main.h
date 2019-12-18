#ifndef __BME_680_MAIN_H
#define __BME_680_MAIN_H

#include <spi.h>

int bme680_sensor_register(const char *name, spi_master_dev_t *spi);

#endif /* __BME_680_MAIN_H */
