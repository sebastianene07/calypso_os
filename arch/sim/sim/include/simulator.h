#ifndef __SIMULATOR_H
#define __SIMULATOR_H

/****************************************************************************
 * Public Types
 ****************************************************************************/

typedef enum {
  DEFAULT = 0,
  NUM_IRQS = 90
} IRQn_Type;

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/**
  \brief   Enable IRQ Interrupts
  \details Enables IRQ interrupts by clearing the I-bit in the CPSR.
           Can only be executed in Privileged modes.
 */
static inline void __enable_irq(void)
{
}


/**
  \brief   Disable IRQ Interrupts
  \details Disables IRQ interrupts by setting the I-bit in the CPSR.
           Can only be executed in Privileged modes.
 */
static inline void __disable_irq(void)
{
}

static inline void NVIC_TriggerSysTick(void)
{
	/* TODO */
}

static inline void sched_context_switch(void)
{
  /* TODO */
}

#endif /* __SIMULATOR_H */
