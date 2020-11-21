#include <board.h>

#include <irq_manager.h>

/****************************************************************************
 * Private variables defintion
 ****************************************************************************/

/* Ram based ISR vector - NUM_IRQS should be defined by the board */

static void (*g_ram_vectors[NUM_IRQS])(void);

/****************************************************************************
 * Public Methods
 ****************************************************************************/

/**************************************************************************
 * Name:
 *  irq_attach
 *
 * Description:
 *  Attach an interrupt callback.
 *
 * Assumption/Limitations:
 *  Call this function with interrupts disabled.
 *
 *************************************************************************/

void irq_attach(int irq_num, irq_cb handler)
{
  g_ram_vectors[irq_num] = handler;
}

/**************************************************************************
 * Name:
 *  irq_detach
 *
 * Description:
 *  Detach an interrupt callback.
 *
 * Assumption/Limitations:
 *  Call this function with interrupts disabled.
 *
 *************************************************************************/

void irq_detach(int irq_num)
{
  g_ram_vectors[irq_num] = NULL;
}

/**************************************************************************
 * Name:
 *  irq_generic_handler 
 *
 * Description:
 *  This is the generic interrupt handler for this OS..
 *
 *************************************************************************/

void irq_generic_handler(void)
{
  uint8_t isr_num = cpu_getirqnum();

  if (isr_num >= NUM_IRQS || g_ram_vectors[isr_num] == NULL)
  {
    return;
  }

  g_ram_vectors[isr_num]();
}
