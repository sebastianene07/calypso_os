#ifndef __CONSOLE_APP
#define __CONSOLE_APP

#include <stdbool.h>

/* Console app stack size */

#define CONFIG_CONSOLE_STACK_SIZE       (2048)

/* Buffer size for commands */

#define CONFIG_CMD_BUFER_LEN            (80)

typedef int (* console_command)(int argc, const char *argv[]);

typedef struct console_command_entry_s
{
  const char *cmd_name;
  console_command cmd_function;
  const char *cmd_help;
#ifdef CONFIG_RUN_APPS_IN_OWN_THREAD
  bool run_in_main_console;
#endif
  uint16_t stack_size;
} console_command_entry_t;


/*
 * console_main - CatOS console entry point
 *
 */
int console_main(int argc, char **argv);

#endif /* __CONSOLE_APP */
