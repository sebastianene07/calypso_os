#include <board.h>
#include <console_main.h>

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>

#include <source/ff.h>
#include <vfs.h>
#include <stdio.h>

int console_sensor_measure(int argc, const char *argv[])
{
  int sensor_fd = open(CONFIG_SENSOR_BME680_PATH_NAME, 0);
  if (sensor_fd < 0) {
    printf("Error %d open\n", sensor_fd);
    return sensor_fd;
  }

  uint8_t buffer[80];

  int ret = read(sensor_fd, buffer, sizeof(buffer)); 
  if (ret < 0) {
    printf("Error %d get sensor data\n", ret);
  } else {
    printf("Success\n\n");
  }

  close(sensor_fd);
  return OK;
}
