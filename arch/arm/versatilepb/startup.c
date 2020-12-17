#include <board.h>

#include <irq_manager.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <scheduler.h>
#include <os_start.h>

/* RAM based ISR vector */

void copy_isr_vector(void *pc_dispatch_address)
{
  uint32_t irq_dispatch_instruction;
  uint32_t *irq_copy = NULL;

  memcpy(&irq_dispatch_instruction, pc_dispatch_address, sizeof(irq_dispatch_instruction));
  for (int i = 0; i < NUM_IRQS; i++)
  {
    memcpy(irq_copy, &irq_dispatch_instruction, sizeof(uint32_t));
    irq_copy++;
  }
}

void __assert_func(bool assert_cond)
{
  __asm volatile("bkpt 1");
}
