#include <board.h>
#include <console_main.h>
#include <console_date.h>

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
static local_time_t g_clock;

/* The set clock hh:min:sec (offset) */
static uint32_t g_clock_offset[3];

static uint32_t g_tick_offset;

/*
 * console_date - View/Set the current time
 *
 */
int console_date(int argc, const char *argv[])
{
  uint64_t ticks = 0;
  int rtc_fd;

  rtc_fd = open(CONFIG_RTC_PATH, 0);
  if (rtc_fd < 0)
  {
    return -EINVAL;
  }

  int ret = read(rtc_fd, &ticks, sizeof(ticks));
  if (ret < 0)
  {
		printf("ERROR %d read RTC\n", ret);
    close(rtc_fd);
    return ret;
  }

  if (argc >= NUM_ARGS_DATE_SET)
  {
    if (!strcmp(argv[1], "set"))
    {
      /* expected hour input in this format hour:min:sec */
      g_tick_offset = ticks;

      sscanf(argv[2], "%d:%d:%d", &g_clock_offset[HOUR_OFFSET],
                                  &g_clock_offset[MIN_OFFSET],
                                  &g_clock_offset[SEC_OFFSET]);
    }
    else if (!strcmp(argv[1], "close"))
    {
      close(rtc_fd);
      rtc_fd = -1;
    }
    else
    {
      printf("Wrong cmd: time set <HH>:<MM>:<SS>\n");
    }
  }

  uint32_t seconds = (ticks - g_tick_offset) / 1000;
  uint32_t minutes = seconds / 60;
  uint32_t hour = minutes / 60;

  g_clock.seconds = (seconds + g_clock_offset[SEC_OFFSET]) % 60;
  g_clock.min     = (minutes + g_clock_offset[MIN_OFFSET]) % 60;
  g_clock.hour    = (hour + g_clock_offset[HOUR_OFFSET]) % 24;

  if (argc > 0) {
    printf("Local time: %02u : %02u : %02u\n",
           g_clock.hour,
           g_clock.min,
           g_clock.seconds);
  }

  close(rtc_fd);

  return 0;
}

void get_local_time(local_time_t *local_time)
{
  console_date(0, NULL);
  if (local_time != NULL) {
    memcpy(local_time, &g_clock, sizeof(local_time_t));
  }
}


