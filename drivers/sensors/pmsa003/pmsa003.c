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

  return ret;
}

static int pmsa_sensor_ioctl(struct opened_resource_s *res, unsigned long request,
    unsigned long arg)
{
  int ret = OK;

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

  ret = vfs_register_node(name, strlen(name), &g_pmsa_ops, VFS_TYPE_CHAR_DEVICE,
      pm_sensor);
  return ret;
}
