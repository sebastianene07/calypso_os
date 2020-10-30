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

int console_echo(int argc, const char *argv[])
{
  if (argc < 2) {
    return -EINVAL;
  }

  printf("%s\r\n", argv[1]);

  return 0;
}
