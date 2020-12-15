#include <board.h>
#include <assert.h>
#include <scheduler.h>
#include <stdbool.h>
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

  /* This function should not return, raise an error.
   * Possible error cases:
   *  - failed to initialize IdleTask
   *  - platform didn't implement cpu_restorecontext/cpu_savecontext
   *  - corrupted memory
  */

  assert(false);
}
