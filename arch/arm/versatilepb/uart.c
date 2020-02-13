#include <stdint.h>
#include <board.h>
#include <semaphore.h>
#include <serial.h>

/****************************************************************************
 * Private Data
 ****************************************************************************/

static volatile unsigned int * const UART0DR = (unsigned int *)0x101f1000;

static struct uart_lower_s g_uart_low_0;
static sem_t g_console_sema;

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int uart_low_init(void)
{
  return sem_init(&g_console_sema, 0, 1);
}

int putchar(int c)
{
  *UART0DR = c;
  return 0;
}

sem_t *get_console_sema(void)
{
  return &g_console_sema;
}
