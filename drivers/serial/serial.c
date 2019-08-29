#include <board.h>

#include <semaphore.h>
#include <serial.h>
#include <string.h>
#include <scheduler.h>
#include <stdlib.h>
#include <vfs.h>
#include <errno.h>

/****************************************************************************
 * Private Function Definitions
 ****************************************************************************/

static int uart_open(void *priv, const char *pathname, int flags, mode_t mode);
static int uart_close(void *priv);
static int uart_write(void *priv, const void *buf, size_t count);
static int uart_read(void *priv, void *buf, size_t count);

/****************************************************************************
 * Private Data
 ****************************************************************************/

struct vfs_ops_s g_uart_ops = {
  .open  = uart_open,
  .close = uart_close,
  .write = uart_write,
  .read  = uart_read,
};

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static int uart_open(void *priv, const char *pathname, int flags, mode_t mode)
{
  struct uart_upper_s *uart_upper = (struct uart_upper_s *)priv;

  /* Call into the lowerhalf open method */

  if (uart_upper->lower->open_cb == NULL) {
    return -ENODEV;
  }

  int ret = uart_upper->lower->open_cb(uart_upper->lower);
  if (ret != OK) {
    return ret;
  }

  /* Grab an entry from the tcb FILE structure. This should be wrapped inside
   * generic open call. */

  struct opened_resource_s *res =
    sched_allocate_resource(priv, &g_uart_ops, mode);
  if (res == NULL) {
    return -ENFILE;
  }

  return res->fd;
}

static int uart_close(void *priv)
{
  return 0;
}

static int uart_write(void *priv, const void *buf, size_t count)
{
}

static int uart_read(void *priv, void *buf, size_t count)
{
  struct uart_upper_s *uart_up = (struct uart_upper_s *)priv;

  /* Blocking read. Wait until 'count' bytes are available from
   * this device.
   */
  struct uart_lower_s *lower = uart_up->lower;
  sem_wait(&lower->rx_notify);

#if 0
  uart_up->lower->index_write_rx_buffer
  uint8_t available_rx_bytes = 0;

  if (lower->index_read_rx_buffer > lower->index_write_rx_buffer)
  {
    available_rx_bytes = uart_up->index_read_rx_buffer -
      uart_up->lower->index_write_rx_buffer;
  }
  else
  {
    available_rx_bytes = UART_RX_BUFFER -
    (uart_up->lower->index_write_rx_buffer - uart_up->index_read_rx_buffer);
  }
#endif

  return 0;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int uart_register(const char *name, const struct uart_lower_s *uart_lowerhalf)
{
  /* Called from lowerhalf driver. Allocate a new upper driver instance where
   * we store the lowerhalf. struct uart_upper_s
   */

  struct uart_upper_s *uart_upper = calloc(sizeof(struct uart_upper_s), 1);
  if (uart_upper == NULL) {
    return -ENOMEM;
  }

  uart_upper->lower = uart_lowerhalf;

  /* Register the upper half node with the VFS */

  int ret = vfs_register_node(name, strlen(name), &g_uart_ops, VFS_TYPE_DEVICE,
    uart_upper);
  if (ret != OK) {
    free(uart_upper);
  }

  return ret;
}
