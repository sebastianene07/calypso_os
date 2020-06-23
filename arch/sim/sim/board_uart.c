/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <board.h>
#include <stdint.h>
#include <board.h>
#include <semaphore.h>
#include <serial.h>
#include <errno.h>

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

static int sim_lpuart_open(const struct uart_lower_s *lower);
static int sim_lpuart_write(const struct uart_lower_s *lower,
                            const void *ptr_data,
                            unsigned int sz);

static int sim_lpuart_read(const struct uart_lower_s *lower, void *data,
                           unsigned int max_buf_sz);

static int sim_lpuart_config(struct uart_lower_s *lower);

static void sim_lpuart_int(void);

/****************************************************************************
 * Public Data
 ****************************************************************************/

sim_uart_peripheral_t g_uart_peripheral;

/****************************************************************************
 * Private Data
 ****************************************************************************/

/* The console semaphore */

static sem_t g_console_sema;

/* Uart 0 lower half operations. There is no need to provide a read_cb function
 * because we notify the incmming data through rx_notify semaphore and we
 * copy it in the rx_buffer from interrupt.
 */

static struct uart_lower_s g_uart_lowerhalfs[] =
{
  {
    .open_cb  = sim_lpuart_open,
    .write_cb = sim_lpuart_write,
    .read_cb  = sim_lpuart_read,
    .dev_path = CONFIG_CONSOLE_UART_PATH,
  },

#ifdef CONFIG_UART_PERIPHERAL_1
  {
    .open_cb  = sim_lpuart_open,
    .write_cb = sim_lpuart_write,
    .read_cb  = sim_lpuart_read,
    .dev_path = CONFIG_UART_PERIPHERAL_1_PATH,
  }
#endif
};

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

/* This function is implemented in the file which is linked with the host
 * definitions so that it can print charcters on the stdout.
 */

void host_console_putc(int c);

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: sim_lpuart_open
 *
 * Description:
 *   This function opens the simulated UART and attaches the interrupt to
 *   receive events from the simulation.
 *
 * Input Parameters:
 *   lower - the lower half UART instance
 *
 * Return Value:
 *   OK in case of success otherwise a negative error code.
 *
 ****************************************************************************/

static int sim_lpuart_open(const struct uart_lower_s *lower)
{
  attach_int(UART_0_IRQ, sim_lpuart_int);
  g_uart_peripheral.is_peripheral_ready = 1;
  return OK;
}

/****************************************************************************
 * Name: sim_lpuart_write
 *
 * Description:
 *   This function writes data on the simulated UART.
 * 
 * Input Parameters:
 *   lower    - the lower half UART instance
 *   ptr_data - the pointer to the data beeing sent
 *   sz       - the size of the data that we would like to send
 *
 * Return Value:
 *   OK in case of success otherwise a negative error code.
 *
 ****************************************************************************/

static int sim_lpuart_write(const struct uart_lower_s *lower,
                            const void *ptr_data,
                            unsigned int sz)
{
  for (int i = 0; i < sz; i++)
   host_console_putc((int)*((uint8_t *)ptr_data + i)); 
  return OK;
}

/****************************************************************************
 * Name: sim_lpuart_read
 *
 * Description:
 *   This function reads data from the simulated UART.
 * 
 * Input Parameters:
 *   lower    - the lower half UART instance
 *   data     - the pointer to the buffer where we store datat
 *   max_buf_sz - the size of the buffer
 *
 * Return Value:
 *   OK in case of success otherwise a negative error code.
 *
 ****************************************************************************/

static int sim_lpuart_read(const struct uart_lower_s *lower, void *data,
                           unsigned int max_buf_sz)
{
  return OK;
}

/****************************************************************************
 * Name: sim_lpuart_config
 *
 * Description:
 *   NOT USED.
 *
 * Return Value:
 *   OK in case of success otherwise a negative error code.
 *
 ****************************************************************************/

static int sim_lpuart_config(struct uart_lower_s *lower)
{
  return OK;
}

/****************************************************************************
 * Name: sim_lpuart_int
 *
 * Description:
 *   This function is invoked when we receive events from the simulated 
 *   interrupt peripheral. It acts as an interrupt handler..
 * 
 ****************************************************************************/

static void sim_lpuart_int(void)
{
  struct uart_lower_s *lower = &g_uart_lowerhalfs[0];

  if (g_uart_peripheral.uart_reg_read_index != g_uart_peripheral.uart_reg_write_index) {

    lower->rx_buffer[lower->index_write_rx_buffer] = g_uart_peripheral.sim_uart_data_fifo[g_uart_peripheral.uart_reg_read_index];
    g_uart_peripheral.uart_reg_read_index = (g_uart_peripheral.uart_reg_read_index + 1) % CONFIG_SIM_LPUART_FIFO_SIZE;

    lower->index_write_rx_buffer = (lower->index_write_rx_buffer + 1) % UART_RX_BUFFER;

    /* Notify incomming RX characters */

    sem_post(&lower->rx_notify);
  }
}

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

/****************************************************************************
 * Name: uart_init
 *
 * Description:
 *   This function initializes the simulated UART and creates entries in 
 *   /dev/uartX.
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
