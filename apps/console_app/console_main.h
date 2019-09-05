#ifndef __CONSOLE_APP
#define __CONSOLE_APP

/* Console app stack size */

#define CONFIG_CONSOLE_STACK_SIZE       (2048)

/* Buffer size for commands */

#define CONFIG_CMD_BUFER_LEN            (80)

typedef int (* console_command)(int argc, const char *argv[]);

typedef struct console_command_entry_s
{
  const char *cmd_name;
  console_command cmd_function;
} console_command_entry_t;


/*
 * console_main - CatOS console entry point
 *
 */
void console_main(void);

#endif /* __CONSOLE_APP */
