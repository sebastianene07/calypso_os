#include <serial.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <semaphore.h>
#include <stdarg.h>

typedef enum integer_type_e
{
  ARG_INT32,
  ARG_UINT32,
  ARG_INT64,
  ARG_UINT64,
  ARG_HEXADEC,
} integer_type_t;

static void print_number(unsigned long arg, integer_type_t type)
{
  char arg_buffer[20];
  int index = 0;
  bool is_negative = false;

  if (type == ARG_INT32) {
    arg = arg & 0xFFFFFFFF;

    if (arg & (1 << 31)) {
      arg = 0xFFFFFFFF - arg + 1;
      is_negative = true;
    }
  } else if (type == ARG_INT64) { 
    if (((long)arg) < 0) {
      arg = 0xFFFFFFFFFFFFFFFF - arg;
      is_negative = true;
    }
  }

  memset(arg_buffer, 0, sizeof(arg_buffer));

  if (type != ARG_HEXADEC) {
    while (arg > 0) { 
      int digit = (arg % 10);
      arg_buffer[index++] = (char)(digit + 48);
      arg = arg / 10; 
    }
  } else {
    while (arg > 0) {
      int digit = (arg % 16);

      if (digit < 10)
         arg_buffer[index++] = (char)(digit + 48);
      else
         arg_buffer[index++] = (char)(digit + 55);
      arg = arg >> 4;
    }
  }

  if (is_negative)
    putchar('-');

  for (index; index >= 0; index--) {
    putchar(arg_buffer[index]);
  }
} 

static void print_string(char *arg)
{
  for (char *c = arg; *c != '\0'; c++)
    putchar((int)*c); 
}

void printf(const char *fmt, ...)
{
  va_list arg_list;
  char c;
  int val_1;
  unsigned int val_2;
  long val_3;
  unsigned long val_4;
  char *val_5;
  char val_6;

  sem_t *console_sema = get_console_sema();

  sem_wait(console_sema);
  va_start(arg_list, fmt);

  for (c = *fmt; c != '\0'; c = *(++fmt)) {

    if (c != '%') {
      putchar(c);
    } else {

      /* Check if the format is a known one */

      c = *(++fmt);

      if (c == '\0')
        break;
      else if (c == 'd') {
        val_1 = va_arg(arg_list, int);
        print_number(val_1, ARG_INT32);
      } else if (c == 'l' && *(fmt + 1) == 'd') {
        val_3 = va_arg(arg_list, long);
        fmt++;
        print_number(val_3, ARG_INT64);
      } else if (c == 'u') {
        val_2 = va_arg(arg_list, unsigned int);
        print_number(val_3, ARG_UINT32);
      } else if (c == 'l' && *(fmt + 1) == 'u') {
        val_4 = va_arg(arg_list, unsigned long);
        print_number(val_3, ARG_UINT64);
      } else if (c == 's') {
        val_5 = va_arg(arg_list, char *); 
        print_string(val_5);
      } else if (c == 'c') {
        val_6 = va_arg(arg_list, char);
        putchar(val_6); 
      } else if (c == 'x') {
        val_2 = va_arg(arg_list, unsigned int);
        print_number(val_2, ARG_HEXADEC); 
      } else if (c == 'l' && *(fmt + 1) == 'x') {
        val_4 = va_arg(arg_list, unsigned long);
        print_number(val_4, ARG_HEXADEC); 
      } else {
        putchar('?');
      }
    }
  }

 va_end(arg_list); 
 sem_post(console_sema);
}

int sprintf(char *out, const char *format, ...)
{
#if 0
  register int *varg = (int *)(&format);
	return print(&out, varg);
#endif
  return 0;
}

int snprintf(char *out, unsigned int len, const char *format, ...)
{
#if 0
	register int *varg = (int *)(&format);
	return print(&out, varg);
#endif
  return 0;
}
