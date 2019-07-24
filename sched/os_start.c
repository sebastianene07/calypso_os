#include <board.h>

#include <uart.h>
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
    /* Init dummy serial console */

    uart_low_init();
    uart_low_send("^-^\n");

    /* Virtual file system initialization */

    vfs_init(NULL, 0);
    uart_low_send("Cat OS v0.0.1\n");

    /* This function should be implemented by each board config. It contains
     * the board specific initialization logic and it initializes the drivers.
     */

    board_init();

    /* Start the application logic and spawn child tasks */

    os_appstart();
 }
