#include <stdlib.h>
#include <s_heap.h>
#include <string.h>
#include <semaphore.h>

/****************************************************************************
 * Public Data
 ****************************************************************************/

extern heap_t g_my_heap;
extern sem_t g_heap_sema;

/****************************************************************************
 * Public Functions
 ****************************************************************************/

void *malloc(size_t size)
{
  void *new_mem = NULL;
  sem_wait(&g_heap_sema);
  new_mem = s_alloc(size, &g_my_heap);
  sem_post(&g_heap_sema);
  return new_mem;
}

void free(void *ptr)
{
  sem_wait(&g_heap_sema);
  s_free(ptr, &g_my_heap);
  sem_post(&g_heap_sema);
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
  void *new_mem = NULL;
  sem_wait(&g_heap_sema);
  new_mem = s_realloc(ptr, size, &g_my_heap);
  sem_post(&g_heap_sema);
  return new_mem;
}

void *reallocarray(void *ptr, size_t nmemb, size_t size)
{
  return NULL;
}

unsigned long atol(const char *nptr)
{
  unsigned long res = 0;

  for (int i = 0; nptr[i] != '\0'; ++i)
      res = res * 10 + nptr[i] - '0';

  return res;
}

int atoi(const char *nptr)
{
  int res = 0;

  for (int i = 0; nptr[i] != '\0'; ++i)
      res = res * 10 + nptr[i] - '0';

  return res;
}
