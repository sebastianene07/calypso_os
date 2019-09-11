#ifndef __STDLIB_H
#define __STDLIB_H

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#ifndef NULL
#define NULL (0)
#endif

#ifndef ARRAY_LEN
#define ARRAY_LEN(X)           (sizeof(X)/sizeof(X[0]))
#endif

/****************************************************************************
 * Public Types
 ****************************************************************************/

typedef unsigned int size_t;

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

void *malloc(size_t size);

void free(void *ptr);

void *calloc(size_t nmemb, size_t size);

void *realloc(void *ptr, size_t size);

void *reallocarray(void *ptr, size_t nmemb, size_t size);

int atoi(const char *nptr);

#endif /* __STDLIB_H */
