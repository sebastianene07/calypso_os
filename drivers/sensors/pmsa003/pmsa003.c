#include <board.h>

#include <sensors/sensors.h>
#include <errno.h>
#include <gpio.h>
#include <scheduler.h>
#include <semaphore.h>
#include <serial.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <vfs.h>

#include "pmsa003.h"

/****************************************************************************
 * Preprocessor Definitions
 ****************************************************************************/

#define SENSOR_NAME        "PMSA003"

/* Log err message to console */
#define LOG_ERR(msg, ...)  printf("["SENSOR_NAME"] Error:"msg"\r\n", ##__VA_ARGS__)

/* Data start token from sensor */
#define PMSA003_DATA_START        (0x42)

/* Sample start */
#define PMSA003_DATA_SAMPLE_START (0x4D)

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/* Lower layer access to the sensor */
static int pmsa_sensor_open(struct opened_resource_s *res,
    const char *pathname, int flags, mode_t mode);
static int pmsa_sensor_close(struct opened_resource_s *res);
static int pmsa_sensor_read(struct opened_resource_s *res, void *buf, size_t count);
static int pmsa_sensor_ioctl(struct opened_resource_s *res, unsigned long request,
    unsigned long arg);

/****************************************************************************
 * Private Data
 ****************************************************************************/

/* Sensor operations exposed to upper layers */
static struct vfs_ops_s g_pmsa_ops = {
  .open   = pmsa_sensor_open,
  .close  = pmsa_sensor_close,
  .read   = pmsa_sensor_read,
  .ioctl  = pmsa_sensor_ioctl,
};

/* Commands sent to sensor */
static const char PMSA003_CMD_ENTER_IDLE[] = 
  {PMSA003_DATA_START, 0x4D, 0xE4, 0x00, 0x00, 0x01, 0x73};

static const char PMSA003_CMD_ENTER_NORMAL[] = 
  {PMSA003_DATA_START, 0x4D, 0xE4, 0x00, 0x01, 0x01, 0x74};

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

static int pmsa_sensor_open(struct opened_resource_s *res,
    const char *pathname, int flags, mode_t mode);
static int pmsa_sensor_close(struct opened_resource_s *res);
static int pmsa_sensor_read(struct opened_resource_s *res, void *buf, size_t count);
static int pmsa_sensor_ioctl(struct opened_resource_s *res, unsigned long request,
    unsigned long arg);

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static int pmsa_sensor_open(struct opened_resource_s *res,
    const char *pathname, int flags, mode_t mode)
{
  int ret = OK;
  pmsa003_sensor_t *pm_sensor     = (pmsa003_sensor_t *)res->vfs_node->priv;
  struct uart_lower_s *uart_lower = pm_sensor->interface;

  ret = uart_lower->open_cb(uart_lower); 
  if (ret < 0) {
    return ret;
  }

  return ret;
}

static int pmsa_sensor_close(struct opened_resource_s *res)
{
  int ret = OK;

  return ret;
}

static int pmsa_sensor_read(struct opened_resource_s *res, void *buf, size_t count)
{
  int ret = OK;
  pmsa003_sensor_t *pm_sensor     = (pmsa003_sensor_t *)res->vfs_node->priv;
  struct uart_lower_s *uart_lower = pm_sensor->interface;
  const int bytes_read_up_limit   = 32;
  int bytes_read                  = 0;

  if (count != PMSA003_DATA_LEN) {
    return -EINVAL;
  }

  sem_wait(&pm_sensor->lock_sensor);

  while (bytes_read < bytes_read_up_limit) {
    uint8_t cmd = 0;
    ret = uart_lower->read_cb(uart_lower, &cmd, sizeof(cmd)); 
    if (ret < 0) {
      ret = -EINVAL;
      goto errout;
    }

    if (cmd != PMSA003_DATA_START) {
      bytes_read++;
      continue;
    } 

    ret = uart_lower->read_cb(uart_lower, &cmd, sizeof(cmd)); 
    if (ret < 0) {
      ret = -EINVAL;
      goto errout;
    }

    if (cmd != PMSA003_DATA_SAMPLE_START) {
      bytes_read++;
      continue;
    } 

    /* This should be a blocking call until we get all the data */
    ret = uart_lower->read_cb(uart_lower, buf + 2,
      PMSA003_DATA_LEN - 2);
    if (ret != (PMSA003_DATA_LEN - 2)) {
      ret = -EINVAL;
      goto errout;
    } else {
      ret = PMSA003_DATA_LEN;
      break; 
    }
  } 

  if (bytes_read >= bytes_read_up_limit) {
    ret = -ENOSYS;
  }

errout:
  sem_post(&pm_sensor->lock_sensor);
  return ret;
}

static int pmsa_sensor_ioctl(struct opened_resource_s *res, unsigned long request,
    unsigned long arg)
{
  pmsa003_sensor_t *pm_sensor     = (pmsa003_sensor_t *)res->vfs_node->priv;
  struct uart_lower_s *uart_lower = pm_sensor->interface;
  int ret = -ENOSYS;

  char *cmd = NULL;
  size_t cmd_size = 0;

  sem_wait(&pm_sensor->lock_sensor);
  switch (request) {
    case IO_PMSA003_ENTER_IDLE:
      { 
        cmd      = (char *)PMSA003_CMD_ENTER_IDLE;
        cmd_size = sizeof(PMSA003_CMD_ENTER_IDLE);
      }
      break;

    case IO_PMSA003_ENTER_NORMAL:
      {
        cmd      = (char *)PMSA003_CMD_ENTER_NORMAL;
        cmd_size = sizeof(PMSA003_CMD_ENTER_NORMAL);
      }
      break;

    default:
      break;
  }

  if (cmd != NULL) {

    uint8_t *cmd_copy = calloc(1, cmd_size);
    if (cmd_copy == NULL) {
      return -ENOMEM;
    }

    memcpy(cmd_copy, cmd, cmd_size);
    ret = uart_lower->write_cb(uart_lower, cmd_copy, cmd_size);
    free(cmd_copy);

    if (ret < 0) {
      sem_post(&pm_sensor->lock_sensor);
      return ret;
    }

    ret = OK;
  }

  sem_post(&pm_sensor->lock_sensor);
  return ret;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int pmsa_sensor_register(const char *name, struct uart_lower_s *uart_lowerhalf)
{
  int ret = OK;

  pmsa003_sensor_t *pm_sensor = calloc(1, sizeof(pmsa003_sensor_t));
  if (pm_sensor == NULL) {
    return -ENOMEM;
  }  

  sem_init(&pm_sensor->lock_sensor, 0, 1);
  pm_sensor->interface = uart_lowerhalf;

  ret = vfs_register_node(name, strlen(name), &g_pmsa_ops, VFS_TYPE_CHAR_DEVICE,
      pm_sensor);
  return ret;
}
