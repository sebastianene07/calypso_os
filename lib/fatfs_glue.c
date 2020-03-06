#include <board.h>
#include <stdio.h>

#include <storage/spi_sdcard.h>
#include <vfs.h>

#include "fatfs/source/ff.h"
#include "fatfs/source/diskio.h"

/****************************************************************************
 * Preprocessor Definitions
 ****************************************************************************/

/* The size of a sector in bytes */

#define SECTOR_SIZE_BYTES                (512U)

/****************************************************************************
 * Private Data
 ****************************************************************************/

/* The MTD operations structure */

static struct mtd_ops_s *g_mtd_ops;

/****************************************************************************
 * Private Methods
 ****************************************************************************/



/****************************************************************************
 * Public GLUE Methods that FatFS links to
 ****************************************************************************/

#if !FF_FS_READONLY && !FF_FS_NORTC
DWORD get_fattime(void)
{
  return 0;
}
#endif

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

  int ret = ioctl(mmc_fd, MTD_GET_OPS, (unsigned long)&g_mtd_ops);
  if (ret < 0) {
    printf("Error: %d cannot get SD SPI ops\n", ret);
  }

  close(mmc_fd);
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

  int ret = g_mtd_ops->mtd_read_sec(buff, sector, count * SECTOR_SIZE_BYTES);
  if (ret < 0) {
    return RES_PARERR;
  }

  return RES_OK;
}

DRESULT MMC_disk_write(const BYTE *buff, DWORD sector, BYTE count)
{
  /* Schedule the following operation on the initialization thread */

  int ret = g_mtd_ops->mtd_write_sec(buff, sector, count * SECTOR_SIZE_BYTES);
  if (ret < 0) {
    return RES_PARERR;
  }

  return 0;
}

DRESULT MMC_disk_ioctl(BYTE ctrl, void *buff)
{
  return 0;
}

/****************************************************************************
 * Public Methods
 ****************************************************************************/

