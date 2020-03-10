#include <board.h>

#define HEAP_SIZE           (1024 * 1024)

static uint8_t g_heap_memory[HEAP_SIZE];

unsigned long _sbss = &g_heap_memory[0];
unsigned long _ebss = &g_heap_memory[HEAP_SIZE];

unsigned long _sheap = &g_heap_memory[0];
unsigned long _eheap = &g_heap_memory[HEAP_SIZE];

void (*g_ram_vectors[NUM_IRQS])(void);
