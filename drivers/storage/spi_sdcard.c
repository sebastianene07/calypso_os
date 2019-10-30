#include <spi_sdcard.h>
#include <stdio.h>
#include <errno.h>
#include <gpio.h>

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
#define SPI_SEND_IF_COND_CMD                  (8)        
#define SPI_SEND_CSD                          (9)
#define SPI_APP_CMD                           (55)
#define SPI_READ_OCR                          (58)

#define SD_CARD_MAX_INIT_FAILURES             (3)

/****************************************************************************
 * Private Data
 ****************************************************************************/

/* Local reference to the SPI driver in master mode */
static spi_master_dev_t *g_sd_spi;

/* CRC7 table */
static uint8_t g_crc7_table[256];

 /****************************************************************************
 * Private Functions
 ****************************************************************************/

static uint8_t sd_spi_write(uint8_t data)
{
  uint8_t recv = 0;
  spi_send_recv(g_sd_spi, &data, sizeof(uint8_t), &recv, sizeof(uint8_t));
  return recv;
}

static void sd_spi_set_cs(uint8_t state)
{
  gpio_toogle(state, g_sd_spi->dev_cfg.cs_pin, g_sd_spi->dev_cfg.cs_port);
}

static void sd_generate_crc_table(void)
{
  int i, j;
  uint8_t CRCPoly = 0x89;  // the value of our CRC-7 polynomial
 
  // generate a table value for all 256 possible byte values
  for (i = 0; i < 256; ++i) {
    g_crc7_table[i] = (i & 0x80) ? i ^ CRCPoly : i;
    for (j = 1; j < 8; ++j) {
        g_crc7_table[i] <<= 1;
        if (g_crc7_table[i] & 0x80)
            g_crc7_table[i] ^= CRCPoly;
    }
  }
}
 
static uint8_t sd_crc_add(uint8_t crc, uint8_t message_byte)
{
    return g_crc7_table[(crc << 1) ^ message_byte];
} 

static void sd_spi_send_cmd(uint8_t cmd, uint32_t arguments, uint8_t *response, size_t rsp_len)
{
  uint8_t sd_card_cmd[SPI_CMD_LEN];
  uint8_t crc7 = 0;

  sd_spi_set_cs(0);

#if IS_BIG_ENDIAN
#error "Unsupported endianness"
#endif

  /* Start bit (value 0) followed by transmission bit (value 1)
   * followed by cmd (6 bits).
   */
  sd_card_cmd[0] = (cmd & 0x3f) | 0x40;
  crc7 = sd_crc_add(crc7, sd_card_cmd[0]);

  sd_card_cmd[1] = (arguments & 0xFF000000) >> 24;
  crc7 = sd_crc_add(crc7, sd_card_cmd[1]);

  sd_card_cmd[2] = (arguments & 0xFF0000) >> 16;
  crc7 = sd_crc_add(crc7, sd_card_cmd[2]);

  sd_card_cmd[3] = (arguments & 0xFF00) >> 8;
  crc7 = sd_crc_add(crc7, sd_card_cmd[3]);

  sd_card_cmd[4] = arguments & 0xFF;
  crc7 = sd_crc_add(crc7, sd_card_cmd[4]);

  /* CRC7 (7 bits) + end bit (1 bit) */
  sd_card_cmd[5] = (crc7 << 1) | 0x1; 
 
  spi_send_recv(g_sd_spi, sd_card_cmd, sizeof(sd_card_cmd), response, rsp_len);
  sd_spi_set_cs(1);
}

int sd_spi_init(spi_master_dev_t *spi)
{
  uint8_t spi_rsp[SPI_MAX_REPONSE_LEN] = {0};
  int retry_counter = 0;

  g_sd_spi = spi;
  sd_generate_crc_table();

  while (retry_counter < SD_CARD_MAX_INIT_FAILURES) {
    /* Send 80 clocks cycle to init card */

    sd_spi_set_cs(1);

    for (int i = 0; i < SPI_INIT_NUM_BYTES; i++)
      sd_spi_write(SPI_INIT_DUMMY_BYTE);

    sd_spi_set_cs(0);

    sd_spi_send_cmd(SPI_RESET_CMD, 0, spi_rsp, 1); 
    if (spi_rsp[0] != 1) {
      LOG_ERR("init bad response: 0x%x\n try: %d", spi_rsp[0],
        retry_counter);
      retry_counter++;
    }
    else {
      break;
    }
  }

  if (retry_counter == SD_CARD_MAX_INIT_FAILURES) {
    return -ENODEV;
  }

  sd_spi_send_cmd(SPI_SEND_IF_COND_CMD, 0x1AA, spi_rsp, SPI_MAX_REPONSE_LEN);
  sd_spi_send_cmd(SPI_APP_CMD, 0, spi_rsp, 1); 
  sd_spi_send_cmd(SPI_READ_OCR, 0, spi_rsp, SPI_MAX_REPONSE_LEN);
  if (spi_rsp[0] != 0) {
    LOG_ERR("invalid card state 0x%x\n ocr:0x%x", spi_rsp[0],
      *((uint32_t *)&spi_rsp[1]));
    return -ENODEV;
  }  

  if (((spi_rsp[1] & 0x40) == 0x40) && (spi_rsp[1] != 0xFF)) {
    printf("SdCard block addressing\n");
  } else {
    printf("SdCard byte addressing\n");
  }

//  sd_spi_send_cmd(SPI_SEND_CSD, 0);
}
