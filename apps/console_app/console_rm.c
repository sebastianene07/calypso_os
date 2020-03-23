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

int console_rm(int argc, const char *argv[])
{
  if (argc < 2) {
    printf("No input file specified\r\n");
    return -EINVAL;
  }

  int ret = unlink(argv[1]);
  if (ret < 0) {
    printf("%d cannot remove %s file\n", ret, argv[1]);
  }
  return ret;
}  
