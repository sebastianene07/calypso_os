#include <stdint.h>
#include <s_heap.h>
#include <core_cm4.h>

#define RAM_BASE  (void *)(0x20000000)
#define STACK_TOP  (void *)(RAM_BASE + 0x5000)

#define HEAP_START (void *)(RAM_BASE + 0x5004)
#define HEAP_END   (void *)(RAM_BASE + 0x19000)
#define HEAP_BLOCK_SIZE (16)

#define  XTAL            (50000000UL)     /* Oscillator frequency */

#define  SYSTEM_CLOCK    (XTAL / 2U)

/*----------------------------------------------------------------------------
  System Core Clock Variable
 *----------------------------------------------------------------------------*/
uint32_t SystemCoreClock = SYSTEM_CLOCK;  /* System Core Clock Frequency */


extern unsigned long _stext;
extern unsigned long _sbss;
extern unsigned long _sdata;
extern unsigned long _etext;
extern unsigned long _ebss;
extern unsigned long _edata;
extern unsigned long _srodata;

heap_t g_my_heap;

void c_startup(void);

void dummy_fn(void);

int main();

void SysTick_Handler(void);

void miau(void)
{
  while(1) {;;}
}

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
        miau,

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


  /* Initialize the HEAP memory */

  s_init(&g_my_heap,
         HEAP_START,
         HEAP_END,
         HEAP_BLOCK_SIZE);

  /* Configure Sys Tick */

  SysTick_Config(SystemCoreClock / 1000);

  /* Start the main program */

  main();
}
