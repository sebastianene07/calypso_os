#ifndef __SPI_SDCARD_H
#define __SPI_SDCARD_H

#include <board.h>
#include <spi.h>

typedef int (* sd_read_spi_sector)(uint8_t *buffer, uint16_t sector, size_t count);
typedef int (* sd_write_spi_sector)(uint8_t *buffer, uint16_t sector, size_t count);

/* This opearation struct is used by the FAT FS layer to call into the low
 * level block access functions from the sd card. The structure populated with
 * this function pointers is returned when we send an IOCTL on the /dev/sd_card
 * with GET_SD_SPI_OPS.
 */
typedef struct {
  sd_read_spi_sector read_spi_card;
  sd_write_spi_sector write_spi_card;
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

#endif /* __SPI_SDCARD_H */
