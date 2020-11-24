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
  irq_attach(GPIOTE_IRQn, GPIOTE_IRQHandler);
  irq_attach(POWER_CLOCK_IRQn, POWER_CLOCK_IRQHandler);
  irq_attach(RADIO_IRQn, RADIO_IRQHandler);
  irq_attach(SAADC_IRQn, SAADC_IRQHandler);
  irq_attach(TIMER0_IRQn, TIMER0_IRQHandler);
  irq_attach(TIMER1_IRQn, TIMER1_IRQHandler);
  irq_attach(RTC0_IRQn, RTC0_IRQHandler);
  irq_attach(WDT_IRQn, WDT_IRQHandler);

  /* ! Enabling the following interrupt may cause asserts - disabling it may
   * cause context switch and uart receive interrupt to not work */
  irq_attach(RTC1_IRQn, RTC1_IRQHandler);

  irq_attach(SWI0_EGU0_IRQn, SWI0_EGU0_IRQHandler);
  irq_attach(SWI1_EGU1_IRQn, SWI1_EGU1_IRQHandler);
  irq_attach(SWI2_EGU2_IRQn, SWI2_EGU2_IRQHandler);
  irq_attach(SWI3_EGU3_IRQn, SWI3_EGU3_IRQHandler);
  irq_attach(SWI4_EGU4_IRQn, SWI4_EGU4_IRQHandler);
  irq_attach(SWI5_EGU5_IRQn, SWI5_EGU5_IRQHandler);

  nrf_softdevice_init();

  return 0;
}
