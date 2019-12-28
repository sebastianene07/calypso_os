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
#include <bsec/inc/bsec_interface.h>

#ifdef CONFIG_LIBRARY_BSEC
#include "bsec_lib/bsec_integration.h"
#endif

static uint32_t g_sample_counter = 0;

static void bsec_out_data(int64_t time_stamp, float iaq, uint8_t iaq_accuracy,
 float temperature, float humidity, float pressure, float raw_temperature,
 float raw_humidity, float gas, bsec_library_return_t bsec_status,
 float static_iaq, float co2_equivalent, float breath_voc_equivalent)
{
  printf("(IAQ: %d, accuracy: %d \n", (uint32_t)(iaq * 100.0), iaq_accuracy);
  printf("(VOC: %d, CO2: %d)\n", (uint32_t)(breath_voc_equivalent * 100.0), (uint32_t)(co2_equivalent * 100.0));
}

int console_sensor_measure(int argc, const char *argv[])
{
  int ret;
  int sensor_fd = open(CONFIG_SENSOR_BME680_PATH_NAME, 0);
  if (sensor_fd < 0) {
    printf("Error %d open\n", sensor_fd);
    return sensor_fd;
  }

#ifdef CONFIG_LIBRARY_BSEC
  ret = bsec_init();
  if (ret != BSEC_OK) {
    printf("Error %d BSEC lib init\n", ret);
    close(sensor_fd);
    return ret;
  }

	ret = bme680_bsec_update_subscription(BSEC_SAMPLE_RATE_LP);
	if (ret != BSEC_OK) {
		printf("Error %d BSEC update subscription\n", ret);
		close(sensor_fd);
		return ret;
	}
#endif

  while (1) {

    struct bme680_field_data data;
    bsec_input_t bsec_inputs[BSEC_MAX_PHYSICAL_SENSOR];
    int num_inputs = 0;

    int ret = read(sensor_fd, &data, sizeof(data));
    if (ret < 0) {
      printf("Error %d get sensor data\n", ret);
      close(sensor_fd);
      return ret;
    } else {
      printf("Success\n\n");
    }

		bsec_inputs[num_inputs].sensor_id = BSEC_INPUT_PRESSURE;
		bsec_inputs[num_inputs].signal    = data.pressure;
		bsec_inputs[num_inputs].time_stamp = g_sample_counter;
    num_inputs++;

		bsec_inputs[num_inputs].sensor_id = BSEC_INPUT_TEMPERATURE;
		bsec_inputs[num_inputs].signal    = data.temperature / 100.0f;
		bsec_inputs[num_inputs].time_stamp = g_sample_counter;
    num_inputs++;

		bsec_inputs[num_inputs].sensor_id = BSEC_INPUT_HEATSOURCE;
		bsec_inputs[num_inputs].signal    = 0.0f;
		bsec_inputs[num_inputs].time_stamp = g_sample_counter;
    num_inputs++;

		bsec_inputs[num_inputs].sensor_id = BSEC_INPUT_HUMIDITY;
		bsec_inputs[num_inputs].signal    = data.humidity / 1000.0f;
		bsec_inputs[num_inputs].time_stamp = g_sample_counter;
    num_inputs++;

		bsec_inputs[num_inputs].sensor_id = BSEC_INPUT_GASRESISTOR;
		bsec_inputs[num_inputs].signal    = data.gas_resistance;
		bsec_inputs[num_inputs].time_stamp = g_sample_counter;
    num_inputs++;

    /* Process the data with the bsec libraray */
    bme680_bsec_process_data(bsec_inputs, num_inputs, bsec_out_data);

    printf("T: %d.%02d degC, P: %d.%02d hPa, H %d.%03d percent ",
        data.temperature / 100, data.temperature % 100,
        data.pressure / 100, data.pressure % 100,
        data.humidity / 1000, data.humidity % 1000);

    if(data.status & BME680_GASM_VALID_MSK)
      printf(", G: %d ohms\n", data.gas_resistance);

    g_sample_counter++;
  }

  close(sensor_fd);
  return OK;
}
