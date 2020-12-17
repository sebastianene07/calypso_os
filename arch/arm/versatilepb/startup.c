#include <board.h>

#include <irq_manager.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <scheduler.h>
#include <os_start.h>

/* RAM based ISR vector */

void copy_isr_vector(void)
{
  extern uint32_t vectors_start;
  extern uint32_t vectors_end;

  uint32_t *vectors_src = &vectors_start;
  uint32_t *vectors_dst = (uint32_t *)0;
 
  while(vectors_src < &vectors_end)
    *vectors_dst++ = *vectors_src++;
}

void __assert_func(bool assert_cond)
{
  __asm volatile("bkpt 1");
}
