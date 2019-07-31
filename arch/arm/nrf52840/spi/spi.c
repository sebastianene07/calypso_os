#include <spi.h>
#include <board.h>
#include <stdlib.h>

#define SPI_M0_BASE            (0x40003000)

/* Register offsets */

#define TASKS_START            (0x010)
#define TASKS_STOP             (0x014)
#define TASKS_SUSPEND          (0x01C)
#define TASKS_RESUME           (0x020)
#define EVENTS_STOPPED         (0x104)
#define EVENTS_ENDRX           (0x110)
#define EVENTS_END             (0x118)
#define EVENTS_ENDTX           (0x120)
#define EVENTS_STARTED         (0x14C)

#define SHORTS                 (0x200)
#define INTENSET               (0x304)
#define INTENCLR               (0x308)
#define STALLSTAT              (0x400)
#define ENABLE                 (0x500)
#define PSEL_SCK               (0x508)
#define PSEL_MOSI              (0x50C)
#define PSEL_MISO              (0x510)
#define PSEL_CSN               (0x514)
#define FREQUENCY              (0x524)
#define RXD_PTR                (0x534)
#define RXD_MAXCNT             (0x538)
#define RXD_AMOUNT             (0x53C)
#define RXD_LIST               (0x540)
#define TXD_PTR                (0x544)
#define TXD_MAXCNT             (0x548)
#define TXD_AMOUNT             (0x54C)
#define TXD_LIST               (0x550)
#define CONFIG                 (0x554)
#define IFTIMING_RXDELAY       (0x560)
#define IFTIMING_CSNDUR        (0x564)
#define ORC                    (0x5C0)

/* Helper macro */

#define SPI_REG_SET(BASE, OFFSET) (*(uint32_t *)((BASE) + (OFFSET)))

static uint8_t* g_spi_peripherals[] =
{
  (uint8_t *)SPI_M0_BASE,
};

void spi_initialize(void)
{
}

int spi_configure(spi_master_config_t *cfg, uint8_t peripheral_id)
{
  if (peripheral_id > ARRAY_LEN(g_spi_peripherals) ||
      cfg == NULL)
  {
    return -1;
  }

  if (cfg->sck_pin != INVALID_SPI_CFG &&
      cfg->sck_port != INVALID_SPI_CFG)
  {
    /* Set the master SPI SCK pin */

    SPI_REG_SET(SPI_M0_BASE, PSEL_SCK) = cfg->sck_pin | (cfg->sck_port << 5);
  }

  if (cfg->mosi_pin != INVALID_SPI_CFG &&
      cfg->mosi_port != INVALID_SPI_CFG)
  {
    /* Set the master MOSI pin */

    SPI_REG_SET(SPI_M0_BASE, PSEL_MOSI) = cfg->mosi_pin | (cfg->mosi_port << 5);
  }

  if (cfg->miso_pin != INVALID_SPI_CFG &&
      cfg->miso_port != INVALID_SPI_CFG)
  {
    /* Set the master MOSI pin */

    SPI_REG_SET(SPI_M0_BASE, PSEL_MISO) = cfg->miso_pin | (cfg->miso_port << 5);
  }

  if (cfg->cs_pin != INVALID_SPI_CFG &&
      cfg->cs_port != INVALID_SPI_CFG)
  {
    /* Set the master chip select pin */

    SPI_REG_SET(SPI_M0_BASE, PSEL_CSN) = cfg->cs_pin | (cfg->cs_port << 5);
  }

  /* Select SPI MODE */

  SPI_REG_SET(SPI_M0_BASE, CONFIG)    = cfg->mode;

  uint32_t freq_cfg = 0x00;
  switch (cfg->freq)
  {
    case SPI_M_FREQ_125_KBPS:
      freq_cfg = 0x02000000;
      break;

    case SPI_M_FREQ_250_KBPS:
      freq_cfg = 0x04000000;
      break;

    case SPI_M_FREQ_500_KBPS:
      freq_cfg = 0x08000000;
      break;

    case SPI_M_FREQ_1_MBPS:
      freq_cfg = 0x10000000;
      break;

    case SPI_M_FREQ_2_MBPS:
      freq_cfg = 0x20000000;
      break;

    case SPI_M_FREQ_4_MBPS:
      freq_cfg = 0x40000000;

    case SPI_M_FREQ_8_MBPS:
      freq_cfg = 0x80000000;

    default:
      break;
  }

  SPI_REG_SET(SPI_M0_BASE, FREQUENCY) = freq_cfg;
  SPI_REG_SET(SPI_M0_BASE, ENABLE)     = 7;
}

#define SPI_TX_BUF_SIZXE (2)
static uint8_t g_rx_spi_buffer[SPI_TX_BUF_SIZXE];

void spi_send(void *data, uint32_t len)
{
  SPI_REG_SET(SPI_M0_BASE, EVENTS_ENDTX) = 0;
  SPI_REG_SET(SPI_M0_BASE, RXD_PTR)    = (uint32_t)&g_rx_spi_buffer[0];
  SPI_REG_SET(SPI_M0_BASE, RXD_MAXCNT) = 0;
  SPI_REG_SET(SPI_M0_BASE, TXD_PTR)    = (uint32_t)data;
  SPI_REG_SET(SPI_M0_BASE, TXD_MAXCNT) = len;
  SPI_REG_SET(SPI_M0_BASE, ORC)        = 0;
  SPI_REG_SET(SPI_M0_BASE, TASKS_START) = 1;

  while (SPI_REG_SET(SPI_M0_BASE, EVENTS_ENDTX) == 0);

  SPI_REG_SET(SPI_M0_BASE, TASKS_START) = 0;
}
