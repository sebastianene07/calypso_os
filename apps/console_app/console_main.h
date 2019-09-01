#ifndef __CONSOLE_APP
#define __CONSOLE_APP

/* Console app stack size */

#define CONFIG_CONSOLE_STACK_SIZE       (2048)

/* Uart console path */

#define CONFIG_CONSOLE_UART_PATH        "/dev/ttyUSB0"

/* RTC device path */

#define CONFIG_RTC_PATH                 "/dev/rtc0"

/* Buffer size for commands */

#define CONFIG_CMD_BUFER_LEN            (80)

/*
 * console_main - CatOS console entry point
 *
 */
void console_main(void);

#endif /* __CONSOLE_APP */
