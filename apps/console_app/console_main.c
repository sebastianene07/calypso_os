#include <console_main.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>

/* Shutdown flag */

static bool g_is_shutdown_set;

/*
 * console_main - console application entry point
 *
 */
void console_main(void)
{
  char cmd_buffer[CONFIG_CMD_BUFER_LEN]={0};
  int uart_fd = open(CONFIG_CONSOLE_UART_PATH, 0);
  if (uart_fd < 0)
  {
    return;
  }

  int len = 0;
  g_is_shutdown_set = true;

  do {
    ssize_t sz = read(uart_fd, cmd_buffer + len, 1);
    if (sz > 0)
    {
      len += sz;
      len = len % CONFIG_CMD_BUFER_LEN;
    }
    else
    {
      continue;
    }

    /* If the character was a terminator interpret the command */

    /* Is echo on ? */

    if (*(cmd_buffer + len - 1) == '\r')
    {
      write(uart_fd, "\r\n", 2);
    }
    else
    {
      write(uart_fd, cmd_buffer + len - 1, 1);
    }

  } while (g_is_shutdown_set);

  /* This app will exit on a reboot/shutdown command */

  close(uart_fd);
}
