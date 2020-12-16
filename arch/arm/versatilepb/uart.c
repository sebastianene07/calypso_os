#include <stdint.h>
#include <board.h>
#include <semaphore.h>
#include <serial.h>
#include <string.h>

/* Receive timeout interrupt mask. A read returns the current mask for the UARTRTINTR interrupt.
 * On a write of 1, the mask of the UARTRTINTR interrupt is set. A write of 0 clears the mask.
 */
#define RTIM            (6)

/* Transmit interrupt mask. A read returns the current mask for the UARTTXINTR interrupt.
 * On a write of 1, the mask of the UARTTXINTR interrupt is set. A write of 0 clears the mask.
 */
#define TXIM            (5)

/* Receive interrupt mask. A read returns the current mask for the UARTRXINTR interrupt.
 * On a write of 1, the mask of the UARTRXINTR interrupt is set. A write of 0 clears the mask.
 */
#define RXIM            (4)


/****************************************************************************
 * Private Types
 ****************************************************************************/

typedef volatile struct versatilepb_uart_priv_s
{
  bool is_initialized;
  void *base_peripheral_ptr;
} versatilepb_uart_priv_t;

typedef volatile struct {
    uint32_t DR;
    uint32_t RSR_ECR;
    uint8_t reserved1[0x10];
    const uint32_t FR;
    uint8_t reserved2[0x4];
    uint32_t LPR;
    uint32_t IBRD;
    uint32_t FBRD;
    uint32_t LCR_H;
    uint32_t CR;
    uint32_t IFLS;
    uint32_t IMSC;
    const uint32_t RIS;
    const uint32_t MIS;
    uint32_t ICR;
    uint32_t DMACR;
} __attribute__((packed)) pl011_T;

/****************************************************************************
 * Private Function Prototypesa
 ****************************************************************************/

static int versatilepb_lpuart_open(const struct uart_lower_s *lower);
static int versatilepb_lpuart_write(const struct uart_lower_s *lower,
                                    const void *ptr_data,
                                    unsigned int sz);
static int versatilepb_lpuart_read(const struct uart_lower_s *lower_half,
                                   void *buf,
                                   unsigned int count);

/****************************************************************************
 * Private Data
 ****************************************************************************/

static volatile pl011_T *uart0 = (pl011_T *)UART0;
static sem_t g_console_sema;

static versatilepb_uart_priv_t g_uart_low_0_priv =
{
  .base_peripheral_ptr = UART0,
};

/* Uart 0 lower half operations. There is no need to provide a read_cb function
 * because we notify the incmming data through rx_notify semaphore and we
 * copy it in the rx_buffer from interrupt.
 */

static struct uart_lower_s g_uart_lowerhalfs[] =
{
  {
    .priv     = &g_uart_low_0_priv,
    .open_cb  = versatilepb_lpuart_open,
    .write_cb = versatilepb_lpuart_write,
    .read_cb  = versatilepb_lpuart_read,
    .dev_path = CONFIG_CONSOLE_UART_PATH,
  },
};

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static int versatilepb_lpuart_config(struct uart_lower_s *lower)
{
  struct versatilepb_uart_priv_s *uart_priv = lower->priv;
  pl011_T *uart_mmio = (pl011_T *)uart_priv->base_peripheral_ptr;

  uart_mmio->IMSC = (1 << RXIM);
  *PIC_IntEnable |= (1 << 12);
  return 0; 
}

static int versatilepb_lpuart_open(const struct uart_lower_s *lower)
{
  struct versatilepb_uart_priv_s *uart_priv = lower->priv;

  sem_wait((sem_t *)&lower->lock);

  if (!uart_priv->is_initialized) {
    versatilepb_lpuart_config((struct uart_lower_s *)lower);
  }

  sem_post((sem_t *)&lower->lock);
  return 0;
}

static int versatilepb_lpuart_write(const struct uart_lower_s *lower,
                                    const void *ptr_data,
                                    unsigned int sz)
{
  struct versatilepb_uart_priv_s *uart_priv = lower->priv;

  sem_wait((sem_t *)&lower->lock);

  for (int i = 0; i < sz; i++) {
    uart0->DR = *((unsigned char *)(ptr_data + i));
  }

  sem_post((sem_t *)&lower->lock);
  return sz;
}

static int versatilepb_lpuart_read(const struct uart_lower_s *lower_half,
                                   void *buf,
                                   unsigned int count)
{
  int ret = 0;
  uint32_t min_copy = 0;
  int total_copy = 0;

  struct uart_lower_s *lower = (struct uart_lower_s *)lower_half;
  struct versatilepb_uart_priv_s *uart_priv = lower->priv;

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
  return sem_init(&g_console_sema, 0, 1);
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
  uart0->DR = c;
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
 *   This function initializes the versatilepb UART peripheral and creates
 *   entries in /dev/uartX.
 *
 * Input Parameters:
 *   uart_num - (out) buffer where we save the number of peripherals registered
 *
 * Returned Value:
 *   On success returns the registered lowerhalfs otherwise it returns NULL.
 *
 ****************************************************************************/

struct uart_lower_s *uart_init(size_t *uart_num)
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
