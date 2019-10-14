#include <board.h>
#include <console_main.h>

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>

#define CONSOLE_HELP_DESCRIPTION      "Prints out the supported commands"

/* Console features */

#ifdef CONFIG_CONSOLE_DATE_ON
int console_date(int argc, const char *argv[]);
#endif

#ifdef CONFIG_CONSOLE_TEST_DISPLAY
int console_display(int argc, const char *argv[]);
#endif

#ifdef CONFIG_CONSOLE_FREE
int console_free(int argc, const char *argv[]);
#endif

static int console_help(int argc, const char *argv[]);

/* Shutdown flag */

static bool g_is_shutdown_set;

/* Command table */

static console_command_entry_t g_cmd_table[] =
{
#ifdef CONFIG_CONSOLE_DATE_ON
  { .cmd_name     = "time",
    .cmd_function = console_date,
    .cmd_help     = "View/Set the current time",
  },
#endif

#ifdef CONFIG_CONSOLE_TEST_DISPLAY
  { .cmd_name     = "display",
    .cmd_function = console_display,
    .cmd_help     = "Test functionality for SSD1331 display"
  },
#endif

#ifdef CONFIG_CONSOLE_FREE
  { .cmd_name     = "free",
    .cmd_function = console_free,
    .cmd_help     = "View the available system memory",
  },
#endif

  { .cmd_name     = "help",
    .cmd_function = console_help,
    .cmd_help     = CONSOLE_HELP_DESCRIPTION
  },
  { .cmd_name     = "?",
    .cmd_function = console_help,
    .cmd_help     = CONSOLE_HELP_DESCRIPTION
  },
};

static int execute_command(int argc, const char *argv[])
{
  for (int i = 0; i < argc; ++i)
  {
    for (int j = 0; j < ARRAY_LEN(g_cmd_table); j++)
    {
      if (strcmp(g_cmd_table[j].cmd_name, argv[0]) == 0)
      {
        return g_cmd_table[j].cmd_function(argc, argv);
      }
    }
  }

  printf("Unknown command: %s\n", argv[0]);
  return -EINVAL;
}

static int console_help(int argc, const char *argv[])
{
  printf("Help menu\n"
         "available commands:\n\n");

  for (int i = 0; i < sizeof(g_cmd_table) / sizeof(g_cmd_table[0]); i++)
  {
    printf("%s - %s\n", g_cmd_table[i].cmd_name,
                        g_cmd_table[i].cmd_help);
  }

  return 0;
}

/*
 * parse_arguments - Parse the console arguments
 *
 */
static int parse_arguments(char *buffer, size_t newline)
{
  buffer[newline] = 0;

  char *context = NULL;
  char *argument = NULL;
  const char *delim = " ";

  const char *argv[CONFIG_CMD_BUFER_LEN];
  int argc = 0;

  argument = strtok_r(buffer, delim, &context);
  if (argument != NULL)
  {
    argv[argc++] = argument;
  }

  while ((argument = strtok_r(NULL, delim, &context)) != NULL)
  {
    if (strlen(argument) > 0)
    {
      argv[argc++] = argument;
    }
  }

  return execute_command(argc, argv);
}

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
      write(uart_fd, CONFIG_CONSOLE_PROMPT_STR, strlen(CONFIG_CONSOLE_PROMPT_STR));
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

#ifdef CONFIG_CONSOLE_ECHO_ON
    /* Is echo on ? */

    if (*(cmd_buffer + len - 1) == '\r')
    {
      write(uart_fd, "\r\n\n", 2);
      is_prompt_printed = true;
      if (len > 1)
      {
        parse_arguments(cmd_buffer, len - 1);
      }
      len = 0;
    }
    else
    {
      write(uart_fd, cmd_buffer + len - 1, 1);
    }
#endif /* CONFIG_CONSOLE_ECHO_ON */

  } while (g_is_shutdown_set);

  /* This app will exit on a reboot/shutdown command */

  close(uart_fd);
}
