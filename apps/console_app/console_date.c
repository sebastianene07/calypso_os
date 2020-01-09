#include <board.h>
#include <console_main.h>

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>
#include <rtc.h>

#define SEC_OFFSET              (2)
#define MIN_OFFSET              (1)
#define HOUR_OFFSET             (0)

/* Num args for date set command */
#define NUM_ARGS_DATE_SET       (2)

/*
 * console_date - View/Set the current time
 *
 */
int console_date(int argc, const char *argv[])
{
  current_time_t my_time;
  int rtc_fd, ret;

  rtc_fd = open(CONFIG_RTC_PATH, 0);
  if (rtc_fd < 0)
  {
    return -EINVAL;
  }

  if (argc >= NUM_ARGS_DATE_SET)
  {
    if (!strcmp(argv[1], "set"))
    {
      /* expected hour input in this format hour:min:sec */

      sscanf(argv[2], "%d:%d:%d", &my_time.g_hours,
                                  &my_time.g_minute,
                                  &my_time.g_second);

      ret = ioctl(rtc_fd, SET_RTC_TIME_IO, (unsigned long)&my_time);
      if (ret != OK) {
        printf("error set RTC time %d\n", ret);
      }
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

  ret = read(rtc_fd, &my_time, sizeof(my_time));
  if (ret < 0)
  {
		printf("ERROR %d read RTC\n", ret);
    close(rtc_fd);
    return ret;
  }

  printf("Local time: %02u : %02u : %02u\n",
         my_time.g_hours,
         my_time.g_minute,
         my_time.g_second);

  close(rtc_fd);

  return 0;
}
