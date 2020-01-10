#include <board.h>
#include <console_main.h>

#include <scheduler.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>

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

#ifdef CONFIG_CONSOLE_LS
int console_ls(int argc, const char *argv[]);
#endif

#ifdef CONFIG_CONSOLE_MOUNT
int console_mount(int argc, const char *argv[]);
int console_umount(int argc, const char *argv[]);
#endif

#ifdef CONFIG_CONSOLE_CAT
int console_cat(int argc, const char *argv[]);
#endif

#ifdef CONFIG_CONSOLE_SENSOR_MEASURE
int console_sensor_measure(int argc, const char *argv[]);
#endif

#ifdef CONFIG_CONSOLE_SLEEP
int console_sleep(int argc, const char *argv[]);
#endif

#ifdef CONFIG_CONSOLE_ECHO
int console_echo(int argc, const char *argv[]);
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
    .cmd_help     = "View/Set the current time or close RTC device",
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

#ifdef CONFIG_CONSOLE_LS
  {
    .cmd_name     = "ls",
    .cmd_function = console_ls,
    .cmd_help     = "List file-system contents",
  },
#endif

#ifdef CONFIG_CONSOLE_MOUNT
  {
    .cmd_name      = "mount",
    .cmd_function  = console_mount,
    .cmd_help      = "Mount a filesystem",
  },
  {
    .cmd_name      = "umount",
    .cmd_function  = console_umount,
    .cmd_help      = "Unmount a previously mounted FS",
  },
#endif

#ifdef CONFIG_CONSOLE_CAT
  {
    .cmd_name     = "cat",
    .cmd_function = console_cat,
    .cmd_help     = "Read files speicifed by <path> argument",
  },
#endif

#ifdef CONFIG_CONSOLE_SENSOR_MEASURE
  {
    .cmd_name     = "sensor_measure",
    .cmd_function = console_sensor_measure,
    .stack_size   = CONFIG_CONSOLE_SENSOR_MEASURE_STACK_SIZE,
    .cmd_help     = "Read data from the gas sensor",
  },
#endif

#ifdef CONFIG_CONSOLE_SLEEP
  {
    .cmd_name            = "sleep",
    .cmd_function        = console_sleep,
    .run_in_main_console = true,
    .cmd_help            = "sleep command in miliseconds",
  },
#endif

#ifdef CONFIG_CONSOLE_ECHO
  {
    .cmd_name            = "echo",
    .cmd_function        = console_echo,
    .run_in_main_console = true,
    .cmd_help            = "Echo a message to the console",
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
#ifdef CONFIG_RUN_APPS_IN_OWN_THREAD
        if (g_cmd_table[j].run_in_main_console)
          return g_cmd_table[j].cmd_function(argc, argv);
        else
          return sched_create_task((
            int (*)(int, char **))g_cmd_table[j].cmd_function,
            g_cmd_table[j].stack_size == 0 ? CONFIG_CONSOLE_STACK_SIZE : g_cmd_table[j].stack_size,
             argc, (char **)argv);
#else
        return g_cmd_table[j].cmd_function(argc, (char **)argv);
#endif /* CONFIG_RUN_APPS_IN_OWN_THREAD */
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
int console_main(int argc, char **argv)
{
  char cmd_buffer[CONFIG_CMD_BUFER_LEN]={0};
  int uart_fd = open(CONFIG_CONSOLE_UART_PATH, 0);
  if (uart_fd < 0)
  {
    return -EINVAL;
  }

  /* We open the RTC device here to prevent the device from going to sleep.
   * when there are no more open references the devices closes and doesn't
   * generate interrupts anymore.
   */
  int rtc_fd = open(CONFIG_RTC_PATH, 0);
  if (rtc_fd < 0)
  {
    return -EINVAL;
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
    else if (*(cmd_buffer + len - 1) == '\b')
    {
      if (len > 1) {
        len-=2;
      }
      write(uart_fd, "\r\n", 2);
      write(uart_fd, CONFIG_CONSOLE_PROMPT_STR, strlen(CONFIG_CONSOLE_PROMPT_STR));
      write(uart_fd, cmd_buffer, len);
    }
    else
    {
      write(uart_fd, cmd_buffer + len - 1, 1);
    }
#endif /* CONFIG_CONSOLE_ECHO_ON */

  } while (g_is_shutdown_set);

  /* This app will exit on a reboot/shutdown command */

  close(uart_fd);
  close(rtc_fd);

  return OK;
}
