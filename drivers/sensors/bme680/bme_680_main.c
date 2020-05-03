#include <board.h>

#include <sensors/sensors.h>
#include <errno.h>
#include <gpio.h>
#include <scheduler.h>
#include <semaphore.h>
#include <spi.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <vfs.h>

#include <bsec/inc/bsec_datatypes.h>

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

/* The device registartion counter */
static spi_master_dev_t *g_spi_m;

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static int bme680_sensor_enter_forcedmode(struct bme680_dev *dev)
{
  int rslt;
  uint8_t set_required_settings;

  /* Set the temperature, pressure and humidity settings */
  dev->tph_sett.os_hum  = BME680_OS_2X;
  dev->tph_sett.os_pres = BME680_OS_4X;
  dev->tph_sett.os_temp = BME680_OS_8X;
  dev->tph_sett.filter  = BME680_FILTER_SIZE_3;

  /* Set the remaining gas sensor settings and link the heating profile */
  dev->gas_sett.run_gas = BME680_ENABLE_GAS_MEAS;

  /* Create a ramp heat waveform in 3 steps */
  dev->gas_sett.heatr_temp = SENSOR_DEFAULT_GAS_HEATER_TEMPERATURE;
  dev->gas_sett.heatr_dur = SENSOR_DEFAULT_HEATER_DURATION_MS;

  /* Select the power mode */
  /* Must be set before writing the sensor configuration */
  dev->power_mode = BME680_FORCED_MODE;

  /* Set the required sensor settings needed */
  set_required_settings = BME680_OST_SEL | BME680_OSP_SEL | BME680_OSH_SEL |
    BME680_FILTER_SEL | BME680_GAS_SENSOR_SEL;

  /* Set the desired sensor configuration */
  rslt = bme680_set_sensor_settings(set_required_settings, dev);

  /* Set the power mode */
  rslt = bme680_set_sensor_mode(dev);
  return rslt;
}

static void bme680_sensor_delay_ms(uint32_t period)
{
  // TODO : implement with sem_timedwait(...) if the chip is not able
  // to give an interrrupt when samples are ready

  usleep(period * 100);
}

static int8_t bme680_sensor_spi_read(uint8_t dev_id, uint8_t reg_addr,
  uint8_t *reg_data, uint16_t len)
{
  gpio_toogle(0, g_spi_m->dev_cfg.cs_pin, g_spi_m->dev_cfg.cs_port);

  uint8_t miso_byte, mosy_byte = 0xFF;
  spi_send_recv(g_spi_m, &reg_addr, sizeof(uint8_t), &miso_byte,
    sizeof(uint8_t));

  for (int i = 0; i < len; i++) {
    spi_send_recv(g_spi_m, &mosy_byte, sizeof(uint8_t), reg_data + i,
      sizeof(uint8_t));
  }

  gpio_toogle(1, g_spi_m->dev_cfg.cs_pin, g_spi_m->dev_cfg.cs_port);

  return OK;
}

static int8_t bme680_sensor_spi_write(uint8_t dev_id, uint8_t reg_addr,
  uint8_t *reg_data, uint16_t len)
{

  gpio_toogle(0, g_spi_m->dev_cfg.cs_pin, g_spi_m->dev_cfg.cs_port);

  uint8_t miso_byte = 0XFF;
  spi_send_recv(g_spi_m, &reg_addr, sizeof(uint8_t), &miso_byte,
    sizeof(uint8_t));

  for (int i = 0; i < len; i++) {
    spi_send_recv(g_spi_m, reg_data + i, sizeof(uint8_t), &miso_byte,
      sizeof(uint8_t));
  }

  gpio_toogle(1, g_spi_m->dev_cfg.cs_pin, g_spi_m->dev_cfg.cs_port);

  return OK;
}

static int bme680_sensor_open(struct opened_resource_s *res,
  const char *pathname, int flags, mode_t mode)
{
  bme680_sensor_t *gas_sensor = (bme680_sensor_t *)res->vfs_node->priv;

  sem_wait(&gas_sensor->lock_sensor);

  int ret = bme680_init(&gas_sensor->dev);
  if (ret != BME680_OK) {
    LOG_ERR("init status %d\n", ret);
    sem_post(&gas_sensor->lock_sensor);
    return ret;
  }

  ret = bme680_sensor_enter_forcedmode(&gas_sensor->dev);

  sem_post(&gas_sensor->lock_sensor);

  return OK;
}

static int bme680_sensor_close(struct opened_resource_s *res)
{
  // TODO exit force mode & put sensor to sleep
  return OK;
}

