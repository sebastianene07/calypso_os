#include <board.h>

#include <vfs.h>
#include <os_start.h>

/*
 * os_startup - initialize the OS resources
 *
 *  Mount the virtual file system and initialize the device drivers. Pass
 *  the execution to os_appstart to start spwanning the initial tasks.
 *  This function should not return and it should be called after the
 *  HEAP memory was initialized and the SysTick was configured to allow
 *  context switching.
 */
void os_startup(void)
{
    vfs_init(NULL, 0);

    /* This function should be implemented by each board config. It contains
     * the board specific initialization logic and it initializes the drivers.
     */

    board_init();

    /* Start the application logic and spawn child tasks */

    os_appstart();
 }
