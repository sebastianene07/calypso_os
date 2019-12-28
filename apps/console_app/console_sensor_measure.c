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

static uint64_t g_sample_counter = 0;

static const char *get_name_from_iaq_index(uint32_t index)
{
	if (index >= 0 && index <= 50)
		/* Pure air; best for well-being */
		return "excellent";
	else if (index > 50 && index <= 100)
		/* No irritation or impact on well-being */
		return "good";
	else if (index > 100 && index <= 150)
		/* Reduction of well-being possible */
		return "lightly polluted";
	else if (index > 150 && index <= 200)
		/* More significant irritation possible */
		return "moderated polluted";
	else if (index > 200 && index <= 250)
		/* Exposition might lead to effects like headache depending on type
     * of VOCs */
		return "heavily polluted";
	else if (index > 250 && index <= 350)
		/* More severe health issue possible if harmful VOC present */
		return "severely polluted";
	else
		/* Headaches, additional neurotoxic effects possible */
		return "exteremely polluted";
}

static void bsec_out_data(int64_t time_stamp, float iaq, uint8_t iaq_accuracy,
 float temperature, float humidity, float pressure, float raw_temperature,
 float raw_humidity, float gas, bsec_library_return_t bsec_status,
 float static_iaq, float co2_equivalent, float breath_voc_equivalent)
{
	uint32_t int_iaq = (uint32_t)iaq;
  printf("air quality:%s IAQ: %d, accuracy: %d timestamp: %u\n",
				 get_name_from_iaq_index(int_iaq), int_iaq, iaq_accuracy, time_stamp);
  printf("(VOC: %d, CO2: %d)\n", (uint32_t)(breath_voc_equivalent),
				(uint32_t)(co2_equivalent));
}

int console_sensor_measure(int argc, const char *argv[])
{
  int ret;
  int sensor_fd = open(CONFIG_SENSOR_BME680_PATH_NAME, 0);
  if (sensor_fd < 0) {
    printf("Error %d open\n", sensor_fd);
    return sensor_fd;
  }

	int rtc_fd = open(CONFIG_RTC_PATH, 0);
	if (rtc_fd < 0)
	{
		close(sensor_fd);
		return rtc_fd;
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
		bsec_bme_settings_t sensor_settings;
    int num_inputs = 0;

		int ret = read(rtc_fd, &g_sample_counter, sizeof(g_sample_counter));
		if (ret < 0)
		{
			return ret;
		}

		g_sample_counter = g_sample_counter * 125; /* in miliseconds */
		g_sample_counter = g_sample_counter * 1000 * 1000;

		/* Control sensor reading */
		bsec_sensor_control(g_sample_counter, &sensor_settings);

    ret = read(sensor_fd, &data, sizeof(data));
    if (ret < 0) {
      printf("Error %d get sensor data\n", ret);
      close(sensor_fd);
      return ret;
    }

		if (sensor_settings.process_data & BSEC_PROCESS_PRESSURE) {
			bsec_inputs[num_inputs].sensor_id = BSEC_INPUT_PRESSURE;
			bsec_inputs[num_inputs].signal    = data.pressure;
			bsec_inputs[num_inputs].time_stamp = g_sample_counter;
			num_inputs++;
		}

		if (sensor_settings.process_data & BSEC_PROCESS_TEMPERATURE) {
			bsec_inputs[num_inputs].sensor_id = BSEC_INPUT_TEMPERATURE;
			bsec_inputs[num_inputs].signal    = data.temperature / 100.0f;
			bsec_inputs[num_inputs].time_stamp = g_sample_counter;
			num_inputs++;

			bsec_inputs[num_inputs].sensor_id = BSEC_INPUT_HEATSOURCE;
			bsec_inputs[num_inputs].signal    = 0.0f;
			bsec_inputs[num_inputs].time_stamp = g_sample_counter;
			num_inputs++;
		}

		if (sensor_settings.process_data & BSEC_PROCESS_HUMIDITY) {
			bsec_inputs[num_inputs].sensor_id = BSEC_INPUT_HUMIDITY;
			bsec_inputs[num_inputs].signal    = data.humidity / 1000.0f;
			bsec_inputs[num_inputs].time_stamp = g_sample_counter;
			num_inputs++;
		}

		if (sensor_settings.process_data & BSEC_PROCESS_GAS) {
			bsec_inputs[num_inputs].sensor_id = BSEC_INPUT_GASRESISTOR;
			bsec_inputs[num_inputs].signal    = data.gas_resistance;
			bsec_inputs[num_inputs].time_stamp = g_sample_counter;
			num_inputs++;
		}

    /* Process the data with the bsec libraray */
    bme680_bsec_process_data(bsec_inputs, num_inputs, bsec_out_data);

    printf("T: %d.%02d degC, P: %d.%02d hPa, H %d.%03d percent ",
        data.temperature / 100, data.temperature % 100,
        data.pressure / 100, data.pressure % 100,
        data.humidity / 1000, data.humidity % 1000);

    if(data.status & BME680_GASM_VALID_MSK)
      printf(", G: %d ohms\n", data.gas_resistance);
  }

  close(sensor_fd);
	close(rtc_fd);
  return OK;
}
