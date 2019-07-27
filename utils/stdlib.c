#include <stdlib.h>
#include <s_heap.h>
#include <string.h>

extern heap_t g_my_heap;

void *malloc(size_t size)
{
  return s_alloc(size, &g_my_heap);
}

void free(void *ptr)
{
  s_free(ptr, &g_my_heap);
}

void *calloc(size_t nmemb, size_t size)
{
  uint8_t *ptr = malloc(nmemb * size);
  if (ptr == NULL)
    return NULL;

  memset(ptr, 0, nmemb * size);
  return ptr;
}

void *realloc(void *ptr, size_t size)
{
  return s_realloc(ptr, size);
}

void *reallocarray(void *ptr, size_t nmemb, size_t size)
{
}
