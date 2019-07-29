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

  /* Grab an entry from the tcb FILE structure. This should be wrapped inside
   * generic open call. */

  struct opened_resource_s *res =
    sched_allocate_resource(priv, &g_uart_ops, mode);
  if (res == NULL) {
    return -ENOMEM;
  }

  /* Call into the lowerhalf open method */

  int ret = uart_upper->lower->open_cb(uart_upper->lower);
  if (ret != OK) {
    return ret;
  }

  return res->fd;
}

static int uart_close(void *priv, int fd)
{
  return sched_free_resource(fd);
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
