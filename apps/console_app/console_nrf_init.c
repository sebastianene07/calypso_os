#include <board.h>
#include <console_main.h>

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>

#include <source/ff.h>
#include <vfs.h>
#include <stdio.h>

/* Nordic pplication entry point */ 
int nrf_softdevice_init(void);

int console_nrf_init(int argc, const char *argv[])
{
  /* Start the floating point hardware processor */
  SCB->CPACR |= (3UL << 20) | (3UL << 22);

  /* Attach interrupts to route events from soft device library */
  attach_int(GPIOTE_IRQn, GPIOTE_IRQHandler);
  attach_int(SWI2_EGU2_IRQn, SWI2_EGU2_IRQHandler);

  printf("Starting NRF soft device\n");
  nrf_softdevice_init();
 
  return 0;
}
