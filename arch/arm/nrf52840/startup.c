#include <board.h>

#include <stdint.h>
#include <s_heap.h>
#include <scheduler.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define RAM_BASE   (void *)(0x20000000)
#define STACK_TOP  (void *)(RAM_BASE + 0x5000)

#define HEAP_START (void *)(RAM_BASE + 0x5004)
#define HEAP_END   (void *)(RAM_BASE + 0x19000)
#define HEAP_BLOCK_SIZE (16)

#define XTAL            (50000000UL)     /* Oscillator frequency */
#define SYSTEM_CLOCK    (XTAL / 2U)

/****************************************************************************
 * Public Data
 ****************************************************************************/

uint32_t SystemCoreClock = SYSTEM_CLOCK;  /* System Core Clock Frequency */

extern unsigned long _stext;
extern unsigned long _sbss;
extern unsigned long _sdata;
extern unsigned long _etext;
extern unsigned long _ebss;
extern unsigned long _edata;
extern unsigned long _srodata;

/* Heap definition */

heap_t g_my_heap;

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

void c_startup(void);

void dummy_fn(void);

void os_startup(void);

void SysTick_Handler(void);

volatile int is_enabled = 0;

__attribute__((section(".isr_vector")))
void (*vectors[])(void) = {
        STACK_TOP,
        c_startup,
        dummy_fn,
        dummy_fn,
        dummy_fn,
        dummy_fn,
        dummy_fn,
        dummy_fn,
        dummy_fn,
        dummy_fn,
        dummy_fn,
        dummy_fn,
        dummy_fn,
        dummy_fn,
        dummy_fn,
        SysTick_Handler,

        /* External interrupts */
};

void dummy_fn(void)
{
        while(1)
        {

        }
}

void c_startup(void)
{
  unsigned long *src, *dst;

  src = &_etext;
  dst = &_sdata;
  while(dst < &_edata)
      *(dst++) = *(src++);

  src = &_sbss;
  while(src < &_ebss)
      *(src++) = 0;

  /* TODO: If the stack goes below _ebss we must ASSERT */

  /* Initialize the HEAP memory */

  s_init(&g_my_heap,
         HEAP_START,
         HEAP_END,
         HEAP_BLOCK_SIZE);

  sched_init();
  sched_create_task(os_startup, 1024);

  /* Configure Sys Tick */

  SysTick_Config(SystemCoreClock / 100);

  /* Schedule tasks */

  while (1)
  {
    ;;
  }
}
