#ifndef __SERIAL_H
#define __SERIAL_H

#include <stdint.h>
#include <semaphore.h>
#include <stddef.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define UART_TX_BUFFER                      (64)
#define UART_RX_BUFFER                      (64)

/****************************************************************************
 * Public Types
 ****************************************************************************/

/* Further declaration */

struct uart_lower_s;

/* Lowerhalf callback that should be implemented by the serial driver */

typedef int (*uart_lowerhalf_open)(const struct uart_lower_s *lower);
typedef int (*uart_lowerhalf_close)(const struct uart_lower_s *lower);
typedef int (*uart_lowerhalf_write)(const struct uart_lower_s *lower,
                                    const void *ptr_data,
                                    unsigned int sz);
typedef int (*uart_lowerhalf_read)(const struct uart_lower_s *lower,
                                   void *data,
                                   unsigned int max_buf_sz);
typedef int (*uart_lowerhalf_ioctl)(const struct uart_lower_s *lower);

/* The lower half structure used by the serial driver */

struct uart_lower_s {
  void *priv;
  uint8_t rx_buffer[UART_RX_BUFFER];
  uint8_t index_write_rx_buffer;
  uint8_t index_read_rx_buffer;
  sem_t rx_notify;
  uint8_t tx_buffer[UART_TX_BUFFER];
  sem_t tx_notify;
  sem_t lock;
  uart_lowerhalf_open  open_cb;
  uart_lowerhalf_close close_cb;
  uart_lowerhalf_write write_cb;
  uart_lowerhalf_read  read_cb;
  uart_lowerhalf_ioctl ioctl_cb;
};

/* The upper half structure */

struct uart_upper_s {
  uint8_t index_read;
  const struct uart_lower_s *lower;
};

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int uart_low_init(void);

int putchar(int c);

int uart_register(const char *name, const struct uart_lower_s *uart_lowerhalf);

struct uart_lower_s **uart_init(size_t *uart_num);

sem_t *get_console_sema(void);

#endif /* __SERIAL_H */
