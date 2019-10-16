#include <stdint.h>
#include <board.h>
#include <semaphore.h>
#include <serial.h>

/****************************************************************************
 * Private Data
 ****************************************************************************/

static volatile unsigned int * const UART0DR = (unsigned int *)0x101f1000;

static struct uart_lower_s g_uart_low_0;

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int uart_low_init(void)
{
  return 0;
}

int uart_low_send(char *msg)
{
  while (*msg != '\0') {
    *UART0DR = (unsigned int)*msg;
    msg++;
  }
  return 0;
}

int putchar(int c)
{
  *UART0DR = c;
}

int uart_init(void)
{
  return uart_register("/dev/ttyUSB0", &g_uart_low_0);
}
