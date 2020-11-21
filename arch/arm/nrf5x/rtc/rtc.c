#include <board.h>
#include <irq_manager.h>
#include <scheduler.h>
#include <rtc.h>
#include <vfs.h>
#include <errno.h>
#include <string.h>

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
#define PRESCALER_1kHZ         (32)
#define COMPARE_1_HZ           (8)

/****************************************************************************
 * Private Function Definitions
 ****************************************************************************/

static int rtc_open(struct opened_resource_s *priv, const char *pathname, int flags, mode_t mode);
static int rtc_close(struct opened_resource_s *priv);
static int rtc_read(struct opened_resource_s *priv, void *buf, size_t count);
static int rtc_ioctl(struct opened_resource_s *priv, unsigned long request, unsigned long arg);


/****************************************************************************
 * Private Defintions
 ****************************************************************************/

/* One tick represent 125 ms - generated from 8Hz event */

static volatile uint32_t g_rtc_ticks_ms;

volatile uint32_t g_seconds;
volatile uint32_t g_minutes;
volatile uint32_t g_hours;
volatile uint32_t g_days;

/* Supported RTC operation */

static struct vfs_ops_s g_rtc_ops = {
  .open  = rtc_open,
  .close = rtc_close,
  .ioctl = rtc_ioctl,
  .read  = rtc_read,
};

/* Opened counter */
static volatile int g_opened_count = 0;

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

    if (g_rtc_ticks_ms % 8 == 0) {
      g_rtc_ticks_ms = 0;

      g_seconds++;

      if (g_seconds % 60 == 0) {

        g_seconds = 0;
        g_minutes++;

        if (g_minutes % 60 == 0) {

          g_minutes = 0;
          g_hours++;

          if (g_hours % 24 == 0) {
            g_hours = 0;
            g_days++;
          }
        }
      }
    }
    EVENTS_TICK_CFG = 0;
  }
}

/*
 * rtc_open - open the RTC module
 *
 * Initialize the RTC module from LF clock
 */
static int rtc_open(struct opened_resource_s *res, const char *pathname,
                    int flags,
                    mode_t mode)
{
  /* fRTC [Hz] = 32768 / (PRESCALER + 1)
   * The PRESCALER should be 4095 for 8Hz tick - 125 ms counter period */

  /* 1kHz -> perioada 1 ms -> 511875 */

  irq_state_t irq_state = cpu_disableint();

  if (g_opened_count == 0) {
    PRESCALER_CFG = PRESCALER_8_HZ;
    INTENSET_CFG  = 0x01;

    irq_attach(RTC0_IRQn, rtc_interrupt);
    NVIC_EnableIRQ(RTC0_IRQn);
    TASKS_START_CFG = 1;
  }

  ++g_opened_count;
  cpu_enableint(irq_state);

  return OK;
}

static int rtc_close(struct opened_resource_s *priv)
{
  irq_state_t irq_state = cpu_disableint();

  --g_opened_count;

  if (g_opened_count == 0) {
    TASKS_START_CFG = 0;
    NVIC_DisableIRQ(RTC0_IRQn);
    irq_attach(RTC0_IRQn, NULL);
  }

  cpu_enableint(irq_state);
  return 0;
}

static int rtc_read(struct opened_resource_s *priv, void *buf, size_t count)
{
  if (buf == NULL || count != sizeof(current_time_t))
  {
    return -EINVAL;
  }

  current_time_t *user_ptr = (current_time_t *)buf;
  user_ptr->g_second  = g_seconds;
  user_ptr->g_minute  = g_minutes;
  user_ptr->g_hours   = g_hours;
  user_ptr->g_days    = g_days;

  return sizeof(current_time_t);
}

static int rtc_ioctl(struct opened_resource_s *priv, unsigned long request, unsigned long arg)
{
  switch(request) {

    case SET_RTC_TIME_IO:
      {
        current_time_t *new_rtc_time = (current_time_t *)arg;

        irq_state_t irq_state = cpu_disableint();
        g_seconds = new_rtc_time->g_second;
        g_minutes = new_rtc_time->g_minute;
        g_hours   = new_rtc_time->g_hours;
        g_rtc_ticks_ms = 0;
        cpu_enableint(irq_state);
        return OK;
      }

    default:
      break;
  }
  return -ENOSYS;
}

/*
 * rtc_register - Register the RTC module
 *
 * Register the RTC module from LF clock
 */
static int rtc_register(const char *name)
{
  return vfs_register_node(name, strlen(name), &g_rtc_ops, VFS_TYPE_CHAR_DEVICE,
                           NULL);
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/*
 * rtc_init - initiailze the RTC module
 *
 * Initialize the RTC module from LF clock
 */
void rtc_init(void)
{
  rtc_register("/dev/rtc0");
}
