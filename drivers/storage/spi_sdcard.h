#ifndef __SPI_SDCARD_H
#define __SPI_SDCARD_H

#include <board.h>
#include <spi.h>

typedef int (* sd_read_spi_sector)(uint8_t *buffer, uint16_t sector, size_t count);

typedef struct {
  sd_read_spi_sector read_spi_card;
} sd_spi_ops_t;


/****************************************************************************
 * Public Function Definitions
 ****************************************************************************/

/*
 * sd_spi_init - initialize a SPI sd card
 *
 * @spi - the SPI device that we will be using 
 *
 *  This function initializes an SD card in SPI mode
 *  and reads the device geometry upon success.
 */
int sd_spi_init(spi_master_dev_t *spi);

/*
 * sd_spi_read_logical_block - reads a logical block from the sd card
 *
 * @spi     - the SPI device that we will be using 
 * @buffer  - place where we store the data
 * @lba_index - logical block index
 * @offset_in_lba - the offset in a 512 byte logical block 
 * @requested_read_size - the number of bytes that we want to read
 *
 *  This function reads up to requested_read_size and returns 0 upon 
 *  success.
 */
int sd_spi_read_logical_block(spi_master_dev_t *spi, uint8_t *buffer, uint32_t lba_index, uint8_t offset_in_lba, size_t requested_read_size);

#endif /* __SPI_SDCARD_H */
