#ifndef __STDLIB_H
#define __STDLIB_H

typedef unsigned int size_t;

void *malloc(size_t size);

void free(void *ptr);

void *calloc(size_t nmemb, size_t size);

void *realloc(void *ptr, size_t size);

void *reallocarray(void *ptr, size_t nmemb, size_t size);

#endif /* __STDLIB_H */
