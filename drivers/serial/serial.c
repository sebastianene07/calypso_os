#include <board.h>

#include <semaphore.h>
#include <serial.h>
#include <scheduler.h>
#include <stdlib.h>
#include <vfs.h>
#include <errno.h>

/****************************************************************************
 * Private Function Definitions
 ****************************************************************************/

static int uart_open(void *priv, const char *pathname, int flags, mode_t mode);
static int uart_close(void *priv, int fd);
static int uart_write(void *priv, int fd, const void *buf, size_t count);
static int uart_read(void *priv, int fd, void *buf, size_t count);

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

  /* Grab an entry from the tcb FILE structure */

  disable_int();
  struct tcb_s *curr_tcb = sched_get_current_task();
  enable_int();
}

static int uart_close(void *priv, int fd)
{
}

static int uart_write(void *priv, int fd, const void *buf, size_t count)
{
}

static int uart_read(void *priv, int fd, void *buf, size_t count)
{
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
