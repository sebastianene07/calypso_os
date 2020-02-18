#ifndef __XTENSA_H
#define __XTENSA_H

typedef enum {
  DEFAULT = 0,
  NUM_IRQS = 90
} IRQn_Type;

static inline void __disable_irq(void)
{
}

static inline void __enable_irq(void)
{
}

static inline void NVIC_TriggerSysTick(void)
{
}

static inline void sched_context_switch(void)
{
}

#endif /* __XTENSA_H */
