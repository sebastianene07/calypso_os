#include <board.h>
#include <scheduler.h>
#include <rtc.h>

/****************************************************************************
 * Pre-Processor Definitions
 ****************************************************************************/

#define RTC_BASE               (0x4000B000)

#define TASKS_START            (0x00)
#define PRESCALER              (0x508)
#define INTENSET               (0x304)
#define INTENCLR               (0x308)
#define CC_0                   (0x540)
#define EVENTS_TICK            (0x100)

#define RTC_CONFIG(offset_r)                                              \
  ((*((volatile uint32_t *)(RTC_BASE + (offset_r)))))

#define TASKS_START_CFG        RTC_CONFIG(TASKS_START)
#define PRESCALER_CFG          RTC_CONFIG(PRESCALER)
#define INTENSET_CFG           RTC_CONFIG(INTENSET)
#define INTENCLR_CFG           RTC_CONFIG(INTENCLR)
#define CC_0_CFG               RTC_CONFIG(CC_0)
#define EVENTS_TICK_CFG        RTC_CONFIG(EVENTS_TICK)

#define PRESCALER_8_HZ         (4095)
#define COMPARE_1_HZ           (8)

/****************************************************************************
 * Private Defintions
 ****************************************************************************/

/* One tick represent 125 ms - generated from 8Hz event */

static volatile uint32_t g_rtc_ticks_ms;

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/*
 * rtc_interrupt - RTC interrupt callback
 *
 * Initialize the RTC module from LF clock
 */
static void rtc_interrupt(void)
{
  if (EVENTS_TICK_CFG == 1)
  {
    ++g_rtc_ticks_ms;
    EVENTS_TICK_CFG = 0;
  }
}

/*
 * rtc_init - initiailze the RTC module
 *
 * Initialize the RTC module from LF clock
 */
void rtc_init(void)
{
/* fRTC [kHz] = 32.768 / (PRESCALER + 1)
 * The PRESCALER should be 4095 for 8Hz tick - 125 ms counter period */

  PRESCALER_CFG = PRESCALER_8_HZ;
  INTENSET_CFG  = 0x01;

  disable_int();
  attach_int(RTC0_IRQn, rtc_interrupt);
  NVIC_EnableIRQ(RTC0_IRQn);
  enable_int();

  TASKS_START_CFG = 1;
}
