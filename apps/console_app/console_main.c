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

#if defined(CONFIG_CONSOLE_FREE) && !defined(CONFIG_SEPARATE_BUNDLES)
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

#ifdef CONFIG_CONSOLE_NRF_INIT_SOFTDEVICE_APP
int console_nrf_init(int argc, const char *argv[]);
#endif

#ifdef CONFIG_CONSOLE_RM
int console_rm(int argc, const char *argv[]);
#endif

#ifdef CONFIG_CONSOLE_TOUCH
int console_touch(int argc, const char *argv[]);
#endif

#ifdef CONFIG_CONSOLE_MKDIR
int console_mkdir(int argc, const char *argv[]);
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
    .stack_size   = CONFIG_CONSOLE_STACK_SIZE,
    .cmd_help     = "View/Set the current time or close RTC device",
  },
#endif

#ifdef CONFIG_CONSOLE_TEST_DISPLAY
  { .cmd_name     = "display",
    .cmd_function = console_display,
    .stack_size   = CONFIG_CONSOLE_STACK_SIZE,
    .cmd_help     = "Test functionality for SSD1331 display"
  },
#endif

#if defined(CONFIG_CONSOLE_FREE) && !defined(CONFIG_SEPARATE_BUNDLES)
  { .cmd_name     = "free",
    .cmd_function = console_free,
    .stack_size   = CONFIG_CONSOLE_STACK_SIZE,
    .cmd_help     = "View the available system memory",
  },
#endif

#ifdef CONFIG_CONSOLE_LS
  {
    .cmd_name     = "ls",
    .cmd_function = console_ls,
    .stack_size   = CONFIG_CONSOLE_STACK_SIZE,
    .cmd_help     = "List file-system contents",
  },
#endif

#ifdef CONFIG_CONSOLE_MOUNT
  {
    .cmd_name      = "mount",
    .cmd_function  = console_mount,
    .stack_size    = CONFIG_CONSOLE_STACK_SIZE,
    .cmd_help      = "Mount a filesystem",
  },
  {
    .cmd_name      = "umount",
    .cmd_function  = console_umount,
    .stack_size    = CONFIG_CONSOLE_STACK_SIZE,
    .cmd_help      = "Unmount a previously mounted FS",
  },
#endif

#ifdef CONFIG_CONSOLE_CAT
  {
    .cmd_name     = "cat",
    .cmd_function = console_cat,
    .stack_size   = CONFIG_CONSOLE_STACK_SIZE,
    .cmd_help     = "Read files speicifed by <path> argument",
  },
#endif

#ifdef CONFIG_CONSOLE_SENSOR_MEASURE
  {
    .cmd_name     = "sensor_measure",
    .cmd_function = console_sensor_measure,
    .stack_size   = CONFIG_CONSOLE_SENSOR_MEASURE_STACKSIZE,
    .cmd_help     = "Read data from the gas sensor",
  },
#endif

#ifdef CONFIG_CONSOLE_SLEEP
  {
    .cmd_name            = "sleep",
    .cmd_function        = console_sleep,
    .stack_size          = CONFIG_CONSOLE_STACK_SIZE,
#ifdef CONFIG_RUN_APPS_IN_OWN_THREAD
    .run_in_main_console = true,
#endif
    .cmd_help            = "sleep command in miliseconds",
  },
#endif

#ifdef CONFIG_CONSOLE_ECHO
  {
    .cmd_name            = "echo",
    .cmd_function        = console_echo,
    .stack_size          = CONFIG_CONSOLE_STACK_SIZE,
#ifdef CONFIG_RUN_APPS_IN_OWN_THREAD
    .run_in_main_console = true,
#endif
    .cmd_help            = "Echo a message to the console",
  },
#endif

#ifdef CONFIG_CONSOLE_NRF_INIT_SOFTDEVICE_APP
  { .cmd_name            = "nrf_init",
    .cmd_function        = console_nrf_init,
    .stack_size          = CONFIG_CONSOLE_NRF_INIT_SOFTDEVICE_APP_STACK_SIZE,
    .cmd_help            = "Nordic soft device application",
  },
#endif

#ifdef CONFIG_CONSOLE_RM
  { .cmd_name            = "rm",
    .cmd_function        = console_rm,
    .stack_size          = CONFIG_CONSOLE_STACK_SIZE,
    .cmd_help            = "Remove a file",
  },
#endif

#ifdef CONFIG_CONSOLE_TOUCH
  { .cmd_name            = "touch",
    .cmd_function        = console_touch,
    .stack_size          = CONFIG_CONSOLE_STACK_SIZE,
    .cmd_help            = "Create a new file",
  },
#endif

#ifdef CONFIG_CONSOLE_MKDIR
  { .cmd_name            = "mkdir",
    .cmd_function        = console_mkdir,
    .stack_size          = CONFIG_CONSOLE_STACK_SIZE,
    .cmd_help            = "Create a new directory",
  },
#endif

  { .cmd_name     = "help",
    .cmd_function = console_help,
    .stack_size   = CONFIG_CONSOLE_STACK_SIZE,
    .cmd_help     = CONSOLE_HELP_DESCRIPTION
  },
  { .cmd_name     = "?",
    .cmd_function = console_help,
    .stack_size   = CONFIG_CONSOLE_STACK_SIZE,
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
            g_cmd_table[j].stack_size,
            argc,
            (char **)argv,
            g_cmd_table[j].cmd_name);
#else
        return g_cmd_table[j].cmd_function(argc, argv);
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
 * main - console application entry point
 *
 */
int main(int argc, char **argv)
{
  char cmd_buffer[CONFIG_CMD_BUFER_LEN]={0};

  printf("\n[console_main] Entry point\n");
  int uart_fd = open(CONFIG_CONSOLE_UART_PATH, 0);
  if (uart_fd < 0)
  {
    printf("[console_main] Cannot open UART exit console\n");
    return -EINVAL;
  }

#ifdef CONFIG_RTC_DRIVER
  /* We open the RTC device here to prevent the RTC peripheral from going to
   * sleep.When there are no more open references the devices closes and doesn't
   * generate interrupts anymore and time is not keeped.
   */
  int rtc_fd = open(CONFIG_RTC_PATH, 0);
  if (rtc_fd < 0)
  {
    printf("Cannot open RTC device %d\n", rtc_fd);
    return -EINVAL;
  }
#endif

  /* We open the RTC device here to prevent the device from going to sleep.
   * when there are no more open references the devices closes and doesn't
   * generate interrupts anymore.
   */
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

    if (*(cmd_buffer + len - 1) == '\r' || *(cmd_buffer + len - 1) == 10)
    {
      write(uart_fd, "\r\n\n", 2);
      is_prompt_printed = true;
      if (len > 1)
      {
        parse_arguments(cmd_buffer, len - 1);
      }
      len = 0;
    }
    else if (*(cmd_buffer + len - 1) == '\b' || *(cmd_buffer + len - 1) == 127)
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

#ifdef CONFIG_RTC_DRIVER
  close(rtc_fd);
#endif
  return OK;
}
