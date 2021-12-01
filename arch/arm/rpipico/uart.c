#include <board.h>

#include <stdint.h>
#include <irq_manager.h>
#include <semaphore.h>
#include <serial.h>
#include <string.h>

#include "bsp.h"

int bsp_serial_console_attach_irq(void *irq_cb(uint8_t ch));
void rpipico_uart0_irq(void);

/****************************************************************************
 * Private Types
 ****************************************************************************/

typedef volatile struct rpipico_uart_priv_s
{
  bool is_initialized;
  void *base_peripheral_ptr;
} rpipico_uart_priv_t;

/****************************************************************************
 * Private Function Prototypesa
 ****************************************************************************/

static int rpipico_lpuart_open(const struct uart_lower_s *lower);
static int rpipico_lpuart_write(const struct uart_lower_s *lower,
                                const void *ptr_data,
                                unsigned int sz);
static int rpipico_lpuart_read(const struct uart_lower_s *lower_half,
                               void *buf,
                               unsigned int count);

/****************************************************************************
 * Private Data
 ****************************************************************************/

static sem_t g_console_sema;

static rpipico_uart_priv_t g_uart_low_0_priv;

/* Uart 0 lower half operations. There is no need to provide a read_cb function
 * because we notify the incmming data through rx_notify semaphore and we
 * copy it in the rx_buffer from interrupt.
 */

static struct uart_lower_s g_uart_lowerhalfs[] =
{
  {
    .priv     = &g_uart_low_0_priv,
    .open_cb  = rpipico_lpuart_open,
    .write_cb = rpipico_lpuart_write,
    .read_cb  = rpipico_lpuart_read,
    .dev_path = CONFIG_CONSOLE_UART_PATH,
  },
};

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static volatile void rpipico_on_uart_rx(uint8_t ch)
{
  struct uart_lower_s *lower = &g_uart_lowerhalfs[0];

  lower->rx_buffer[lower->index_write_rx_buffer] = ch;
  lower->index_write_rx_buffer = (lower->index_write_rx_buffer + 1) % UART_RX_BUFFER;

  /* Notify incomming RX characters */
  sem_post(&lower->rx_notify);
}

static int rpipico_lpuart_config(struct uart_lower_s *lower)
{
  irq_state_t irq_state;
  int irq;

  /* Attach the interrupt handler */
  /* Note: We need to route interrupts from Calypso OS to the pico-sdk */

  irq_state = cpu_disableint();
  irq = bsp_serial_console_attach_irq(rpipico_on_uart_rx);
  irq_attach(irq, rpipico_uart0_irq);
  cpu_enableint(irq_state);

  return 0;
}

static int rpipico_lpuart_open(const struct uart_lower_s *lower)
{
  struct rpipico_uart_priv_s *uart_priv = lower->priv;

  sem_wait((sem_t *)&lower->lock);

  if (!uart_priv->is_initialized) {
    rpipico_lpuart_config((struct uart_lower_s *)lower);
  }

  sem_post((sem_t *)&lower->lock);
  return 0;
}

static int rpipico_lpuart_write(const struct uart_lower_s *lower,
                                const void *ptr_data,
                                unsigned int sz)
{
  sem_wait((sem_t *)&lower->lock);

  for (int i = 0; i < sz; i++) {
    bsp_serial_console_putc(*((uint8_t *)(ptr_data + i)));
  }

  sem_post((sem_t *)&lower->lock);
  return sz;
}

static int rpipico_lpuart_read(const struct uart_lower_s *lower_half,
                               void *buf,
                               unsigned int count)
{
  int total_copy = 0;

  struct uart_lower_s *lower = (struct uart_lower_s *)lower_half;

  do {
    uint8_t available_rx_bytes = 0;

    sem_wait((sem_t *)&lower->lock);

    if (lower->index_read_rx_buffer < lower->index_write_rx_buffer)
    {
      available_rx_bytes = lower->index_write_rx_buffer -
        lower->index_read_rx_buffer;
    }
    else if (lower->index_read_rx_buffer != lower->index_write_rx_buffer)
    {
      available_rx_bytes = UART_RX_BUFFER -
      (lower->index_read_rx_buffer - lower->index_write_rx_buffer);
    }

    uint8_t min_copy = count > available_rx_bytes ? available_rx_bytes : count;
    if (min_copy > 0)
    {
      memcpy(buf + total_copy, lower->rx_buffer + lower->index_read_rx_buffer,
             min_copy);

      total_copy += min_copy;
      count      -= min_copy;

      lower->index_read_rx_buffer += min_copy;
      lower->index_read_rx_buffer = lower->index_read_rx_buffer % UART_RX_BUFFER;

      sem_post((sem_t *)&lower->lock);
    }
    else
    {
      sem_post((sem_t *)&lower->lock);
      sem_wait(&lower->rx_notify);
    }
  } while (count > 0);

  return total_copy;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: uart_low_init
 *
 * Description:
 *   The lower level UART initialization should allow printing characters on
 *   the console.
 *
 ****************************************************************************/

int uart_low_init(void)
{
  int ret;

  ret = sem_init(&g_console_sema, 0, 1);
  if (ret != 0)
    return ret;

  bsp_serial_console_init();
  return ret;
}

/****************************************************************************
 * Name: putchar
 *
 * Description:
 *   This function is used by the 'printf' to show an individual charcter to
 *   the console.
 *
 * Return Value:
 *   It always returns 0.
 *
 ****************************************************************************/

int putchar(int c)
{
  bsp_serial_console_putc(c);
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

/****************************************************************************
 * Name: uart_init
 *
 * Description:
 *   This function initializes the rpipico UART peripheral and creates
 *   entries in /dev/uartX.
 *
 * Input Parameters:
 *   uart_num - (out) buffer where we save the number of peripherals registered
 *
 * Returned Value:
 *   On success returns the registered lowerhalfs otherwise it returns NULL.
 *
 ****************************************************************************/

struct uart_lower_s *uart_upper_init(size_t *uart_num)
{
  int ret = 0;

  if (uart_num == NULL) {
    return NULL;
  }

  *uart_num = ARRAY_LEN(g_uart_lowerhalfs);

  for (int i = 0; i < *uart_num; i++) {

    sem_init(&g_uart_lowerhalfs[i].tx_notify, 0, 0);
    sem_init(&g_uart_lowerhalfs[i].rx_notify, 0, 0);
    sem_init(&g_uart_lowerhalfs[i].lock, 0, 1);

    ret = uart_register(g_uart_lowerhalfs[i].dev_path, &g_uart_lowerhalfs[i]);
    if (ret < 0) {
      return NULL;
    }
  }

  return g_uart_lowerhalfs;
}
