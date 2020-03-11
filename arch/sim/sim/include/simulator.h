#ifndef __SIMULATOR_H
#define __SIMULATOR_H

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define CONFIG_SIM_LPUART_FIFO_SIZE   (1)

/****************************************************************************
 * Public Types
 ****************************************************************************/

/* The simulated interrupts values */

typedef enum {
  UART_0_IRQ   = 0,
  NUM_IRQS
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
