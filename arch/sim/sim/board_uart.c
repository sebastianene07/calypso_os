#include <board.h>
#include <stdint.h>
#include <board.h>
#include <semaphore.h>
#include <serial.h>

/****************************************************************************
 * Private Data
 ****************************************************************************/

static struct uart_lower_s g_uart_low_0;
static sem_t g_console_sema;

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

void host_console_putc(int c);

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int uart_low_init(void)
{
  return sem_init(&g_console_sema, 0, 1);
}

int putchar(int c)
{
  host_console_putc(c);
  return 0;
}

sem_t *get_console_sema(void)
{
  return &g_console_sema;
}
