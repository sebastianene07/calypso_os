/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <board.h>
#include <stdint.h>
#include <board.h>
#include <semaphore.h>
#include <serial.h>

/****************************************************************************
 * Private Data
 ****************************************************************************/

/* The simulated UART lower half */

static struct uart_lower_s g_uart_low_0;

/* The console semaphore */

static sem_t g_console_sema;

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

/* This function is implemented in the file which is linked with the host
 * definitions so that it can print charcters on the stdout.
 */

void host_console_putc(int c);

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: uart_low_init
 *
 * Description:
 *   The lower level UART initialization should allow printing characters on
 *   the console. We only initialize the console semaphore here as we don't
 *   have a hardware peripheral to output charcters and we use the process
 *   stdout.
 *
 ****************************************************************************/

int uart_low_init(void)
{
  return sem_init(&g_console_sema, 0, 1);
}

/****************************************************************************
 * Name: putchar
 *
 * Description:
 *   This function is used by the 'printf' to show an individual charcter to
 *   the console. It invokes the host_console_putc that prints the character
 *   to the stdout.
 *
 * Return Value:
 *   It always returns 0.
 *
 ****************************************************************************/

int putchar(int c)
{
  host_console_putc(c);
  return 0;
}

/****************************************************************************
 * Name: get_console_sema
 *
 * Description:
 *   This function returns the address of the console semaphore.
 *
 ****************************************************************************/

sem_t *get_console_sema(void)
{
  return &g_console_sema;
}
