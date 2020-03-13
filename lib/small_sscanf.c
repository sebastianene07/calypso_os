#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

static int get_num_args(const char *fmt)
{
  int num_args = 0;

  while (*fmt != '\0')
  {
    fmt = strchr(++fmt, '%');
    num_args++;
  }
  return num_args;
}

static const char *get_next(const char *str, const char *format, bool is_match)
{
  while (*str != '\0' && *format != '\0')
  {
    if (!is_match && *str != *format)
    {
      return str;
    }
    else if (is_match)
    {
      while (*str != '\0' && *str != *format)
      {
        str++;
      }

      return str;
    }

    str++;
    format++;
  }

  return NULL;
}

static void copy_arg(const char *start, const char *end, char *arg_buffer, size_t len)
{
  memset(arg_buffer, 0, len);

  if (start == NULL)
  {
    return;
  }

  if (end == NULL)
  {
    strncpy(arg_buffer, start, len);
  }

  const char *start_it;
  for (start_it = start; start_it != end && *start_it != '\0' && len > 0; start_it++)
  {
    *arg_buffer = *start_it;
    ++arg_buffer;
    --len;
  }
}

int sscanf(const char *str, const char *format, ...)
{
  int changed_elements = 0;
  int num_args = get_num_args(format);
  va_list args;
  char arg_buffer[32];

  va_start(args, format);

  const char *fmt = format;
  for (int i = 0; i < num_args; ++i)
  {
    fmt = strchr(fmt, '%');
    if (*fmt == '\0')
      break;

    ++fmt;

    switch (*fmt)
    {
      case 'd':
      case 'i':
        {
          int *arg = va_arg(args, int *);
          const char *start = get_next(str, format, false);

          format = ++fmt;
          const char *end = get_next(start, format, true);
          str    = end;

          copy_arg(start, end, arg_buffer, sizeof(arg_buffer));
          *arg = atoi(arg_buffer);
          break;
        }
      case 'u':
        {
          unsigned int *arg = va_arg(args, unsigned int *);
          const char *start = get_next(str, format, false);
          format = ++fmt;
          const char *end = get_next(start, format, true);
          str    = end;

          copy_arg(start, end, arg_buffer, sizeof(arg_buffer));
          *arg = atoi(arg_buffer);
        }
        break;

      default:
        break;
    }
  }

  va_end(args);

  return changed_elements;
}
