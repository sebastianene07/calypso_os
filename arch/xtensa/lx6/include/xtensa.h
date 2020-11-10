#ifndef __XTENSA_H
#define __XTENSA_H

typedef enum {
  DEFAULT = 0,
  NUM_IRQS = 90
} IRQn_Type;

typedef uint64_t irq_state_t;

/**
  \brief   Enable IRQ Interrupts
  \details Enables IRQ interrupts by clearing the I-bit in the CPSR.
           Can only be executed in Privileged modes.
 */
static inline void enable_int(irq_state_t irq_state)
{
}


/**
  \brief   Disable IRQ Interrupts
  \details Disables IRQ interrupts by setting the I-bit in the CPSR.
           Can only be executed in Privileged modes.
 */
static inline irq_state_t disable_int(void)
{
  return 0;
}

static inline void NVIC_TriggerSysTick(void)
{
}

static inline void sched_context_switch(void)
{
}

#endif /* __XTENSA_H */
