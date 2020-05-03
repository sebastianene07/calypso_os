#include <timer.h>

int timer_init(void)
{
  /* 32 bit timer width */

  TIMER_FUNCTION_REGISTER(TIMER_BITMODE) = 0x03;

  /* Set the prescaler  16 MhZ / 2 ^ (PRESCALER) */

  TIMER_FUNCTION_REGISTER(TIMER_PRESCALER) = 0x04;

  /* Set the timer mode to timer - automaticaly increment the internal counter */

	TIMER_FUNCTION_REGISTER(TIMER_MODE) = 0;

  /* Start timer */

  TIMER_FUNCTION_REGISTER(TIMER_TASK_START) = 0x01;

  return 0;
}
