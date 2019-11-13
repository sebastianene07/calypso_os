#ifndef __SPI_SDCARD_H
#define __SPI_SDCARD_H

#include <board.h>
#include <spi.h>

int sd_spi_init(spi_master_dev_t *spi);

int sd_spi_read_logical_block(spi_master_dev_t *spi, uint8_t *buffer, uint32_t lba_index, uint8_t offset_in_lba, size_t requested_read_size);

#endif /* __SPI_SDCARD_H */
