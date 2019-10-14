#include <board.h>
#include <console_main.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>

#define SEC_OFFSET              (2)
#define MIN_OFFSET              (1)
#define HOUR_OFFSET             (0)

/* Num args for date set command */
#define NUM_ARGS_DATE_SET       (2)

/* The clock that will be shown hh:min:sec */
static uint8_t g_clock[3];

/* The set clock hh:min:sec (offset) */
static uint32_t g_clock_offset[3];

static uint32_t tick_offset;

/*
 * console_date - View/Set the current time
 *
 */
int console_date(int argc, const char *argv[])
{
  uint32_t ticks = 0;

  int rtc_fd = open(CONFIG_RTC_PATH, 0);
  if (rtc_fd < 0)
  {
    return -EINVAL;
  }

  int ret = read(rtc_fd, &ticks, sizeof(ticks));
  if (ret < 0)
  {
    return ret;
  }

  if (argc >= NUM_ARGS_DATE_SET)
  {
    if (!strcmp(argv[1], "set"))
    {
      /* expected hour input in this format hour:min:sec */
      tick_offset = ticks;

      sscanf(argv[2], "%d:%d:%d", &g_clock_offset[HOUR_OFFSET],
                                  &g_clock_offset[MIN_OFFSET],
                                  &g_clock_offset[SEC_OFFSET]);
    }
    else
    {
      printf("Wrong cmd: time set <HH>:<MM>:<SS>\n");
      goto free_fd;
    }
  }

  uint32_t seconds = (ticks - tick_offset) >> 3;
  uint32_t minutes = seconds / 60;
  uint32_t hour = minutes / 60;

  g_clock[SEC_OFFSET] = (seconds + g_clock_offset[SEC_OFFSET]) % 60;
  g_clock[MIN_OFFSET] = (minutes + g_clock_offset[MIN_OFFSET]) % 60;
  g_clock[HOUR_OFFSET] = (hour + g_clock_offset[HOUR_OFFSET]) % 24;

  printf("Local time: %02u : %02u : %02u\n", g_clock[HOUR_OFFSET],
    g_clock[MIN_OFFSET], g_clock[SEC_OFFSET]);

free_fd:
  close(rtc_fd);
  return 0;
}
