#include <stdint.h>

#define UART_BASE                           (0x40002000)

#define UART_ENABLE_OFFSET                  (0x500)
#define UART_PIN_SEL_RTS_OFSSET             (0x508)
#define UART_PIN_SEL_TXD_OFFSET             (0x50C)
#define UART_PIN_SEL_CTS_OFFSET             (0x510)
#define UART_PIN_SEL_RXD_OFFSET             (0x514)
#define UART_TASK_START_RX_OFFSET           (0x00)
#define UART_TASK_STOP_RX_OFFSET            (0x04)
#define UART_TASK_START_TX_OFFSET           (0x08)
#define UART_TASK_STOP_TX_OFFSET            (0x0C)
#define UART_CONFIG_OFFSET                  (0x56C)
#define UART_BAUDRATE_OFFSET                (0x524)
#define UART_TXD_OFFSET                     (0x544)
#define UART_TXD_MAXCNT                     (0x548)
#define UART_ENDTX_OFFSET                   (0x120)

/* UART configuration fields */

#define UART_HWFC                           (0x00)

#define UART_ENABLE (*((uint32_t *)(UART_BASE + UART_ENABLE_OFFSET)))
#define UART_CONFIG (*((uint32_t *) (UART_BASE + UART_CONFIG_OFFSET)))
#define UART_BAUDRATE (*((uint32_t *)(UART_BASE + UART_BAUDRATE_OFFSET)))
#define UART_TX_PORT_CONFIG (*((uint32_t *)(UART_BASE + UART_PIN_SEL_TXD_OFFSET)))
#define UART_RX_PORT_CONFIG (*((uint32_t *)(UART_BASE + UART_PIN_SEL_RXD_OFFSET)))
#define UART_TXD_PTR_CONFIG (*((uint32_t *)(UART_BASE + UART_TXD_OFFSET)))
#define UART_TXD_MAXCNT_CONFIG (*((uint32_t *)(UART_BASE + UART_TXD_MAXCNT)))
#define UART_TX_START_TASK (*((uint32_t *)(UART_BASE + UART_TASK_START_TX_OFFSET)))
#define UART_ENDTX (*((uint32_t *)(UART_BASE + UART_ENDTX_OFFSET)))
#define UART_STOP_TX_TASK (*((uint32_t *)(UART_BASE + 0x00C)))

/* Board configs : this should not stay in driver code */

#define UART_TX_PIN                         (6)   /* range 0 - 31 */
#define UART_TX_PORT                        (0)   /* range 0 - 1  */

#define UART_RX_PIN                         (8)   /* range 0 - 31 */
#define UART_RX_PORT                        (0)   /* range 0 - 1  */

int uart_init(void)
{
  /* Configure UART - hardware flow control, no pairty, one stop bit */

  UART_CONFIG = 0;//(0 << UART_HWFC);

  /* Configure UART baud rate 115200 */

  UART_BAUDRATE = 0x01D60000;

  /* Configure pins - we need only TX and RX */

  UART_TX_PORT_CONFIG = UART_TX_PIN;
  UART_RX_PORT_CONFIG = UART_RX_PIN;

  /* Enable UART0 */

  UART_ENABLE = 0x08;

  return 0;
}

int uart_send(char *msg, int msg_len)
{
  UART_TXD_PTR_CONFIG     = (uint32_t)msg;
  UART_TXD_MAXCNT_CONFIG  = msg_len;
  UART_TX_START_TASK      = 1;

  while (UART_ENDTX == 0)
  {
    ;;
  }

  return 0;
}

char uart_receive(void)
{
  char c = 0;

  return c;
}
