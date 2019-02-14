#include <spi.h>
#include <gpio.h>
#include <board.h>

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
  gpio_toogle(0, RST_PIN, RST_PORT);
  usleep(400000);

  gpio_toogle(1, RST_PIN, RST_PORT);
  usleep(400000);
}

static void display_send_cmd(uint8_t cmd)
{
  gpio_toogle(0, DC_PIN, DC_PORT);
  uint8_t data = cmd;
  spi_send(&data, sizeof(data));
}

static void display_send_data(uint8_t data)
{
  gpio_toogle(1, DC_PIN, DC_PORT);
  uint8_t cmd = data;
  spi_send(&cmd, sizeof(cmd));
}

static void display_clear(void)
{
  display_send_cmd(0x10);
  usleep(2000);

  for (int i = 0; i < 46464 / 8; i++)
  {
    display_send_data(0xFF);
  }
  usleep(2000);

  display_send_cmd(0x13);
  usleep(2000);

  for (int i = 0; i < 46464 / 8; i++)
  {
    display_send_data(0xFF);
  }
  usleep(2000);

  display_send_cmd(0x12);
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

  /* Configure the GPIO's connected to the display */

  gpio_configure(DC_PIN, DC_PORT, GPIO_DIRECTION_OUT);
  gpio_configure(RST_PIN, RST_PORT, GPIO_DIRECTION_OUT);
  gpio_configure(POWER_EN_PIN, POWER_EN_PORT, GPIO_DIRECTION_OUT);
  gpio_configure(BRDR_CTRL_PIN, 1, GPIO_DIRECTION_OUT);
  gpio_toogle(1, BRDR_CTRL_PIN, 1);
  gpio_configure(BUSY_PIN, BUSY_PORT, GPIO_DIRECTION_IN);


  /* Send reset */

  display_reset();

  /* Send power settings */

  display_send_cmd(0x01); /* POWER SETTINGS */
  display_send_data(0x03);
  display_send_data(0x00);
  display_send_data(0x2b);
  display_send_data(0x2b);
  display_send_data(0x09);

  /* BOOSTER */

  display_send_cmd(0x06);
  display_send_data(0x07);
  display_send_data(0x07);
  display_send_data(0x17);

  /* Partial display refresh */

  display_send_cmd(0x16);
  display_send_data(0x00);

  /* Power ON */

  display_send_cmd(0x04);
#if 0
  while (gpio_read(BUSY_PIN, BUSY_PORT) == 0)
  {
  }
#endif
  usleep(500000);

  /* Pannel settings */

  display_send_cmd(0x00);
  display_send_data(0xAF);
  display_send_cmd(0x30);
  display_send_data(0x3A);

  display_send_cmd(0x82);
  display_send_data(0x12);

  usleep(2000);

  display_clear();
}
