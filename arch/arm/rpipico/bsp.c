#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>

#include "pico/stdlib.h"
#include "hardware/resets.h"
#include "hardware/regs/resets.h"
#include "hardware/uart.h"
#include "bsp.h"

/****************************************************************************
 * Preprocessor Definitions
 ****************************************************************************/

#define CONSOLE_UART_ID             uart0
#define CONSOLE_BAUD_RATE           115200

#define CONSOLE_UART_TX_PIN         0
#define CONSOLE_UART_RX_PIN         1

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
