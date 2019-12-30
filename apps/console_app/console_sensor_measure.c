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
#include <sensors/bme680/bme_680_main.h>

#ifdef CONFIG_LIBRARY_BSEC
#include <bsec/inc/bsec_interface.h>
#include "bsec_lib/bsec_integration.h"
#endif

static int g_sensor_fd;

extern volatile uint64_t g_rtc_ticks_ms;

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
  printf("temperature %d, humidity: %d pressure %d\n", (uint32_t)temperature,
				(uint32_t)(humidity), (uint32_t)pressure);
}

static uint32_t state_load(uint8_t *state_buffer, uint32_t n_buffer)
{
  return 0;
}

static void state_save(const uint8_t *state_buffer, uint32_t length)
{
}

static uint32_t config_load(uint8_t *config_buffer, uint32_t n_buffer)
{
  return 0;
}

static int64_t get_timestamp_us(void)
{
  return g_rtc_ticks_ms * 1000;
}

static void sleep(uint32_t t_ms)
{
  usleep(1000 * t_ms);
}

static int8_t bus_read(uint8_t dev_addr, uint8_t reg_addr,
  uint8_t *reg_data_ptr, uint16_t data_len)
{
  bme680_sensor_spi_transaction_t transaction = {
    .dev_addr = dev_addr,
    .reg_addr = reg_addr,
    .reg_data_ptr = reg_data_ptr,
    .data_len     = data_len,
  };

  int8_t ret = ioctl(g_sensor_fd, IO_BME680_BUS_READ, &transaction);
  return ret;
}

static int8_t bus_write(uint8_t dev_addr, uint8_t reg_addr,
  uint8_t *reg_data_ptr, uint16_t data_len)
{
  bme680_sensor_spi_transaction_t transaction = {
    .dev_addr = dev_addr,
    .reg_addr = reg_addr,
    .reg_data_ptr = reg_data_ptr,
    .data_len     = data_len,
  };

  int8_t ret = ioctl(g_sensor_fd, IO_BME680_BUS_WRITE, &transaction);
  return ret;
}

int console_sensor_measure(int argc, const char *argv[])
{
  int ret;
  return_values_init bsec_ret;

  g_sensor_fd = open(CONFIG_SENSOR_BME680_PATH_NAME, 0);
  if (g_sensor_fd < 0) {
    printf("Error %d open\n", g_sensor_fd);
    return g_sensor_fd;
  }

  bsec_ret = bsec_iot_init(BSEC_SAMPLE_RATE_LP, 5.0f, bus_write, bus_read, sleep,
    state_load, config_load);
  if (bsec_ret.bme680_status) {
    printf("Error init BSEC lib %d\n", bsec_ret.bme680_status);
    close(g_sensor_fd);
    return -EINVAL;
  }

  bsec_iot_loop(sleep, get_timestamp_us, bsec_out_data, state_save, 10000);

  close(g_sensor_fd);
  return OK;
}
