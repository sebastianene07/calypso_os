#include <board.h>

#include <s_heap.h>
#include <serial.h>
#include <vfs.h>
#include <os_start.h>

/****************************************************************************
 * Public Data
 ****************************************************************************/

/* Heap definition */

heap_t g_my_heap;

/* Linker script segments */

extern unsigned long _stext;
extern unsigned long _sbss;
extern unsigned long _sdata;
extern unsigned long _etext;
extern unsigned long _ebss;
extern unsigned long _edata;
extern unsigned long _srodata;

/****************************************************************************
 * Public Functions
 ****************************************************************************/

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
  unsigned long *src, *dst;

  /* Copy initialized variable data from flash to ram */

  src = &_etext;
  dst = &_sdata;
  while(dst < &_edata)
      *(dst++) = *(src++);

  /* Zero out bss segment */

  src = &_sbss;
  while(src < &_ebss)
      *(src++) = 0;

  /* Init dummy serial console */

  uart_low_init();
  uart_low_send("\n^-^\n");

  /* Initialize the HEAP memory */

  s_init(&g_my_heap,
         HEAP_START,
         HEAP_END,
         HEAP_BLOCK_SIZE);

  /* Initialize the scheduler */

  sched_init();

  /* Virtual file system initialization */

  vfs_init(NULL, 0);

  /* This function should be implemented by each board config. It contains
   * the board specific initialization logic and it initializes the drivers.
   */

  board_init();

  /* Start the application logic and spawn child tasks */

  os_appstart();
 }
