#ifndef __SPI_H
#define __SPI_H

#include <board.h>
#include <stdint.h>
#include <stdlib.h>
#include <semaphore.h>

/* Configuration structures should be initialized with this */

#define INVALID_SPI_CFG           (-1)

/* SPI master clock frequency */

typedef enum spi_frequency_e
{
  SPI_M_FREQ_125_KBPS,
  SPI_M_FREQ_250_KBPS,
  SPI_M_FREQ_500_KBPS,
  SPI_M_FREQ_1_MBPS,
  SPI_M_FREQ_2_MBPS,
  SPI_M_FREQ_4_MBPS,
  SPI_M_FREQ_8_MBPS,
} spi_frequency_t;

/* SPI master CPOL, CPHA and order */

typedef enum spi_mode_e
{
  SPI_M_MODE_0,
  SPI_M_MODE_1,
  SPI_M_MODE_2,
  SPI_M_MODE_3,
} spi_mode_t;

/* SPI configuration structure */

typedef struct spi_master_config_s
{
  uint8_t miso_pin;
  uint8_t miso_port;

  uint8_t mosi_pin;
  uint8_t mosi_port;

  uint8_t sck_pin;
  uint8_t sck_port;

  uint8_t cs_pin;
  uint8_t cs_port;

  spi_frequency_t freq;
  spi_mode_t mode;
} spi_master_config_t;

/* SPI device */

typedef struct spi_master_dev_s
{
  void *priv;
  spi_master_config_t dev_cfg;
  uint8_t rx_spi_buffer[CONFIG_SPI_BUFFER_LEN];
  uint8_t tx_spi_buffer[CONFIG_SPI_BUFFER_LEN];
  sem_t notify_rx_avail;
  sem_t lock_device;
} spi_master_dev_t;

/* Send SPI data - blocking operation */

typedef void (* spi_send_cb)(spi_master_dev_t *dev, const void *data, size_t len);

/* Receive SPI data - blocking operation*/

typedef void (* spi_send_recv_cb)(spi_master_dev_t *dev, const void *data_tx, size_t len_tx, void *data_rx, size_t len_rx);

void spi_init(void);

#endif /* __SPI_H */
