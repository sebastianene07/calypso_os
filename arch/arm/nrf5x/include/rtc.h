#ifndef __RTC_H
#define __RTC_H

/* Ioctl to set rtc time */
#define SET_RTC_TIME_IO             (0)

typedef struct current_time_s {
  uint32_t g_second;
  uint32_t g_minute;
  uint32_t g_hours;
  uint32_t g_days;
} current_time_t;

void rtc_init(void);

#endif /* __RTC_H */
