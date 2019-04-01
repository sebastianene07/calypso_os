#ifndef __STRING_H
#define __STRING_H

#include <stdlib.h>

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
void *memset(void *buf, int c, size_t n);

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
void *memcpy(void *dest, const void *src, size_t len);

/*
 * strlen - get the length of the string
 *
 * @s - the content of the string
 *
 *  Return the length of the string without the terminating character.
 *
 */
size_t strlen(const char *s);

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
char *strtok(char *str, const char *delim);

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
char *strtok_r(char *str, const char *delim, char **saveptr);

#endif /* __STRING_H */
