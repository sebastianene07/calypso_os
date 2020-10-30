#include <board.h>
#include <console_main.h>

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>

#include <vfs.h>
#include <stdio.h>

int console_mkdir(int argc, const char *argv[])
{
  if (argc < 2) {
    printf("No input dir specified\r\n");
    return -EINVAL;
  }

  int ret = mkdir(argv[1], 0);
  if (ret < 0) {
    printf("%d create dir %s\n", ret, argv[1]);
    return ret;
  }

  return ret;
}
