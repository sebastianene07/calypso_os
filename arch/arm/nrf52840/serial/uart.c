#include <stdint.h>
#include <board.h>
#include <semaphore.h>
#include <serial.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* Uart 0 base register */

#define UART_BASE                           (0x40002000)

/* Uart configuration options register offsets */

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
#define UART_EVENTS_TXSTOPPED_OFFSET        (0x158)

/* UART configuration fields */

#define UART_CONFIG(offset_r)  ((*((volatile uint32_t *)(UART_BASE + (offset_r)))))

#define UART_ENABLE            UART_CONFIG(UART_ENABLE_OFFSET)
#define UART_CONFIG_REG        UART_CONFIG(UART_CONFIG_OFFSET)
#define UART_BAUDRATE          UART_CONFIG(UART_BAUDRATE_OFFSET)
#define UART_TX_PORT_CONFIG    UART_CONFIG(UART_PIN_SEL_TXD_OFFSET)
#define UART_RX_PORT_CONFIG    UART_CONFIG(UART_PIN_SEL_RXD_OFFSET)
#define UART_TXD_PTR_CONFIG    UART_CONFIG(UART_TXD_OFFSET)
#define UART_TXD_MAXCNT_CONFIG UART_CONFIG(UART_TXD_MAXCNT)
#define UART_TX_START_TASK     UART_CONFIG(UART_TASK_START_TX_OFFSET)
#define UART_ENDTX             UART_CONFIG(UART_ENDTX_OFFSET)
#define UART_STOP_TX_TASK      UART_CONFIG(UART_TASK_STOP_TX_OFFSET)
#define UART_EVENTS_TXSTOPPED  UART_CONFIG(UART_EVENTS_TXSTOPPED_OFFSET)

/* Board configs : this should not stay in driver code */

#define UART_TX_PIN                         (6)   /* range 0 - 31 */
#define UART_TX_PORT                        (0)   /* range 0 - 1  */

#define UART_RX_PIN                         (8)   /* range 0 - 31 */
#define UART_RX_PORT                        (0)   /* range 0 - 1  */

/****************************************************************************
 * Private Data
 ****************************************************************************/

static char g_uart_tx_buffer[UART_TX_BUFFER];
static char g_uart_rx_buffer[UART_RX_BUFFER];
static sem_t g_uart_sema;

static struct uart_lower_s g_uart_low_0;

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int uart_low_init(void)
{
  /* Configure UART - no hardware flow control, no pairty, one stop bit */

  UART_CONFIG_REG = 0;

  /* Configure UART baud rate 115200 */

  UART_BAUDRATE = 0x01D60000;

  /* Configure pins - we need only TX and RX */

  UART_TX_PORT_CONFIG = UART_TX_PIN;
  UART_RX_PORT_CONFIG = UART_RX_PIN;

  /* Enable UART0 */

  UART_ENABLE = 0x08;
  sem_init(&g_uart_sema, 0, 1);

  return 0;
}

int uart_low_send(char *msg)
{
  sem_wait(&g_uart_sema);

  UART_EVENTS_TXSTOPPED = 0;
  UART_ENDTX            = 0;

  int i = 0;
  for (msg; *msg != NULL; msg++)
  {
    g_uart_tx_buffer[i] = *msg;
    i++;
  }

  UART_TXD_PTR_CONFIG     = (uint32_t)g_uart_tx_buffer;
  UART_TXD_MAXCNT_CONFIG  = i;
  UART_TX_START_TASK      = 1;

  while (UART_ENDTX == 0);

  /* Stop the UART TX */
  UART_STOP_TX_TASK = 1;

  /* Wait until we receive the stopped event */
  while (UART_EVENTS_TXSTOPPED == 0);

  UART_TX_START_TASK = 0;
  sem_post(&g_uart_sema);

  return 0;
}

char uart_low_receive(void)
{
  char c = 0;

  /* Not implemented */
  return c;
}

int uart_init(void)
{
  return uart_register("/dev/ttyUSB0", &g_uart_low_0);
}
