#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <timer.h>
#include <unistd.h>

#include <scheduler.h>

#include "uart.h"
#include "gpio.h"

unsigned int LED = 13;
unsigned int BUTTON_1 = 11;

void task_1(void)
{
  while(1)
  {
    char msg[] = "[TASK_1] running\n";
    uart_send(msg, 17);
  }
}

void task_2(void)
{
  while (1)
  {
    char msg[] = "[TASK_2] running\n";
    uart_send(msg, 17);
  }
}

void os_startup(void)
{
    uart_init();
    gpio_init();

    gpio_configure(LED, GPIO_DIRECTION_OUT);
    gpio_configure(BUTTON_1, GPIO_DIRECTION_IN);

    timer_init();

    sched_create_task(task_1, 256);
    sched_create_task(task_2, 256);

    while(1)
    {
      char msg[] = "[TASK_0] running\n";

      gpio_toogle(1, LED);

      usleep(500000); /* 500 MS */

      while (gpio_read(BUTTON_1) == 1)
      {
      }

      gpio_toogle(0, LED);

      usleep(500000); /* 500 MS */

      uart_send(msg, 17);
    }
 }
