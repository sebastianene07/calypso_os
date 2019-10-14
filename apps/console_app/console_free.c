#include <board.h>
#include <console_main.h>

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>
#include <semaphore.h>
#include <../../s_alloc/s_heap.h>

/****************************************************************************
 * Public Data
 ****************************************************************************/

extern heap_t g_my_heap;
extern sem_t g_heap_sema;

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static size_t get_used_heap_size(void)
{
  mem_node_t *node = NULL;
  size_t current_size = 0;

  sem_wait(&g_heap_sema);

  list_for_each_entry (node, &g_my_heap.g_used_heap_list, node_list)
  {
    current_size += node->mask.size * g_my_heap.block_size;
  }

  sem_post(&g_heap_sema);
  return current_size;
}

static size_t get_free_heap_size(size_t *biggest_chunk)
{
  mem_node_t *node = NULL;
  size_t current_size = 0;

  sem_wait(&g_heap_sema);

  list_for_each_entry (node, &g_my_heap.g_free_heap_list, node_list)
  {
    size_t chunk_size = node->mask.size * g_my_heap.block_size;
    if (chunk_size > *biggest_chunk)
    {
      *biggest_chunk = chunk_size;
    }

    current_size += chunk_size;
  }

  sem_post(&g_heap_sema);
  return current_size;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int console_free(int argc, const char *argv[])
{
  size_t biggest_chunk = 0;

  printf("Free size | Used size | Largest Free Chunk | Total\n\n");
  printf("%d bytes | %d bytes | %d bytes | %d bytes\n\n",
         get_free_heap_size(&biggest_chunk),
         get_used_heap_size(),
         biggest_chunk,
         g_my_heap.block_size * g_my_heap.num_blocks);

  return 0;
}
