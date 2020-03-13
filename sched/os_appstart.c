#include <board.h>
#include <scheduler.h>
#include <builtin_apps.h>

/*
 * os_appstart - initialize the starting apps
 *
 * Create the initial threads and sit in a loop to wait for pre-emption.
 */
void os_appstart(void)
{
#ifdef CONFIG_CONSOLE_APP
  sched_create_task(console_main, CONFIG_CONSOLE_STACK_SIZE, 0, NULL);
#endif

  while(1)
  {
    sched_run();
  }
}
