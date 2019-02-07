#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "uart.h"
#include "gpio.h"

unsigned int LED = 13;

int main()
{
    uart_init();
    gpio_init();
    gpio_configure(LED);

    gpio_toogle(1, LED);

    int code = 65;
    int incr = 0;

    while(1)
    {
      for(int c = 0; c < 5000000; c++);

      char *bau = malloc(100);
      memset(bau, code + incr, 10);
      bau[11] = 10;

      uart_send(bau, 12);

      gpio_toogle(0, LED);
      for(int c = 0; c < 5000000; c++);

      gpio_toogle(1, LED);

      free(bau);

      incr = (incr + 1) % 26;
    }

    return 0;
}
