#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <timer.h>
#include <unistd.h>
#include <semaphore.h>

#include <scheduler.h>

#include <uart.h>
#include <gpio.h>
#include <display.h>

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

    gpio_configure(LED, 0, GPIO_DIRECTION_OUT);
    gpio_configure(BUTTON_1, 0, GPIO_DIRECTION_IN);
//    display_init();

    timer_init();

    sched_create_task(task_1, 256);
    sched_create_task(task_2, 256);

    sem_t sema;
    sem_init(&sema, 0, 0);

    while(1)
    {
      char msg[] = "[TASK_0] running\n";

      gpio_toogle(1, LED, 0);

      usleep(500000); /* 500 MS */

      while (gpio_read(BUTTON_1, 0) == 1)
      {
      }

      gpio_toogle(0, LED, 0);

      usleep(500000); /* 500 MS */

      sem_wait(&sema);
      uart_send(msg, 17);
      sem_post(&sema);
    }
 }
