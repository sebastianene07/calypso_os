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
  /* Attach interrupts to route events from soft device library */
  attach_int(GPIOTE_IRQn, GPIOTE_IRQHandler);
  attach_int(POWER_CLOCK_IRQn, POWER_CLOCK_IRQHandler);
  attach_int(RADIO_IRQn, RADIO_IRQHandler);
  attach_int(SAADC_IRQn, SAADC_IRQHandler);
  attach_int(TIMER0_IRQn, TIMER0_IRQHandler);
  attach_int(TIMER1_IRQn, TIMER1_IRQHandler);
  attach_int(RTC0_IRQn, RTC0_IRQHandler);
  attach_int(WDT_IRQn, WDT_IRQHandler);

  /* ! Enabling the following interrupt may cause asserts - disabling it may 
   * cause context switch and uart receive interrupt to not work */
  attach_int(RTC1_IRQn, RTC1_IRQHandler);

  attach_int(SWI0_EGU0_IRQn, SWI0_EGU0_IRQHandler);
  attach_int(SWI1_EGU1_IRQn, SWI1_EGU1_IRQHandler);
  attach_int(SWI2_EGU2_IRQn, SWI2_EGU2_IRQHandler);
  attach_int(SWI3_EGU3_IRQn, SWI3_EGU3_IRQHandler);
  attach_int(SWI4_EGU4_IRQn, SWI4_EGU4_IRQHandler);
  attach_int(SWI5_EGU5_IRQn, SWI5_EGU5_IRQHandler);
  
  nrf_softdevice_init();
 
  return 0;
}
