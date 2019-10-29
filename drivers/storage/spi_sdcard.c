#include <spi.h>

/****************************************************************************
 * Macro Definitions
 ****************************************************************************/

#define SPI_INIT_CLOCK_CYCLES                 (80)
#define SPI_INIT_NUM_BYTES                    (SPI_INIT_CLOCK_CYCLES / 8)
#define SPI_INIT_DUMMY_BYTE                   (0xFF)

/****************************************************************************
 * Private Data
 ****************************************************************************/

/* Local reference to the SPI driver in master mode */
static spi_master_dev_t *g_sd_spi;

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static uint8_t sd_spi_write(uint8_t data)
{
  uint8_t recv = 0;
  spi_send_recv(g_sd_spi, &data, sizeof(uint8_t), &recv, sizeof(uint8_t));
  return recv;
}

void sd_spi_init(spi_master_dev_t *spi)
{
  g_sd_spi = spi;

  /* Send 80 clocks cycle to init card */

  for (int i = 0; i < SPI_INIT_NUM_BYTES; i++)
    sd_spi_write(SPI_INIT_DUMMY_BYTE);
}
