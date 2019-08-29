#include <stdint.h>
#include <board.h>
#include <semaphore.h>
#include <serial.h>
#include <stdlib.h>
#include <scheduler.h>

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
#define UART_INTENSET_OFFSET                (0x304)
#define UART_RXD_PTR                        (0x534)
#define UART_RX_MAXCNT                      (0x538)
#define UART_RX_AMOUNT                      (0x53C)
#define UART_EVENTS_RXSTARTED_OFFSET        (0x14C)
#define UART_EVENTS_RXDRDY_OFFSET           (0x108)
#define UART_EVENTS_ENDRX_OFFSET            (0x110)
#define UART_SHORTS_OFFSET                  (0x200)

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
#define UART_INTENSET_CONFIG   UART_CONFIG(UART_INTENSET_OFFSET)
#define UART_RXD_PTR_CONFIG    UART_CONFIG(UART_RXD_PTR)
#define UART_RX_MAXCNT_CONFIG  UART_CONFIG(UART_RX_MAXCNT)
#define UART_TASK_START_RX_CFG UART_CONFIG(UART_TASK_START_RX_OFFSET)
#define UART_SHORTS_CONFIG     UART_CONFIG(UART_SHORTS_OFFSET)

#define UART_EVENTS_RXSTARTED_CFG  UART_CONFIG(UART_EVENTS_RXSTARTED_OFFSET)
#define UART_EVENTS_RXDRDY_CFG     UART_CONFIG(UART_EVENTS_RXDRDY_OFFSET   )
#define UART_EVENTS_ENDRX_CFG      UART_CONFIG(UART_EVENTS_ENDRX_OFFSET    )

/* Board configs : this should not stay in driver code */
#define UART_TX_PIN                         (6)   /* range 0 - 31 */
#define UART_TX_PORT                        (0)   /* range 0 - 1  */

#define UART_RX_PIN                         (8)   /* range 0 - 31 */
#define UART_RX_PORT                        (0)   /* range 0 - 1  */

#define UART_DMA_RX_LEN                     (UART_RX_BUFFER / 8)

/****************************************************************************
 * Private Types
 ****************************************************************************/

/****************************************************************************
 * Private Functions Definition
 ****************************************************************************/

static int nrf52840_lpuart_open(struct uart_lower_s *lower);

/****************************************************************************
 * Private Data
 ****************************************************************************/

static char g_uart_tx_buffer[UART_TX_BUFFER];
static sem_t g_uart_sema;

static struct uart_lower_s g_uart_low_0 =
{
  .open_cb = nrf52840_lpuart_open,
};

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int uart_low_init(void)
{
  /* Configure UART - no hardware flow control, no pairty, one stop bit */

  UART_CONFIG_REG = 0;

  /* Enable EndRX and RX started event */
  UART_INTENSET_CONFIG = (1 << 4) | (1 << 19) | (1 << 2);
  /* Configure UART baud rate 115200 */

  UART_BAUDRATE = 0x01D60000;

  /* Configure pins - we need only TX and RX */

  UART_TX_PORT_CONFIG = UART_TX_PIN;
  UART_RX_PORT_CONFIG = UART_RX_PIN;

  /* Enable UART0 */

  UART_ENABLE = 0x08;
  sem_init(&g_uart_sema, 0, 1);

  UART_SHORTS_CONFIG     = (1 << 5);
  UART_RXD_PTR_CONFIG    = (uint32_t)g_uart_low_0.rx_buffer;
  UART_RX_MAXCNT_CONFIG  = UART_RX_BUFFER;
  UART_TASK_START_RX_CFG = 1;
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
  return c;
}

static void nrf52840_lpuart_int(void)
{
  /* EVENTS_RXDRDY is received every time - also EVENTS_TXDRDY */

  if (UART_EVENTS_RXSTARTED_CFG == 1)
  {
    UART_EVENTS_RXSTARTED_CFG = 0;

    UART_RXD_PTR_CONFIG    = (uint32_t)g_uart_low_0.rx_buffer;
    UART_RX_MAXCNT_CONFIG  = UART_RX_BUFFER;
  }
  else if (UART_EVENTS_ENDRX_CFG == 1)
  {
    UART_EVENTS_ENDRX_CFG  = 0;
    UART_TASK_START_RX_CFG = 1;
    UART_EVENTS_RXDRDY_CFG = 0;

    sem_post(&g_uart_low_0.rx_notify);
  }
  else if (UART_EVENTS_RXDRDY_CFG == 1)
  {
    UART_EVENTS_RXDRDY_CFG = 0;

    sem_post(&g_uart_low_0.rx_notify);
  }
}

static int nrf52840_lpuart_open(struct uart_lower_s *lower)
{
  /* Initialize the semaphore */

  sem_init(&g_uart_low_0.rx_notify, 0, 0);

  /* Attach the uart interrupt */

  disable_int();

  UART_EVENTS_RXDRDY_CFG    = 0;
  UART_EVENTS_ENDRX_CFG     = 0;
  UART_EVENTS_RXSTARTED_CFG = 0;

  attach_int(UARTE0_UART0_IRQn, nrf52840_lpuart_int);
  NVIC_EnableIRQ(UARTE0_UART0_IRQn);

  enable_int();

  return 0;
}

int uart_init(void)
{
  return uart_register("/dev/ttyUSB0", &g_uart_low_0);
}
