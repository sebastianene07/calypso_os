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

sem_t sema;

void task_1(void)
{
  while(1)
  {
    sem_wait(&sema);
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

    timer_init();
    sem_init(&sema, 0, 0);

    sched_create_task(task_1, 1024);
    sched_create_task(task_2, 1024);

    while(1)
    {
      char msg[] = "[TASK_0] press button to unblock task 1\n";

      gpio_toogle(1, LED, 0);

      usleep(500000); /* 500 MS */
      uart_send(msg, 17);

      while (gpio_read(BUTTON_1, 0) == 1)
      {
      }
      sem_post(&sema);
      gpio_toogle(0, LED, 0);

      usleep(500000); /* 500 MS */
    }
 }
