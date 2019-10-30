#include <spi_sdcard.h>
#include <stdio.h>
#include <errno.h>

#define LOG_ERR(msg, ...) printf("[sd_spi] Error:"msg, __VA_ARGS__)

/****************************************************************************
 * Macro Definitions
 ****************************************************************************/

#define SPI_INIT_CLOCK_CYCLES                 (80)
#define SPI_INIT_NUM_BYTES                    (SPI_INIT_CLOCK_CYCLES / 8)
#define SPI_INIT_DUMMY_BYTE                   (0xFF)

/* Command definitions */

#define SPI_CMD_LEN                           (6)
#define SPI_MAX_REPONSE_LEN                   (5)

#define SPI_RESET_CMD                         (0)

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

static void sd_spi_send_cmd(uint8_t cmd, uint32_t arguments, uint8_t *response, size_t rsp_len)
{
  uint8_t sd_card_cmd[SPI_CMD_LEN];

#if IS_BIG_ENDIAN
#error "Unsupported endianness"
#endif

  /* CRC7 (7 bits) + end bit (1 bit) */
  sd_card_cmd[0] = 1; 
 
  sd_card_cmd[1] = arguments & 0xFF;
  sd_card_cmd[2] = (arguments & 0xFF00) >> 8;
  sd_card_cmd[3] = (arguments & 0xFF0000) >> 16;
  sd_card_cmd[4] = (arguments & 0xFF000000) >> 24;

  /* Start bit (value 0) followed by transmission bit (value 1)
   * followed by cmd (6 bits).
   */
  sd_card_cmd[5] = (cmd & 0x3f) | 0x40;

  spi_send_recv(g_sd_spi, sd_card_cmd, sizeof(sd_card_cmd), response, rsp_len);
}

int sd_spi_init(spi_master_dev_t *spi)
{
  uint8_t spi_rsp[SPI_MAX_REPONSE_LEN] = {0};

  g_sd_spi = spi;

  /* Send 80 clocks cycle to init card */

  for (int i = 0; i < SPI_INIT_NUM_BYTES; i++)
    sd_spi_write(SPI_INIT_DUMMY_BYTE);

  sd_spi_send_cmd(SPI_RESET_CMD, 0, spi_rsp, 1); 
  if (spi_rsp[0] != 1) {
    LOG_ERR("init bad response: 0x%x\n", spi_rsp[0]);
    return -ENODEV;
  } 
}
