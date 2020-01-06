#include <board.h>
#include <console_main.h>

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>

int console_sleep(int argc, const char *argv[])
{
  if (argc <= 1) return 0;
  uint32_t milis = 0;

  sscanf(argv[1], "%d", &milis);
  usleep(milis * 100);
  return 0;
}
