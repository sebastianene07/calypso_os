#include <board.h>
#include <console_main.h>

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>

#include <source/ff.h>
#include <vfs.h>
#include <stdio.h>

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int console_mount(int argc, const char *argv[])
{
  if (argc != 4) {
    printf("Expected <type> = (\"FATFS\"|\"EXFAT\")\n"
           "         <mount_path> <mtd_dev_path\n");
    return -EINVAL;
  }

  int ret = mount(argv[1], argv[2], 0, (void *)argv[3]);
  if (ret < 0) {
    printf("Can't mount %s in path %s with MTD %s\n", argv[1], argv[2], argv[3]);
  }
  return ret;
}

int console_umount(int argc, const char *argv[]) {
  if (argc < 2) {
    printf("Expected mount_path to unmount\n");
    return -ENOSYS;
  }

  return umount(argv[1], 0);
}
