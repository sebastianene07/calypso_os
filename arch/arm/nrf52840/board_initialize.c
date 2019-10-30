#include <board.h>
#include <spi.h>
#include <serial.h>
#include <gpio.h>
#include <rtc.h>
#include <scheduler.h>

#ifdef CONFIG_DISPLAY_SSD1331
#include <display/ssd_1331.h>
#endif

#include <storage/spi_sdcard.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define XTAL                  (50000000UL)     /* Oscillator frequency */
#define SYSTEM_CLOCK          (XTAL / 2U)

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

static uint32_t SystemCoreClock = SYSTEM_CLOCK;  /* System Core Clock Frequency */

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
  rtc_init();

  struct spi_master_config_s spi[] = {
    {
      .miso_pin  = CONFIG_SPI_0_MISO_PIN,
      .miso_port = CONFIG_SPI_0_MISO_PORT,

      .mosi_pin  = CONFIG_SPI_0_MOSI_PIN,
      .mosi_port = CONFIG_SPI_0_MOSI_PORT,

      .sck_pin   = CONFIG_SPI_0_SCK_PIN,
      .sck_port  = CONFIG_SPI_0_SCK_PORT,

      .cs_pin    = CONFIG_SPI_0_CS_PIN,
      .cs_port   = CONFIG_SPI_0_CS_PORT,

      .freq      = SPI_M_FREQ_1_MBPS,
      .mode      = SPI_M_MODE_0,
    },
  };

  spi_master_dev_t *spi_devs = spi_init(spi, ARRAY_LEN(spi));

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
    .spi_dev = &spi_devs[0],
    .dc_pin  = CONFIG_DISPLAY_DC_PIN,
    .dc_port = CONFIG_DISPLAY_DC_PORT,
    .cs_pin  = spi_devs[0].dev_cfg.cs_pin,
    .cs_port = spi_devs[0].dev_cfg.cs_port,
  };

  ssd1331_display_init(&display_config);
#else
  sd_spi_init(&spi_devs[0]);
#endif

  SysTick_Config(SystemCoreClock / 2000);
}
