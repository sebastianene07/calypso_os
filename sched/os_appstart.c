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
# ifdef CONFIG_SEPARATE_BUNDLES
    /* TODO bootsrap some initrd code */
# else
    sched_create_task(main, CONFIG_CONSOLE_STACK_SIZE, 0, NULL, "Console");
# endif /* CONFIG_SEPARATE_BUNDLES */
#endif /* CONFIG_CONSOLE_APP */

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
