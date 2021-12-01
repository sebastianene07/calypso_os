#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>

#include "pico/stdlib.h"
#include "hardware/resets.h"
#include "hardware/regs/resets.h"
#include "hardware/uart.h"

#include "bsp.h"
#include "hardware/irq.h"

/****************************************************************************
 * Preprocessor Definitions
 ****************************************************************************/

#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY    UART_PARITY_NONE

#define CONSOLE_UART_ID             uart0
#define CONSOLE_BAUD_RATE           115200

#define CONSOLE_UART_TX_PIN         12
#define CONSOLE_UART_RX_PIN         13

/****************************************************************************
 * External symbols
 ****************************************************************************/

void runtime_init(void);

/****************************************************************************
 * Empty symbols required to build libbsp.a
 ****************************************************************************/

int vprintf(const char * restrict format, va_list ap)
{
  return 0;
}

int puts(const char *s)
{
  return 0;
}

int main(void)
{
  return 0;
}

/****************************************************************************
 * UART bsp functions
 ****************************************************************************/

void bsp_serial_console_init(void)
{
  runtime_init();

  // Set up our UART with the required speed.
  uart_init(CONSOLE_UART_ID, CONSOLE_BAUD_RATE);

  // Set the TX and RX pins by using the function select on the GPIO
  // Set datasheet for more information on function select
  gpio_set_function(CONSOLE_UART_TX_PIN, GPIO_FUNC_UART);
  gpio_set_function(CONSOLE_UART_RX_PIN, GPIO_FUNC_UART);
}

static volatile void (*g_uart0_notify_received)(uint8_t);

void __attribute__((optimize("O0"))) rpipico_uart0_irq(void)
{
  while (uart_is_readable(CONSOLE_UART_ID)) {
    uint8_t ch = uart_getc(CONSOLE_UART_ID);
    if (g_uart0_notify_received != NULL) {
      g_uart0_notify_received(ch);
    }
  }
}

int bsp_serial_console_attach_irq(void *irq_cb(uint8_t))
{
  int UART_IRQ = CONSOLE_UART_ID == uart0 ? UART0_IRQ : UART1_IRQ;

  uart_set_baudrate(CONSOLE_UART_ID, 115200);
  uart_set_hw_flow(CONSOLE_UART_ID, false, false);
  uart_set_format(CONSOLE_UART_ID, DATA_BITS, STOP_BITS, PARITY);
  uart_set_fifo_enabled(CONSOLE_UART_ID, false);

  g_uart0_notify_received = irq_cb;
  irq_set_enabled(UART_IRQ, true);
  uart_set_irq_enables(CONSOLE_UART_ID, true, false);

  return UART_IRQ;
}

void bsp_serial_console_putc(int c)
{
  uart_putc(CONSOLE_UART_ID, c);
}

/****************************************************************************
 * GPIO bsp functions
 ****************************************************************************/

void bsp_gpio_led_init()
{
  const uint LED_PIN = PICO_DEFAULT_LED_PIN;
  gpio_init(LED_PIN);
  gpio_set_dir(LED_PIN, GPIO_OUT);
  gpio_put(LED_PIN, 1);
}
