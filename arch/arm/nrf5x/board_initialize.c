#include <board.h>
#include <spi.h>
#include <serial.h>
#include <gpio.h>
#include <rtc.h>
#include <timer.h>
#include <scheduler.h>

#ifdef CONFIG_DISPLAY_SSD1331
#include <display/ssd_1331.h>
#endif

#ifdef CONFIG_SPI_SDCARD
#include <storage/spi_sdcard.h>
#endif

#ifdef CONFIG_SENSOR_DRIVERS
#include <sensors/sensors.h>
#endif

#if 1
#include <softdevice/nrf_softdevice_init.h>
#endif

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define CLOCK_BASE            (0x40000000)

/* Configuration offsets for registers */

#define LFCLKSRC_OFFSET       (0x518)
#define LFCLKSTART_OFFSET     (0x008)
#define EVENTS_LFCLKSTARTED   (0x104)

/* Config registers */

#define CLOCK_CONFIG(offset_r)                                              \
  ((*((volatile uint32_t *)(CLOCK_BASE + (offset_r)))))

#define LFCLKSRC_CFG              CLOCK_CONFIG(LFCLKSRC_OFFSET)
#define LFCLKSTART_CFG            CLOCK_CONFIG(LFCLKSTART_OFFSET)
#define EVENTS_LFCLKSTARTED_CFG   CLOCK_CONFIG(EVENTS_LFCLKSTARTED)

/****************************************************************************
 * Private Data
 ****************************************************************************/

static unsigned int LED = 13;
static unsigned int BUTTON_1 = 11;

/* System Core Clock Frequency */
static uint32_t g_system_core_clock_freq = CONFIG_SYSTEM_CLOCK_FREQUENCY * 1000000;

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/*
 * clock_init - initialize the system low freq clock
 *
 * Initialize the LF clock.
 */
static void clock_init(void)
{
  /* Select internal crystal osc source, no bypass */

  LFCLKSRC_CFG = 0x01;

  /* Start the low frequency clock task */

  LFCLKSTART_CFG = 0x01;

  /* Wait for the started event */

  while (EVENTS_LFCLKSTARTED == 0x01);
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/*
 * board_init - initialize the board resources
 *
 * Initialize the board specific device drivers and prepare the board.
 */
void board_init(void)
{
  /* Driver initialization logic */

  clock_init();
#ifdef CONFIG_NRF5X_RTC
  rtc_init();
#endif

#ifdef CONFIG_NRF5X_TIMER
  timer_init();
#endif

  struct spi_master_config_s spi[] = {
#ifdef CONFIG_SPI_0
    {
      .miso_pin  = CONFIG_SPI_0_MISO_PIN,
      .miso_port = CONFIG_SPI_0_MISO_PORT,

      .mosi_pin  = CONFIG_SPI_0_MOSI_PIN,
      .mosi_port = CONFIG_SPI_0_MOSI_PORT,

      .sck_pin   = CONFIG_SPI_0_SCK_PIN,
      .sck_port  = CONFIG_SPI_0_SCK_PORT,

      .cs_pin    = CONFIG_SPI_0_CS_PIN,
      .cs_port   = CONFIG_SPI_0_CS_PORT,

      .freq      = CONFIG_SPI_0_FREQUENCY,
      .mode      = SPI_M_MODE_0,
    },
#endif

#ifdef CONFIG_SPI_1
    {
      .miso_pin  = CONFIG_SPI_1_MISO_PIN,
      .miso_port = CONFIG_SPI_1_MISO_PORT,

      .mosi_pin  = CONFIG_SPI_1_MOSI_PIN,
      .mosi_port = CONFIG_SPI_1_MOSI_PORT,

      .sck_pin   = CONFIG_SPI_1_SCK_PIN,
      .sck_port  = CONFIG_SPI_1_SCK_PORT,

      .cs_pin    = CONFIG_SPI_1_CS_PIN,
      .cs_port   = CONFIG_SPI_1_CS_PORT,

      .freq      = CONFIG_SPI_1_FREQUENCY,
      .mode      = SPI_M_MODE_0,
    },
#endif
  };

//  spi_master_dev_t *spi_devs = spi_init(spi, ARRAY_LEN(spi));

  uart_low_init();
  uart_low_send("\r\n.");

  gpio_init();
  uart_low_send(".");

  gpio_configure(LED, 0, GPIO_DIRECTION_OUT, GPIO_PIN_INPUT_DISCONNECT,
                 GPIO_NO_PULL, GPIO_PIN_S0S1, GPIO_PIN_NO_SENS);
  uart_low_send(".\r\n");

  uart_init();

#ifdef CONFIG_DISPLAY_SSD1331
  ssd1331_config_t display_config = {
    .spi_dev = &spi_devs[CONFIG_DISPLAY_DRIVER_SSD1331_SPI_ID],
    .dc_pin  = CONFIG_DISPLAY_DC_PIN,
    .dc_port = CONFIG_DISPLAY_DC_PORT,
    .cs_pin  = spi_devs[0].dev_cfg.cs_pin,
    .cs_port = spi_devs[0].dev_cfg.cs_port,
  };

  ssd1331_display_init(&display_config);
#endif

#ifdef CONFIG_SPI_SDCARD
  gpio_configure(CONFIG_SPI_SDCARD_VSYS_PIN, CONFIG_SPI_SDCARD_VSYS_PORT,
    GPIO_DIRECTION_OUT, GPIO_PIN_INPUT_DISCONNECT,
    GPIO_NO_PULL, GPIO_PIN_S0S1, GPIO_PIN_NO_SENS);

  gpio_toogle(0, CONFIG_SPI_SDCARD_VSYS_PIN, CONFIG_SPI_SDCARD_VSYS_PORT);
  gpio_toogle(1, CONFIG_SPI_SDCARD_VSYS_PIN, CONFIG_SPI_SDCARD_VSYS_PORT);
  sd_spi_init(&spi_devs[CONFIG_SPI_SDCARD_SPI_ID]);
#endif

#ifdef CONFIG_SENSOR_BME680
  bme680_sensor_register(CONFIG_SENSOR_BME680_PATH_NAME,
      &spi_devs[CONFIG_SENSOR_BME680_SPI_ID]);
#endif

  SysTick_Config(g_system_core_clock_freq / CONFIG_SYSTEM_SCHEDULER_SLICE_FREQUENCY);
}
