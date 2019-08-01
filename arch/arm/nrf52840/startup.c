#include <board.h>

#include <stdint.h>
#include <scheduler.h>
#include <os_start.h>

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

void dummy_fn(void);

void Pend_SV_Handler(void);
void SysTick_Handler(void);

/* The fault handler implementation calls a function called
prvGetRegistersFromStack(). */
static void HardFault_Handler(void)
{
}

__attribute__((section(".isr_vector")))
void (*g_vectors[NUM_IRQS])(void) = {
        STACK_TOP,
        os_startup,
        dummy_fn,
        HardFault_Handler,
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
        Pend_SV_Handler,
        SysTick_Handler,

        /* External interrupts */
};

void dummy_fn(void)
{
        while(1)
        {

        }
}
