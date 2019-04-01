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
size_t strlen(const char *s)

#endif /* __STRING_H */
