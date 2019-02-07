#include <string.h>
#include <stdint.h>

void *memset(void *s, int c, size_t n)
{
  uint8_t *ptr = (uint8_t *)s;

  for (int i = 0; i < n; i++)
    *(++ptr) = c;

  return s;
}
