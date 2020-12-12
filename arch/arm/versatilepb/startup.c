#include <board.h>

#include <irq_manager.h>
#include <stdbool.h>
#include <stdint.h>
#include <scheduler.h>
#include <os_start.h>

/* Flash based ISR vector */
__attribute__((section(".isr_vector")))
void (*g_vectors[NUM_IRQS])(void) = {
        STACK_TOP,
#ifdef CONFIG_CONSOLE_NRF_INIT_SOFTDEVICE_APP
        Reset_Handler,
#else
        __start,
#endif
        irq_generic_handler,  /* NMI handler */
        irq_generic_handler,  /* Hard fault handler */
        irq_generic_handler,
        irq_generic_handler,
        irq_generic_handler,
        irq_generic_handler,
        irq_generic_handler,
        irq_generic_handler,
        irq_generic_handler,
        irq_generic_handler,  /* SVC */
        irq_generic_handler,
        irq_generic_handler,
        irq_generic_handler,  /* Pend SV */
        irq_generic_handler,  /* Systick handler */

        /* External interrupts */

        irq_generic_handler,
        irq_generic_handler,
        irq_generic_handler,
        irq_generic_handler,
        irq_generic_handler,
        irq_generic_handler,
        irq_generic_handler,
        irq_generic_handler,
        irq_generic_handler,
        irq_generic_handler,
        irq_generic_handler,
        irq_generic_handler,
        irq_generic_handler,
        irq_generic_handler,
        irq_generic_handler,
        irq_generic_handler,
        irq_generic_handler,
        irq_generic_handler,
        irq_generic_handler,
        irq_generic_handler,
        irq_generic_handler,
        irq_generic_handler,
        irq_generic_handler,
        irq_generic_handler,
        irq_generic_handler,
        irq_generic_handler,
        irq_generic_handler,
        irq_generic_handler,
        irq_generic_handler,
        irq_generic_handler,
        irq_generic_handler,
        irq_generic_handler,
        irq_generic_handler,
        irq_generic_handler,
        irq_generic_handler,
        irq_generic_handler,
        irq_generic_handler,
        irq_generic_handler,
        irq_generic_handler,
        irq_generic_handler,
        irq_generic_handler,
        irq_generic_handler,
        irq_generic_handler,
        irq_generic_handler,
        irq_generic_handler,
        irq_generic_handler,
        irq_generic_handler,
        irq_generic_handler,
};

void __assert_func(bool assert_cond)
{
  __asm volatile("bkpt 1");
}
