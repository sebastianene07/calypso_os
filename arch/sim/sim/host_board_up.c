#include <stdio.h>
#include <string.h>

void _start(void);

void host_console_putc(int c)
{
  char buffer[12];
  memset(buffer, 0, sizeof(buffer));

  int ret = snprintf(buffer, sizeof(buffer), "%c", c);
  if (ret > 0) {
    write(1, buffer, ret);
  }
}

int main(int argc, char **argv)
{
  _start();
  return 0;
}
