#include <stdio.h>
#include "pico/stdlib.h"
#include <stdarg.h>

void __start(void);

int vprintf(const char * restrict format, va_list ap)
{
  return 0;
}

int puts(const char *s)
{
  return 0;
}

int main(void)
{
  __start();
  return 0;
}

int bsp_main() {
    setup_default_uart();
    printf("[RPIPICO] Uart initialized\n");
    return 0;
}
