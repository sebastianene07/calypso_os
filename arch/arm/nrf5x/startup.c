#include <board.h>

#include <stdint.h>
#include <scheduler.h>
#include <os_start.h>

/* Ram based ISR vector */
void (*g_ram_vectors[NUM_IRQS])(void);

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

void dummy_fn(void);

void Pend_SV_Handler(void);
void SysTick_Handler(void);
static void generic_isr_handler(void);

/* The fault handler implementation calls a function called
prvGetRegistersFromStack(). */
static void HardFault_Handler(void)
{
}

/* Flash based ISR vector */
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

        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
};

void dummy_fn(void)
{
        while(1)
        {

        }
}

static void generic_isr_handler(void)
{
  /* get the exception number */
  uint8_t isr_num = (SCB->ICSR & 0xF);

  if (isr_num >= NUM_IRQS || g_ram_vectors[isr_num] == NULL)
    return;

  g_ram_vectors[isr_num]();
}