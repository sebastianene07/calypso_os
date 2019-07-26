#include <board.h>
#include <semaphore.h>
#include <serial.h>

int uart_register(const char *name, const struct uart_lower_s *uart_lowerhalf)
{
  /* Called from lowerhalf driver. Allocate a new upper driver instance where
   * we store the lowerhalf. struct uart_upper_s
   */

  /* Register the upper half node with the VFS */
}
