#include "fatfs/source/ff.h"
#include "fatfs/source/diskio.h"
#include <stdio.h>

/* The Fat FS device shructure holding some file descriptiors to opened devices
 */

struct fatfs_device_s
{
  int mmc_fd;
  int ramdev_fd;
  int usb_fd;
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

  int mmc_fd = open("/dev/mmc0", 0);
  if (mmc_fd < 0) {
    return mmc_fd;
  }

  /* Store the fd in a global structure that will be used later on */

  g_fatfs_config.mmc_fd = mmc_fd;

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

  return 0;
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
