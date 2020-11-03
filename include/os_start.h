#ifndef __OS_START
#define __OS_START

/*
 * os_appstart - initialize the starting apps
 *
 * Create the initial threads and sit in a loop to wait for pre-emption.
 */
void os_appstart(void);

/*
 * _start - initialize the OS resources
 *
 *  Mount the virtual file system and initialize the device drivers. Pass
 *  the execution to os_appstart to start spwanning the initial tasks.
 *  This function should not return and it should be called after the
 *  HEAP memory was initialized and the SysTick was configured to allow
 *  context switching.
 */
void __start(void);

#endif /* __OS_START */
