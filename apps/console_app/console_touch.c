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

int console_touch(int argc, const char *argv[])
{
  if (argc < 2) {
    printf("No input file specified\r\n");
    return -EINVAL;
  }

  int fd = open(argv[1], O_CREATE);
  if (fd < 0) {
    printf("Error %d create %s\n", fd, argv[1]);
    return fd;
  }

  close(fd);
  return 0;
}
