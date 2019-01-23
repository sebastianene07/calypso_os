#include <stdint.h>

#include "uart.h"
#include "gpio.h"

unsigned int LED = 13;

int main()
{
    uart_init();
    gpio_init();
    gpio_configure(LED);

    gpio_toogle(1, LED);

    while(1)
    {
      for(int c = 0; c < 5000000; c++);

      static char *bau = "Ce mai faci ?";

      uart_send(bau, 13);

      gpio_toogle(0, LED);
      for(int c = 0; c < 5000000; c++);

      gpio_toogle(1, LED);
    }
    return 0;
}
