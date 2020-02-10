#ifndef __SENSORS_H
#define __SENSORS_H

#include <board.h>

#ifdef CONFIG_SENSOR_BME680
#include <sensors/bme680/bme_680_main.h>
#endif

#ifdef CONFIG_SENSOR_PMSA003
#include <sensors/pmsa003/pmsa003.h>
#endif

#endif /* __SENSORS_H */
