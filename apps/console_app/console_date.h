#ifndef __CONSOLE_DATE_H
#define __CONSOLE_DATE_H

typedef struct local_time_s {
  uint8_t seconds;
  uint8_t min;
  uint8_t hour;
} local_time_t;

void get_local_time(local_time_t *local_time);

#endif /* __CONSOLE_DATE_H */
