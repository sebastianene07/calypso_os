#include "fatfs/source/ff.h"
#include "fatfs/source/diskio.h"

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
  return 0;
}

DSTATUS MMC_disk_status(void)
{
  return 0;
}

DRESULT MMC_disk_read(BYTE *buff, DWORD sector, BYTE count)
{
  return 0;
}

DRESULT MMC_disk_write(const BYTE *buff, DWORD sector, BYTE count)
{
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
