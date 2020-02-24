#include <stdint.h>
#include <board.h>
#include <semaphore.h>
#include <serial.h>

#include <uart_reg.h>

/****************************************************************************
 * Private Data
 ****************************************************************************/

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
  uint32_t fifo_addr = UART_FIFO_AHB_REG(0);
  WRITE_PERI_REG(fifo_addr, c);

  return 0;
}

sem_t *get_console_sema(void)
{
  return &g_console_sema;
}
