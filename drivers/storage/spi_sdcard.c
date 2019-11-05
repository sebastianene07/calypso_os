#include <spi_sdcard.h>
#include <stdio.h>
#include <errno.h>
#include <gpio.h>
#include <string.h>

#define LOG_ERR(msg, ...)  printf("[sd_spi] Error:"msg"\r\n", ##__VA_ARGS__)
#define LOG_INFO(msg, ...) printf("[sd_spi] Info:"msg"\r\n", ##__VA_ARGS__)

/****************************************************************************
 * Macro Definitions
 ****************************************************************************/

#define SPI_INIT_CLOCK_CYCLES                 (80)
#define SPI_INIT_NUM_BYTES                    (SPI_INIT_CLOCK_CYCLES / 8)
#define SPI_INIT_DUMMY_BYTE                   (0xFF)

/* Command definitions */

#define SPI_CMD_LEN                           (6)
#define SPI_MAX_REPONSE_LEN                   (20)
#define SPI_MAX_RESET_RETRIES                 (60)

#define SPI_RESET_CMD                         (0)
#define SPI_SEND_OP_COND_CMD                  (1)
#define SD_SET_WBLON                          (7)
#define SPI_SEND_IF_COND_CMD                  (8)        
#define SPI_SEND_CSD                          (9)
#define SPI_APP_CMD                           (55)
#define SPI_READ_OCR                          (58)
#define SPI_SD_SEND_OP_COND_CMD               (41)
#define SD_CARD_MAX_INIT_FAILURES             (10)
#define SD_CSD_RESPONSE_LEN                   (18)

#define SD_TOKEN_START                        (0xFE)
#define SD_TOKEN_START_MULTI_BLOCK            (0xFC)
#define SD_TOKEN_STOP_TRANSMISSION            (0xFD)
#define SD_TOKEN_DATA_ACCEPTED                (0x05)
#define SD_TOKEN_FLOATING_BUS                 (0xFF)

/****************************************************************************
 * Private Types
 ****************************************************************************/

enum card_type_e {
  SD_BAD_TYPE,
  SD_LOW_CAPACITY_CARD,
  SD_HIGH_CAPACITY_CARD,
};

struct sd_card_s {
  size_t size;
  size_t num_blocks;
  enum card_type_e type;
};

/****************************************************************************
 * Private Data
 ****************************************************************************/

/* Local reference to the SPI driver in master mode */
static spi_master_dev_t *g_sd_spi;

/* CRC7 table */
static uint8_t g_crc7_table[256];

/* Current sd card instance */
static struct sd_card_s g_sd_card;

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

static uint8_t g_sd_resp[SPI_CMD_LEN + 9];
static int g_rsp_index = 0;

static void sd_spi_send_cmd(uint8_t cmd, uint32_t arguments)
{
  uint8_t sd_card_cmd[SPI_CMD_LEN];
  uint8_t crc7 = 0;
  uint8_t res = 0;

  g_rsp_index = 0;
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

  memset(g_sd_resp, 0xFF, sizeof(g_sd_resp)); 
  spi_send_recv(g_sd_spi, sd_card_cmd, sizeof(sd_card_cmd), g_sd_resp, sizeof(g_sd_resp)); 

  sd_spi_set_cs(1);

  for (int i = 6; i < sizeof(g_sd_resp); i++) {
    printf("\r\n0x%x,", g_sd_resp[i]);
  }
  printf("\r\n");
}

static void sd_spi_read(uint8_t *buffer, size_t expected_len)
{
  int n = 9;
  int i = 0;
  uint8_t resp = 0xFF;

  do {
    resp = sd_spi_write(0xFF);
    if (resp != 0xFF && i < expected_len) {
      buffer[i++] = resp;
      n = 9;
    }
  } while (--n > 0 && i < expected_len);
}

static uint8_t sd_read_byte_ignore_char(uint8_t ignore)
{
  for (g_rsp_index; g_rsp_index < sizeof(g_sd_resp); g_rsp_index++)
    if (g_sd_resp[g_rsp_index] != ignore) {
      g_rsp_index++;
      return g_sd_resp[g_rsp_index - 1];
    }

  return 0xFF;
}

