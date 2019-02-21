#include <spi.h>
#include <gpio.h>
#include <board.h>

/* Pinconfigs */

#define DC_PIN        (7)
#define DC_PORT       (1)

#define RST_PIN       (3)
#define RST_PORT      (1)

#define BUSY_PIN      (4)
#define BUSY_PORT     (1)

/* D4 */

#define POWER_EN_PIN      (5)
#define POWER_EN_PORT     (1)

/* BRDR ctrl D6 */

#define BRDR_CTRL_PIN     (8)
#define BRDR_CTRL_PIN     (1)

static void display_reset(void)
{
}

/* For waveshare EINK display
 * these are Arduino connections we need to map them to the Nordic NRF52 board
 *
 * 3.3V --> 3V3
 * GND  --> GND
 * DIN  --> D11
 * CLK  --> D13
 * CS   --> D10
 * DC   --> D9
 * RST  --> D2
 * BUSY --> D3
 *
 * SCK  D13 <-> PIN1.15
 * MOSI D11 <-> PIN1.13
 * CS   D10 <-> PIN1.12
 * DC   D9  <-> PIN1.11 - OUTPUT GPIO
 * RST  D2  <-> PIN1.03 - OUTPUT GPIO
 * BUSY D3  <-> PIN1.04 - INPUT GPIO
 */

int display_init(void)
{
  /* Configure the SPI pins */

  spi_master_config_t cfg = {INVALID_SPI_CFG};
  cfg.sck_pin  = SPIM_SCK_PIN;
  cfg.sck_port = SPIM_SCK_PORT;

  cfg.mosi_pin  = SPIM_MOSI_PIN;
  cfg.mosi_port = SPIM_MOSI_PORT;

  cfg.cs_pin  = SPIM_CS_PIN;
  cfg.cs_port = SPIM_CS_PORT;

  cfg.freq = SPI_M_FREQ_1_MBPS;
  cfg.mode = SPI_M_MODE_0;

  if (spi_configure(&cfg, 0) < 0)
  {
    return -1;
  }
}
