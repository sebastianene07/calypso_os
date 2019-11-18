#include <board.h>
#include <stdio.h>

#include <storage/spi_sdcard.h>
#include <vfs.h>

#include "fatfs/source/ff.h"
#include "fatfs/source/diskio.h"

/* The Fat FS device shructure holding some file descriptiors to opened devices
 */

struct fatfs_device_s
{
  int mmc_fd;
  int ramdev_fd;
  int usb_fd;

  sd_spi_ops_t spi_sd_card_ops;
};

static struct fatfs_device_s g_fatfs_config;

#if !FF_FS_READONLY && !FF_FS_NORTC
DWORD get_fattime(void)
{
  return 0;
}
#endif

DSTATUS RAM_disk_initialize(void)
{
  return 0;
}

DSTATUS RAM_disk_status(void)
{
  return 0;
}

DRESULT RAM_disk_read(BYTE *buff, DWORD sector, BYTE count)
{
  return 0;
}

DRESULT RAM_disk_write(const BYTE *buff, DWORD sector, BYTE count)
{
  return 0;
}

DRESULT RAM_disk_ioctl(BYTE ctrl, void *buff)
{
  return 0;
}


DSTATUS MMC_disk_initialize(void)
{
  /* Open the MMC device. This function will be called from the initialization
   * context so the file descriptor should be available only in the context
   * of that task. The later operations like : read, write, ioctl ops should
   * schedule their requests on the same thread that called this function.
   */

  int mmc_fd = open(CONFIG_SD_SPI_NAME, 0);
  if (mmc_fd < 0) {
    printf("Error: %d cannot open MMC device\n", mmc_fd);
    return mmc_fd;
  }

  /* Store the fd in a global structure that will be used later on */

  g_fatfs_config.mmc_fd = mmc_fd;

  int ret = ioctl(mmc_fd, GET_SD_SPI_OPS, 
    (unsigned long)&g_fatfs_config.spi_sd_card_ops);
  if (ret < 0) {
    printf("Error: %d cannot get SD SPI ops\n", ret);
  }

  return 0;
}

DSTATUS MMC_disk_status(void)
{
  /* Schedule the following operation on the initialization thread */

  /* Verify if the file descriptor is opened if it's opened send an ioctl
   * to the device to verify if the disk is write protected.
   */

  return 0;
}

DRESULT MMC_disk_read(BYTE *buff, DWORD sector, BYTE count)
{
  /* Schedule the following operation on the initialization thread */

  return g_fatfs_config.spi_sd_card_ops.read_spi_card(buff, sector, count * 512);
}

DRESULT MMC_disk_write(const BYTE *buff, DWORD sector, BYTE count)
{
  /* Schedule the following operation on the initialization thread */

  return 0;
}

DRESULT MMC_disk_ioctl(BYTE ctrl, void *buff)
{
  return 0;
}


DSTATUS USB_disk_initialize(void)
{
  return 0;
}

DSTATUS USB_disk_status(void)
{
  return 0;
}

DRESULT USB_disk_read(BYTE *buff, DWORD sector, BYTE count)
{
  return 0;
}

DRESULT USB_disk_write(const BYTE *buff, DWORD sector, BYTE count)
{
  return 0;
}

DRESULT USB_disk_ioctl(BYTE ctrl, void *buff)
{
  return 0;
}
