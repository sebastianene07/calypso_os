#include <board.h>
#include <console_main.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>
#include <../../s_alloc/s_heap.h>

extern heap_t g_my_heap;

static size_t get_used_heap_size(void)
{
  mem_node_t *node = NULL;
  size_t current_size = 0;

  /* TODO: Take the global heap sema */

  list_for_each_entry (node, &g_my_heap.g_used_heap_list, node_list)
  {
    current_size += node->mask.size * g_my_heap.block_size;
  }

  /* TODO: Release the global heap sema */

  return current_size;
}

static size_t get_free_heap_size(size_t *biggest_chunk)
{
  mem_node_t *node = NULL;
  size_t current_size = 0;

  /* TODO: Take the global heap sema */

  list_for_each_entry (node, &g_my_heap.g_free_heap_list, node_list)
  {
    size_t chunk_size = node->mask.size * g_my_heap.block_size;
    if (chunk_size > *biggest_chunk)
    {
      *biggest_chunk = chunk_size;
    }

    current_size += chunk_size;
  }

  /* TODO: Release the global heap sema */



  return current_size;
}

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
