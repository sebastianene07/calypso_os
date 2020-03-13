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

int console_cat(int argc, const char *argv[])
{
  if (argc < 2) {
    printf("No input file specified\r\n");
    return -EINVAL;
  }

  int fd = open(argv[1], 0);
  if (fd < 0) {
    printf("Error %d open %s\n", fd, argv[1]);
    return fd;
  }

  char buffer[80];
  int nread = 0;
  int ret;

  do {

    memset(buffer, 0, sizeof(buffer));
    ret = 0;
    nread = 0;

    while ((sizeof(buffer) - nread - 1) > 0 &&
           (ret = read(fd, buffer + nread, sizeof(buffer) - nread - 1)) > 0) {
      nread += ret;
    }

    printf("%s\r\n", buffer);
  } while (ret != 0);

  close(fd);
  return 0;
}
