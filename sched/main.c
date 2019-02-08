#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <timer.h>

#include <scheduler.h>
#include <core_cm4.h>

#include "uart.h"
#include "gpio.h"

unsigned int LED = 13;

extern int is_enabled;

volatile int change = 0;

void led_blink(void)
{

  while(1)
  {

    if (!change)
    {
      gpio_toogle(0, LED);
    }
    else
    {
      gpio_toogle(1, LED);
    }
    change = !change;
  }
}

int os_startup(void)
{
    uart_init();
    gpio_init();
    gpio_configure(LED);
    timer_init();

    int code = 65;
    int incr = 0;

    char *bau = malloc(100);

    __disable_irq();
    sched_create_task(led_blink, 1024);
    __enable_irq();

    while(1)
    {
      memset(bau, code + incr, 10);
      bau[11] = 10;

      uart_send(bau, 12);

      incr = (incr + 1) % 26;
    }

    return 0;
}
