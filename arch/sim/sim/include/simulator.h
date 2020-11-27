#ifndef __SIMULATOR_H
#define __SIMULATOR_H

#include <stdint.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define CONFIG_SIM_LPUART_FIFO_SIZE   (32)

/****************************************************************************
 * Public Types
 ****************************************************************************/

/* The simulated interrupts values */

typedef enum {
  UART_0_IRQ   = 0,
  SYSTICK_IRQ  = 1,
  NUM_IRQS
} IRQn_Type;

/* The simulated UART peripheral */

typedef struct {
  uint8_t sim_uart_data_fifo[CONFIG_SIM_LPUART_FIFO_SIZE];
  uint8_t uart_reg_read_index;  /* The read index is incremented when we read data from the FIFO */
  uint8_t uart_reg_write_index; /* The write index is incremented when we put data in the FIFO */
  uint8_t is_peripheral_ready;
} sim_uart_peripheral_t;

/* The irq state type */

typedef uint32_t irq_state_t;

/****************************************************************************
 * Public Functions Definitions
 ****************************************************************************/

#endif /* __SIMULATOR_H */
