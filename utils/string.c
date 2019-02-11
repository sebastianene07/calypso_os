#include <string.h>
#include <stdint.h>

void *memset(void *s, int c, size_t n)
{
  uint8_t *ptr = (uint8_t *)s;

  for (int i = 0; i < n; i++)
    *(++ptr) = c;

  return s;
}

void *memcpy(void *dest, const void *src, size_t len)
{
  for (int i = 0; i < len; i++)
  {
    *((uint8_t *)dest + i) = *((uint8_t *)src + i);
  }

  return dest;
}
