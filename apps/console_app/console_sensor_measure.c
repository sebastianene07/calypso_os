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

#include <sensors/bme680/bme680.h>

int console_sensor_measure(int argc, const char *argv[])
{
  int sensor_fd = open(CONFIG_SENSOR_BME680_PATH_NAME, 0);
  if (sensor_fd < 0) {
    printf("Error %d open\n", sensor_fd);
    return sensor_fd;
  }

  struct bme680_field_data data;

  int ret = read(sensor_fd, &data, sizeof(data)); 
  if (ret < 0) {
    printf("Error %d get sensor data\n", ret);
  } else {
    printf("Success\n\n");
  }

  printf("T: %d.%02d degC, P: %d.%02d hPa, H %d.%03d percent ",
      data.temperature / 100, data.temperature % 100,
      data.pressure / 100, data.pressure % 100,
      data.humidity / 1000, data.humidity % 1000);
 
  if(data.status & BME680_GASM_VALID_MSK)
    printf(", G: %d ohms\n", data.gas_resistance);

  close(sensor_fd);
  return OK;
}