int sd_spi_init(spi_master_dev_t *spi)
{
  uint8_t spi_rsp;
  int retry_counter = 0;

  g_sd_spi = spi;
  sd_generate_crc_table();

  sd_spi_set_cs(1);

  for (int i = 0; i < SPI_INIT_NUM_BYTES; i++)
    sd_spi_write(SPI_INIT_DUMMY_BYTE);

  LOG_INFO("Send CMD0 0x%x\n", SPI_RESET_CMD);
  sd_spi_send_cmd(SPI_RESET_CMD, 0);
  g_rsp_index = 6;
  if ((spi_rsp = sd_read_byte_ignore_char(0xFF)) != 1) {
    LOG_ERR("init bad response: 0x%x\n", spi_rsp);
    return -ENODEV;
  }
  LOG_INFO("Response: 0x%x", spi_rsp);

  LOG_INFO("Send CMD8 0x%x\n", SPI_SEND_IF_COND_CMD);
  sd_spi_send_cmd(SPI_SEND_IF_COND_CMD, 0x1AA);
  g_rsp_index = 6;
  spi_rsp = sd_read_byte_ignore_char(0xFF);
  if ((spi_rsp == 0xFF) || (spi_rsp == 0x05)) {
    sd_spi_set_cs(1);
  } else {

    sd_spi_set_cs(1);

    spi_rsp = sd_read_byte_ignore_char(0x00);
    if (spi_rsp != 0x01) {
      LOG_ERR("Voltage not accepted 0x%d\n", spi_rsp);
      return -EINVAL;
    }

    spi_rsp = sd_read_byte_ignore_char(0xFF);
    if (spi_rsp != 0xAA) {
      LOG_ERR("Wrong test patern 0x%x\n", spi_rsp);
      return -EINVAL;
    }
  }

  int counter = 0;
  uint8_t cmd = SPI_SD_SEND_OP_COND_CMD;

  LOG_INFO("Send CMD55");
  sd_spi_send_cmd(SPI_APP_CMD, 0);
  spi_rsp = sd_read_byte_ignore_char(0xFF);

  LOG_INFO("Response: 0x%x", spi_rsp); 

  do {
    counter++;
    LOG_INFO("Send %s", cmd == SPI_SD_SEND_OP_COND_CMD ? "ACMD41" : "CMD 1");
    sd_spi_send_cmd(cmd, 0x40000000);
    spi_rsp = sd_read_byte_ignore_char(0xFF);

    LOG_INFO("Response: 0x%x", spi_rsp);

    if ((spi_rsp & 0xF) == 0x05) {
      cmd = SPI_SEND_OP_COND_CMD; 
    } else {
      cmd = SPI_SD_SEND_OP_COND_CMD;
    }

  } while (spi_rsp != 0 && counter < SPI_MAX_RESET_RETRIES);
 
  if (counter == SPI_MAX_RESET_RETRIES) {
    LOG_ERR("cannot reset 0x%x\r\n", spi_rsp);
    return -EINVAL; 
  }

  sd_spi_send_cmd(SPI_READ_OCR, 0);
  g_rsp_index = 6;
  spi_rsp = sd_read_byte_ignore_char(0xFF);
  if (spi_rsp != 0) {
    LOG_ERR("get capacity response 0x%x", spi_rsp);
    return -EINVAL;
  }

  spi_rsp = sd_read_byte_ignore_char(0xFF);
  if (((spi_rsp & 0x40) == 0x40) && (spi_rsp != 0xFF)) {
    LOG_INFO("SdCard block addressing 0x%x\n", spi_rsp);
    g_sd_card.type = SD_HIGH_CAPACITY_CARD;
  } else {
    LOG_ERR("byte addressing  0x%x\n", spi_rsp);
    g_sd_card.type = SD_LOW_CAPACITY_CARD;
    return -EINVAL; 
  }

  LOG_INFO("Send CMD9\n");
  sd_spi_send_cmd(SPI_SEND_CSD, 0);
  spi_rsp = sd_read_byte_ignore_char(0xFF);
  if (spi_rsp != 0) {
    LOG_ERR("read CSD register failed 0x%x\n", spi_rsp);
    return -ENODEV;
  }

  uint8_t sd_capacity_info[SD_CSD_RESPONSE_LEN];
  sd_read_byte_ignore_char(0xFE);
  g_rsp_index++;
  int remaining_bytes = sizeof(g_sd_resp) - g_rsp_index;

  printf("\r\n Found 0xFE on %d pos\n", remaining_bytes);

  memcpy(sd_capacity_info, g_sd_resp + g_rsp_index, remaining_bytes);
  sd_spi_set_cs(0);
  sd_spi_read(sd_capacity_info + remaining_bytes,
              SD_CSD_RESPONSE_LEN - remaining_bytes);
  sd_spi_set_cs(1); 

   for (int i = 0; i < SD_CSD_RESPONSE_LEN; i++) {
    printf("\r\n0x%x,", sd_capacity_info[i]);
  }

  if (g_sd_card.type == SD_HIGH_CAPACITY_CARD) {
    g_sd_card.num_blocks = sd_capacity_info[9] + 1;
    g_sd_card.num_blocks += (uint32_t)(sd_capacity_info[8] << 8);
    g_sd_card.num_blocks += (uint32_t)(sd_capacity_info[7] & 0x0F) << 12;
    g_sd_card.size = 524288 * g_sd_card.num_blocks;
  } else {
    g_sd_card.size = (((uint16_t)(sd_capacity_info[6] & 0x03) << 10) | 
      ((uint16_t)(sd_capacity_info[7] << 2)) |
      ((uint16_t)(sd_capacity_info[8] & 0xC0) >> 6)) + 1;
    g_sd_card.size = g_sd_card.size << (((sd_capacity_info[9] & 0x03) << 1) | 
      ((sd_capacity_info[10] & 0x80) >> 7) + 2);
    g_sd_card.size = g_sd_card.size << (sd_capacity_info[5] & 0x0F);

    g_sd_card.num_blocks = g_sd_card.size / 524288;
    sd_spi_set_cs(1);
  }

  LOG_INFO("card blocks: %d\n", g_sd_card.num_blocks);
  return OK;
}
