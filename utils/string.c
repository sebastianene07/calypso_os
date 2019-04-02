#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/*
 * memset - fill a buffer with a value
 *
 * @buf - the destination buffer
 * @c   - the character to fill with
 * @n   - the number of characters to write
 *
 *  Fill a buffer with a number of 'n' values with 'c'.
 *
 */
void *memset(void *s, int c, size_t n)
{
  uint8_t *ptr = (uint8_t *)s;

  for (int i = 0; i < n; i++)
    *(++ptr) = c;

  return s;
}

/*
 * memcpy - copies the memory content from one place to another
 *
 * @dest - the destination where we put the content
 * @src  - the source where we copy data from
 * @len  - the length of the data to copy from
 *
 *  Copies a memory region pointer by src of size len to the destination
 *  pointed by dest.
 *
 */
void *memcpy(void *dest, const void *src, size_t len)
{
  for (int i = 0; i < len; i++)
  {
    *((uint8_t *)dest + i) = *((uint8_t *)src + i);
  }

  return dest;
}

/*
 * strlen - get the length of the string
 *
 * @s - the content of the string
 *
 *  Return the length of the string without the terminating character.
 *
 */
size_t strlen(const char *s)
{
  unsigned int index = 0;

  for (index = 0;
       *(s + index) != '\0';
       index++);

  return index;
}

/*
 * strtok - tokenize the input str with the specified delim
 *
 * @str   - the content of the string
 * @delim - the delimiters buffer
 *
 *  The function breaks a string into a sequence of zero or more nonempty
 *  tokens.  On the first call to strtok(), the string to be parsed should
 *  be specified in str.  In each subsequent call that should parse the same
 *  string, str must be NULL.
 *
 *  The delim argument specifies a set of bytes that delimit the tokens in the
 *  parsed string. The caller may specify different strings in delim in
 *  successive calls that parse the same string.
 *
 *  Each call to strtok() returns a pointer to a null-terminated string
 *  containing the next token. This string does not include the delimiting
 *  byte.  If no more tokens are found, strtok() returns NULL
 *
 */
char *strtok(char *str, const char *delim)
{
  static char *olds;
  return strtok_r(str, delim, &olds);
}

/*
 * strtok_r - re-entrant version of strtok
 *
 * @str     - the content of the string
 * @delim   - the delimiters buffer
 * @saveptr - the context of strtok_r
 *
 *  The strtok_r() function is a reentrant version strtok(). The saveptr
 *  argument is a pointer to a char * variable that is used internally by
 *  strtok_r() in order to maintain context between successive calls that
 *  parse the same string. On the first call to strtok_r(), str should point
 *  to the string to be parsed, and the value of saveptr is ignored. In
 *  subsequent calls, str should be NULL, and saveptr should be unchanged
 *  since the previous call.
 *
 *  Different strings may be parsed concurrently using sequences of calls to
 *  strtok_r() that specify different saveptr arguments
 */
char *strtok_r(char *str, const char *delim, char **saveptr)
{
  if (str != NULL)
  {
    *saveptr = str;
  }
  else
  {
    if (*saveptr == NULL)
    {
      return NULL;
    }

    size_t delim_len = strlen(delim);
    char *begin = *saveptr;
    str = begin;
    bool delim_not_found = false;

    do {

      if (*str == '\0')
      {
        *saveptr = NULL;
        return begin;
      }

      for (int i = 0; i < delim_len; i++)
      {
        if (*str == delim[i])
        {
          delim_not_found = true;
          str--;
          break;
        }
      }

      str++;
    } while (!delim_not_found);

    *str     = '\0';
    *saveptr = str + 1;

    return begin;
  }

  return str;
}
