/*
 * This file is part of the CatOS distribution https://github.com/catos.
 *
 * Copyright (c) 2019 Sebastian Ene.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <spi.h>
#include <string.h>
#include <stdlib.h>
#include <gpio.h>

/****************************************************************************
 * Macros Defintions
 ****************************************************************************/

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

#define SPI_REG_SET(BASE, OFFSET) (*(volatile uint32_t *)((BASE) + (OFFSET)))

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

static void spi_send(spi_master_dev_t *dev, const void *data, size_t len);

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/*
 * spi_configure_pins - pin configuration function
 *
 * @cfg - pointer to the configuration structure
 * @base_spi_ptr - the base address of the SPI peripheral
 *
 */
static void spi_configure_pins(spi_master_config_t *cfg, uint32_t base_spi_ptr)
{
    gpio_configure(cfg->sck_pin, cfg->sck_port, GPIO_DIRECTION_OUT);
    gpio_configure(cfg->mosi_pin, cfg->mosi_port, GPIO_DIRECTION_OUT);
    gpio_configure(cfg->miso_pin, cfg->miso_port, GPIO_DIRECTION_IN);
    gpio_configure(cfg->cs_pin, cfg->cs_port, GPIO_DIRECTION_OUT);

    /* Set the master SPI SCK pin */

    SPI_REG_SET(base_spi_ptr, PSEL_SCK) = cfg->sck_pin | (cfg->sck_port << 5);
    /* Set the master MOSI pin */

    SPI_REG_SET(base_spi_ptr, PSEL_MOSI) = cfg->mosi_pin | (cfg->mosi_port << 5);

    /* Set the master MOSI pin */

    SPI_REG_SET(base_spi_ptr, PSEL_MISO) = cfg->miso_pin | (cfg->miso_port << 5);

    /* Set the master chip select pin */

    SPI_REG_SET(base_spi_ptr, PSEL_CSN) = cfg->cs_pin | (cfg->cs_port << 5);

  /* Select SPI MODE */

  SPI_REG_SET(base_spi_ptr, CONFIG)    = cfg->mode;

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

  SPI_REG_SET(base_spi_ptr, FREQUENCY) = freq_cfg;
  SPI_REG_SET(base_spi_ptr, ENABLE)    = 7;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/*
 * spi_init - entry point for the SPI initialization
 *
 * This function is called by the board initialization logic
 * to configure the SPI peripheral on the board.
 *
 */
void spi_init(void)
{
  static spi_master_dev_t spi_0 = {0};
  spi_0.priv = (void *)SPI_M0_BASE;

  sem_init(&spi_0.notify_rx_avail, 0, 0);
  sem_init(&spi_0.lock_device, 0, 1); 

  /* Pin configuration : SPI 0 */

  spi_0.dev_cfg = (struct spi_master_config_s) {
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
  };

  spi_configure_pins(&spi_0.dev_cfg, SPI_M0_BASE);
  uint8_t test_data[] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
  while (1) {
    spi_send(&spi_0, test_data, sizeof(test_data));
  }
}

/*
 * spi_send - sends data on the SPI bus 
 *
 * @dev  - the master device that initiates the transfer
 * @data - the data to send
 * @len  - the length of the data beeing sent
 *
 * This function configures the SPIm peripheral to send a chunk of data.
 */
static void spi_send(spi_master_dev_t *dev, const void *data, size_t len)
{
  uint32_t base_spi_reg = (uint32_t)dev->priv;

  sem_wait(&dev->lock_device);

  memcpy(dev->tx_spi_buffer, data, len);

  SPI_REG_SET(base_spi_reg, EVENTS_STARTED) = 0;
  SPI_REG_SET(base_spi_reg, EVENTS_END) = 0;
  SPI_REG_SET(base_spi_reg, RXD_PTR)    = (uint32_t)dev->rx_spi_buffer;
  SPI_REG_SET(base_spi_reg, RXD_MAXCNT) = 0;
  SPI_REG_SET(base_spi_reg, TXD_PTR)    = (uint32_t)dev->tx_spi_buffer;
  SPI_REG_SET(base_spi_reg, TXD_MAXCNT) = len;
  SPI_REG_SET(base_spi_reg, ORC)        = 0;
  SPI_REG_SET(base_spi_reg, TASKS_START) = 1;

  while (SPI_REG_SET(base_spi_reg, EVENTS_STARTED) == 0) { }

  while (SPI_REG_SET(base_spi_reg, EVENTS_END) == 0) { }

  SPI_REG_SET(base_spi_reg, TASKS_START) = 0;

  sem_post(&dev->lock_device);
}
