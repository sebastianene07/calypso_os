#include <board.h>
#include <scheduler.h>
#include <builtin_apps.h>

/*
 * os_appstart - initialize the system applications
 *
 * Assumptions:
 *   This function does not return..
 */
void os_appstart(void)
{
  /* Kick off the console application */

#ifdef CONFIG_CONSOLE_APP
  sched_create_task(console_main, CONFIG_CONSOLE_STACK_SIZE, 0, NULL, "Console");
#endif

  /* The scheduler already kicked of the Idle task which is reponsible for
   * manageing the system resources, entering sleep and calling the scheduler
   * code.
   */

  sched_run();
}
