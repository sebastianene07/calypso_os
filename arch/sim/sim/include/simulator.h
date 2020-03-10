#ifndef __SIMULATOR_H
#define __SIMULATOR_H

/****************************************************************************
 * Public Types
 ****************************************************************************/

/* We need this type to prevent compilation errors but we don't have interrupts
 * on this build.
 */

typedef enum {
  DEFAULT = 0,
  NUM_IRQS = 1
} IRQn_Type;

/****************************************************************************
 * Public Functions Definitions
 ****************************************************************************/

/* These functions enable/disable the SysTick simulated interrupts */

void __enable_irq(void);

void __disable_irq(void);

/* This function handles the context switching mechanism and it's symbol is
 * exported in the partially linked executable.
 */

void sched_context_switch(void);

#endif /* __SIMULATOR_H */