static int bme680_sensor_read(struct opened_resource_s *res, void *buf,
 size_t count)
{
  bme680_sensor_t *gas_sensor = (bme680_sensor_t *)res->vfs_node->priv;
  struct bme680_dev *dev = &gas_sensor->dev;
  struct bme680_field_data data;

  sem_wait(&gas_sensor->lock_sensor);

  uint16_t meas_period;
  bme680_get_profile_dur(&meas_period, dev);
  bme680_sensor_delay_ms(meas_period);

  bme680_get_sensor_data(&data, dev);
  int data_size = sizeof(struct bme680_field_data) > count ? count :
    sizeof(struct bme680_field_data);

  memcpy(buf, &data, data_size);

  if (dev->power_mode == BME680_FORCED_MODE) {
    bme680_set_sensor_mode(dev);
  }

  sem_post(&gas_sensor->lock_sensor);

  return data_size; 
}

static int bme680_sensor_ioctl(struct opened_resource_s *priv,
 unsigned long request, unsigned long arg)
{
  int ret = -ENOSYS;
  uint8_t set_required_settings;

  bme680_sensor_t *gas_sensor = (bme680_sensor_t *)priv->vfs_node->priv;
  struct bme680_dev *dev = &gas_sensor->dev;

  sem_wait(&gas_sensor->lock_sensor);

  switch (request) {
#ifdef CONFIG_LIBRARY_BSEC
    case IO_BME680_SET_CONFIG:
      {
        bsec_bme_settings_t *sensor_settings = (bsec_bme_settings_t *)arg;
        if (sensor_settings == NULL) {
          sem_post(&gas_sensor->lock_sensor);
          return -EINVAL;
        }

        if (sensor_settings->trigger_measurement) {
          dev->tph_sett.os_hum = sensor_settings->humidity_oversampling;
          dev->tph_sett.os_pres = sensor_settings->pressure_oversampling;
          dev->tph_sett.os_temp = sensor_settings->temperature_oversampling;
          dev->gas_sett.run_gas = sensor_settings->run_gas;
          dev->gas_sett.heatr_temp = sensor_settings->heater_temperature;
          dev->gas_sett.heatr_dur  = sensor_settings->heating_duration;

          dev->power_mode = BME680_FORCED_MODE;
          set_required_settings = BME680_OST_SEL | BME680_OSP_SEL |
            BME680_OSH_SEL | BME680_GAS_SENSOR_SEL;

          ret = bme680_set_sensor_settings(set_required_settings, dev);
          if (ret != BME680_OK) {
            sem_post(&gas_sensor->lock_sensor);
            return -EIO;
          }

          ret = bme680_set_sensor_mode(dev);
          if (ret != BME680_OK) {
            sem_post(&gas_sensor->lock_sensor);
            return -EACCES;
          }
        }
      }
      break;
#endif

    case IO_BME680_BUS_READ:
      {
        bme680_sensor_spi_transaction_t *bus_transaction =
          (bme680_sensor_spi_transaction_t *)arg;


        ret = bme680_sensor_spi_read(bus_transaction->dev_addr,
                                     bus_transaction->reg_addr,
                                     bus_transaction->reg_data_ptr,
                                     bus_transaction->data_len);
      }
      break;

    case IO_BME680_BUS_WRITE:
      {
        bme680_sensor_spi_transaction_t *bus_transaction =
          (bme680_sensor_spi_transaction_t *)arg;


        ret = bme680_sensor_spi_write(bus_transaction->dev_addr,
                                      bus_transaction->reg_addr,
                                      bus_transaction->reg_data_ptr,
                                      bus_transaction->data_len);
      }
      break;

    default:
      break;
  }

  sem_post(&gas_sensor->lock_sensor);

  return ret;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int bme680_sensor_register(const char *name, spi_master_dev_t *spi)
{
  int ret = OK;

  bme680_sensor_t *gas_sensor = calloc(1, sizeof(bme680_sensor_t));
  if (gas_sensor == NULL) {
    return -ENOMEM;
  }

  gas_sensor->dev = (struct bme680_dev) {
    .dev_id     = 0,
    .intf       = BME680_SPI_INTF,
    .amb_temp   = SENSOR_DEFAULT_AMBIENTAL_TEMP,
    .read       = bme680_sensor_spi_read,
    .write      = bme680_sensor_spi_write,
    .delay_ms   = bme680_sensor_delay_ms
  };

  sem_init(&gas_sensor->lock_sensor, 0, 1);

  /* Register the upper half node with the VFS */
  ret = vfs_register_node(name, strlen(name), &g_bme680_ops,
      VFS_TYPE_CHAR_DEVICE, gas_sensor);
  if (ret != OK) {
    LOG_ERR("register node status %d\n", ret);
    free(gas_sensor);
  }

  g_spi_m = spi;

  return ret;
}
