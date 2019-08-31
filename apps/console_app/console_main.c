#include <console_main.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>

#define CONSOLE_PROMPT_STR                        "root:#/>"
#define CONSOLE_ECHO_ON

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
  bool is_prompt_printed = true;
  g_is_shutdown_set = true;

  do {

    if (is_prompt_printed)
    {
      write(uart_fd, CONSOLE_PROMPT_STR, strlen(CONSOLE_PROMPT_STR));
      is_prompt_printed = false;
    }

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

#ifdef CONSOLE_ECHO_ON
    /* Is echo on ? */

    if (*(cmd_buffer + len - 1) == '\r')
    {
      write(uart_fd, "\r\n", 2);
      is_prompt_printed = true;
    }
    else
    {
      write(uart_fd, cmd_buffer + len - 1, 1);
    }
#endif /* CONSOLE_ECHO_ON */

  } while (g_is_shutdown_set);

  /* This app will exit on a reboot/shutdown command */

  close(uart_fd);
}
