#include <board.h>

#include "bme680.h"

#include <sensors/sensors.h>
#include <errno.h>
#include <scheduler.h>
#include <semaphore.h>
#include <spi.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <vfs.h>

/****************************************************************************
 * Preprocessor Definitions
 ****************************************************************************/

#define SENSOR_NAME        "BME680"

/* Log err message to console */
#define LOG_ERR(msg, ...)  printf("["SENSOR_NAME"] Error:"msg"\r\n", ##__VA_ARGS__)

/* Default temperature in Celsisus */
#define SENSOR_DEFAULT_AMBIENTAL_TEMP               (25)

/* Heater parameters */
#define SENSOR_DEFAULT_GAS_HEATER_TEMPERATURE       (320)
#define SENSOR_DEFAULT_HEATER_DURATION_MS           (150)

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/* Lower layer access to the sensor */
static void bme680_sensor_delay_ms(uint32_t period);
static int8_t bme680_sensor_spi_read(uint8_t dev_id, uint8_t reg_addr,
 uint8_t *reg_data, uint16_t len);
static int8_t bme680_sensor_spi_write(uint8_t dev_id, uint8_t reg_addr,
 uint8_t *reg_data, uint16_t len);

/* Virtual file system ops */
static int bme680_sensor_open(struct opened_resource_s *res,
 const char *pathname, int flags, mode_t mode);
static int bme680_sensor_close(struct opened_resource_s *res);
static int bme680_sensor_read(struct opened_resource_s *res, void *buf, 
 size_t count);
static int bme680_sensor_ioctl(struct opened_resource_s *priv,
 unsigned long request, unsigned long arg);

/****************************************************************************
 * Private Data
 ****************************************************************************/

/* Sensor operations exposed to upper layers */
static struct vfs_ops_s g_bme680_ops = {
  .open   = bme680_sensor_open,
  .close  = bme680_sensor_close,
  .read   = bme680_sensor_read,
  .ioctl  = bme680_sensor_ioctl,
};

/* Initialization portion */
static struct bme680_dev g_gas_sensor = {
  .dev_id     = 0,
  .intf       = BME680_SPI_INTF,
  .amb_temp   = SENSOR_DEFAULT_AMBIENTAL_TEMP,
  .read       = bme680_sensor_spi_read,
  .write      = bme680_sensor_spi_write,
  .delay_ms   = bme680_sensor_delay_ms 
};

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static int bme680_sensor_enter_forcedmode(void)
{
  int rslt;
  uint8_t set_required_settings;

  /* Set the temperature, pressure and humidity settings */
  g_gas_sensor.tph_sett.os_hum  = BME680_OS_2X;
  g_gas_sensor.tph_sett.os_pres = BME680_OS_4X;
  g_gas_sensor.tph_sett.os_temp = BME680_OS_8X;
  g_gas_sensor.tph_sett.filter  = BME680_FILTER_SIZE_3;

  /* Set the remaining gas sensor settings and link the heating profile */
  g_gas_sensor.gas_sett.run_gas = BME680_ENABLE_GAS_MEAS;

  /* Create a ramp heat waveform in 3 steps */
  g_gas_sensor.gas_sett.heatr_temp = SENSOR_DEFAULT_GAS_HEATER_TEMPERATURE;
  g_gas_sensor.gas_sett.heatr_dur = SENSOR_DEFAULT_HEATER_DURATION_MS;

  /* Select the power mode */
  /* Must be set before writing the sensor configuration */
  g_gas_sensor.power_mode = BME680_FORCED_MODE; 

  /* Set the required sensor settings needed */
  set_required_settings = BME680_OST_SEL | BME680_OSP_SEL | BME680_OSH_SEL |
    BME680_FILTER_SEL | BME680_GAS_SENSOR_SEL;

  /* Set the desired sensor configuration */
  rslt = bme680_set_sensor_settings(set_required_settings, &g_gas_sensor);

  /* Set the power mode */
  rslt = bme680_set_sensor_mode(&g_gas_sensor);
  return rslt;
}

static void bme680_sensor_delay_ms(uint32_t period)
{
  // TODO : implement with sem_timedwait(...) if the chip is not able
  // to give an interrrupt when samples are ready
}

static int8_t bme680_sensor_spi_read(uint8_t dev_id, uint8_t reg_addr,
  uint8_t *reg_data, uint16_t len)
{
}

static int8_t bme680_sensor_spi_write(uint8_t dev_id, uint8_t reg_addr,
  uint8_t *reg_data, uint16_t len)
{
}

static int bme680_sensor_open(struct opened_resource_s *res,
  const char *pathname, int flags, mode_t mode)
{
}

static int bme680_sensor_close(struct opened_resource_s *res)
{
}

static int bme680_sensor_read(struct opened_resource_s *res, void *buf, 
 size_t count)
{

  /* Get the total measurement duration so as to sleep or wait till the
   * measurement is complete */
  uint16_t meas_period;
  bme680_get_profile_dur(&meas_period, &g_gas_sensor);

  struct bme680_field_data data;

  /* Delay till the measurement is ready */

  bme680_sensor_delay_ms(meas_period);
  int rslt = bme680_get_sensor_data(&data, &g_gas_sensor);

  //printf("T: %.2f degC, P: %.2f hPa, H %.2f %%rH ", data.temperature / 100.0f,
  //    data.pressure / 100.0f, data.humidity / 1000.0f );
 
  /* Avoid using measurements from an unstable heating setup */
  if(data.status & BME680_GASM_VALID_MSK)
    printf(", G: %d ohms\n", data.gas_resistance);

  /* Trigger the next measurement if you would like to read data out continuously */
  if (g_gas_sensor.power_mode == BME680_FORCED_MODE) {
    rslt = bme680_set_sensor_mode(&g_gas_sensor);
  }
}

static int bme680_sensor_ioctl(struct opened_resource_s *priv,
 unsigned long request, unsigned long arg)
{
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int bme680_sensor_register(const char *name, spi_master_dev_t *spi)
{
  int ret = OK;

  ret = bme680_init(&g_gas_sensor);
  if (ret != BME680_OK) {
    LOG_ERR("init status %d\n", ret);
    return ret;
  }

  /* Register the upper half node with the VFS */

  ret = vfs_register_node(name, strlen(name), &g_bme680_ops,
      VFS_TYPE_CHAR_DEVICE, NULL);
  if (ret != OK) {
    LOG_ERR("register node status %d\n", ret);
  }

  return ret;
} 
