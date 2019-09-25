#include <board.h>
#include <console_main.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>

#ifdef CONFIG_CONSOLE_TEST_DISPLAY
#include <../drivers/display/ssd_1331.h>
#endif

#ifdef CONFIG_CONSOLE_DATE_ON
static int date(int argc, const char *argv[]);
#endif

#ifdef CONFIG_CONSOLE_TEST_DISPLAY
static int display(int argc, const char *argv[]);
#endif

/* Shutdown flag */

static bool g_is_shutdown_set;

/* Command table */

static console_command_entry_t g_cmd_table[] =
{
#ifdef CONFIG_CONSOLE_DATE_ON
  { .cmd_name = "date", .cmd_function = date },
#endif
#ifdef CONFIG_CONSOLE_TEST_DISPLAY
  { .cmd_name = "display", .cmd_function = display }
#endif
};

#ifdef CONFIG_CONSOLE_DATE_ON

#define SEC_OFFSET              (2)
#define MIN_OFFSET              (1)
#define HOUR_OFFSET             (0)

/* Num args for date set command */
#define NUM_ARGS_DATE_SET       (3)

/* The clock that will be shown hh:min:sec */
static uint8_t g_clock[3];

/* The set clock hh:min:sec (offset) */
static uint32_t g_clock_offset[3];

static uint32_t tick_offset;

/*
 * date - View/Set the current time
 *
 */
static int date(int argc, const char *argv[])
{
  uint32_t ticks = 0;

  int rtc_fd = open(CONFIG_RTC_PATH, 0);
  if (rtc_fd < 0)
  {
    return -EINVAL;
  }

  int ret = read(rtc_fd, &ticks, sizeof(ticks));
  if (ret < 0)
  {
    return ret;
  }

  if (argc == NUM_ARGS_DATE_SET && !strcmp(argv[1], "set"))
  {
    /* expected hour input in this format hour:min:sec */
    tick_offset = ticks;

    sscanf(argv[2], "%d:%d:%d", &g_clock_offset[HOUR_OFFSET],
                                &g_clock_offset[MIN_OFFSET],
                                &g_clock_offset[SEC_OFFSET]);
  }

  uint32_t seconds = (ticks - tick_offset) >> 3;
  uint32_t minutes = seconds / 60;
  uint32_t hour = minutes / 60;

  g_clock[SEC_OFFSET] = (seconds + g_clock_offset[SEC_OFFSET]) % 60;
  g_clock[MIN_OFFSET] = (minutes + g_clock_offset[MIN_OFFSET]) % 60;
  g_clock[HOUR_OFFSET] = (hour + g_clock_offset[HOUR_OFFSET]) % 24;

  printf("Local time: %02u : %02u : %02u\n", g_clock[HOUR_OFFSET], g_clock[MIN_OFFSET],
    g_clock[SEC_OFFSET]);
  close(rtc_fd);

  return 0;
}
#endif /* CONFIG_CONSOLE_DATE_ON */

#ifdef CONFIG_CONSOLE_TEST_DISPLAY
static Color get_color_from_name(const char *color_name)
{
  if (!strcmp("BLACK", color_name))
    return COLOR_BLACK;
  else if (!strcmp("GREY", color_name)) 
    return COLOR_GREY;
  else if (!strcmp("WHITE", color_name))
    return COLOR_WHITE;
  else if (!strcmp("RED", color_name))
    return COLOR_RED;
  else if (!strcmp("PINK", color_name))
    return COLOR_PINK;
  else if (!strcmp("YELLOW", color_name))
    return COLOR_YELLOW;
  else if (!strcmp("GOLDEN", color_name))
    return COLOR_GOLDEN;
  else if (!strcmp("BROWN", color_name))
    return COLOR_BROWN;
  else if (!strcmp("BLUE", color_name))
    return COLOR_BLUE;
  else if (!strcmp("CYAN", color_name))
    return COLOR_CYAN;
  else if (!strcmp("GREEN", color_name))
    return COLOR_GREEN;
  else 
    return COLOR_PURPLE;
} 

static DisplayPower get_power_from_name(const char *power_name)
{
  if (!strcmp("DIM", power_name))
    return DimMode;
  else if (!strcmp("SLEEP", power_name))
    return SleepMode;
  else 
    return NormalMode;
}

static DisplayMode get_mode_from_name(const char *mode_name)
{
  if (!strcmp("NORMAL", mode_name))
    return NormalDisplay;
  else if (!strcmp("ON", mode_name))
    return DisplayOn;
  else if (!strcmp("OFF", mode_name)) 
    return DisplayOff;
  else
    return InverseDisplay;
}

/*
 * display - Test the SSD1331 display functionality 
 *
 */
static int display(int argc, const char *argv[])
{
  if (argc >= 2)
  {
    if (strcmp(argv[1], "frame") == 0)
    {
      if (argc == 4)
      {
        Color frame_color = get_color_from_name(argv[2]);
        Color fill_color  = get_color_from_name(argv[3]);

        ssd1331_display_drawFrame(0, 0, RGB_OLED_WIDTH, RGB_OLED_HEIGHT, frame_color, fill_color); 
      }
      else
      {
        printf("Wrong command: display frame <frame_color> <fill_color>\n"
               "{BLACK | GREY | WHITE | RED | PINK | YELLOW | GOLDEN" 
               "| BROWN | BLUE | CYAN | GREEN | PURPLE}\n");
      }
    }
    else if (strcmp(argv[1], "clear") == 0)
    {
      ssd1331_display_clearWindow(0, 0, RGB_OLED_WIDTH, RGB_OLED_HEIGHT);
    }
    else if (strcmp(argv[1], "dim") == 0)
    {
      ssd1331_display_dimWindow(0, 0, RGB_OLED_WIDTH, RGB_OLED_HEIGHT);
    }
    else if (strcmp(argv[1], "mode") == 0)
    {
      if (argc == 3)
      {
        ssd1331_display_setDisplayMode(get_mode_from_name(argv[2]));
      }
      else
      {
        printf("Wrong command: display mode <mode_name>\r\n"
               "{ NORMAL | ON | OFF | INVERT }\r\n");
      }
    }
    else if (strcmp(argv[1], "power") == 0)
    {
      if (argc == 3)
      {
        ssd1331_display_setDisplayPower(get_power_from_name(argv[2]));
      }
      else
      {
        printf("Wrong command: display power <power_mode>\r\n"
               "{ DIM | SLEEP | NORMAL }\r\n");
      }
    }
  }
  else
  {
    printf("Supported operations are:\r\n""display frame <frame_color> <fill_color>\r\n"
        "display clear\r\n"
        "display dim\r\n"
        "display mode <mode_name>\r\n"
        "display power <power_mode>\r\n");
  }

  return 0;
}

#endif /* CONFIG_CONSOLE_TEST_DISPLAY */

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
      write(uart_fd, "\r\n", 2);
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
