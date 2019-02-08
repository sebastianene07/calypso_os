#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <timer.h>

#include "uart.h"
#include "gpio.h"

unsigned int LED = 13;

extern int is_enabled;

int os_startup(void)
{
    uart_init();
    gpio_init();
    gpio_configure(LED);
    timer_init();

    gpio_toogle(1, LED);

    int code = 65;
    int incr = 0;

    while(1)
    {
      char *bau = malloc(100);
      memset(bau, code + incr, 10);
      bau[11] = 10;

      uart_send(bau, 12);

      if (is_enabled)
      {
        gpio_toogle(0, LED);
      }
      else
      {
        gpio_toogle(1, LED);
      }

      free(bau);

      incr = (incr + 1) % 26;
    }

    return 0;
}
