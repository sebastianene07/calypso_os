#include <stdint.h>
#include <board.h>
#include <semaphore.h>
#include <serial.h>
#include <stdlib.h>
#include <string.h>
#include <scheduler.h>
#include <gpio.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* Uart 0 base register for peripheral number zero */

#define UART_BASE_0                           (0x40002000)

/* Uart 1 base register for the second peripheral */

#define UART_BASE_1                           (0x40028000)

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

#define UART_CONFIG(base_peripheral, offset_r)  ((*((volatile uint32_t *)((base_peripheral) + (offset_r)))))

#define UART_ENABLE(base_peripheral)            UART_CONFIG((base_peripheral) , UART_ENABLE_OFFSET)
#define UART_CONFIG_REG(base_peripheral)        UART_CONFIG((base_peripheral) , UART_CONFIG_OFFSET)
#define UART_BAUDRATE(base_peripheral)          UART_CONFIG((base_peripheral) , UART_BAUDRATE_OFFSET)
#define UART_TX_PORT_CONFIG(base_peripheral)    UART_CONFIG((base_peripheral) , UART_PIN_SEL_TXD_OFFSET)
#define UART_RX_PORT_CONFIG(base_peripheral)    UART_CONFIG((base_peripheral) , UART_PIN_SEL_RXD_OFFSET)
#define UART_TXD_PTR_CONFIG(base_peripheral)    UART_CONFIG((base_peripheral) , UART_TXD_OFFSET)
#define UART_TXD_MAXCNT_CONFIG(base_peripheral) UART_CONFIG((base_peripheral) , UART_TXD_MAXCNT)
#define UART_TX_START_TASK(base_peripheral)     UART_CONFIG((base_peripheral) , UART_TASK_START_TX_OFFSET)
#define UART_ENDTX(base_peripheral)             UART_CONFIG((base_peripheral) , UART_ENDTX_OFFSET)
#define UART_STOP_TX_TASK(base_peripheral)      UART_CONFIG((base_peripheral) , UART_TASK_STOP_TX_OFFSET)
#define UART_EVENTS_TXSTOPPED(base_peripheral)  UART_CONFIG((base_peripheral) , UART_EVENTS_TXSTOPPED_OFFSET)
#define UART_INTENSET_CONFIG(base_peripheral)   UART_CONFIG((base_peripheral) , UART_INTENSET_OFFSET)
#define UART_RXD_PTR_CONFIG(base_peripheral)    UART_CONFIG((base_peripheral) , UART_RXD_PTR)
#define UART_RX_MAXCNT_CONFIG(base_peripheral)  UART_CONFIG((base_peripheral) , UART_RX_MAXCNT)
#define UART_RX_AMOUNT_CFG(base_peripheral)     UART_CONFIG((base_peripheral) , UART_RX_AMOUNT)
#define UART_TASK_START_RX_CFG(base_peripheral) UART_CONFIG((base_peripheral) , UART_TASK_START_RX_OFFSET)
#define UART_SHORTS_CONFIG(base_peripheral)     UART_CONFIG((base_peripheral) , UART_SHORTS_OFFSET)

#define UART_EVENTS_RXSTARTED_CFG(base_peripheral)  UART_CONFIG((base_peripheral) , UART_EVENTS_RXSTARTED_OFFSET)
#define UART_EVENTS_RXDRDY_CFG(base_peripheral)     UART_CONFIG((base_peripheral) , UART_EVENTS_RXDRDY_OFFSET   )
#define UART_EVENTS_ENDRX_CFG(base_peripheral)      UART_CONFIG((base_peripheral) , UART_EVENTS_ENDRX_OFFSET    )

#define UART_DMA_RX_LEN                     (UART_RX_BUFFER / 8)

/****************************************************************************
 * Private Types
 ****************************************************************************/

struct nrf52840_uart_priv_s {
  void *base_peripheral_ptr;
  sem_t uart_sema;
  char uart_tx_buffer[UART_TX_BUFFER];
  IRQn_Type irq;
};

/****************************************************************************
 * Private Functions Definition
 ****************************************************************************/

static int nrf52840_lpuart_open(const struct uart_lower_s *lower);
static int nrf52840_lpuart_write(const struct uart_lower_s *lower,
                                 const void *ptr_data,
                                 unsigned int sz);

/****************************************************************************
 * Private Data
 ****************************************************************************/

static struct nrf52840_uart_priv_s g_uart_low_0_priv = 
{
  .base_peripheral_ptr = (void *)UART_BASE_0,
  .irq                 = UARTE0_UART0_IRQn,
};

static struct uart_lower_s g_uart_low_0 =
{
  .priv     = &g_uart_low_0_priv,  
  .open_cb  = nrf52840_lpuart_open,
  .write_cb = nrf52840_lpuart_write,
};

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int uart_low_init(void)
{
  /* Configure UART - no hardware flow control, no pairty, one stop bit */

  UART_CONFIG_REG(UART_BASE_0) = 0;

  /* Enable EndRX and RX started event */
  UART_INTENSET_CONFIG(UART_BASE_0) = (1 << 4) | (1 << 19) | (1 << 2);

  /* Configure UART baud rate 115200 */

  UART_BAUDRATE(UART_BASE_0) = CONFIG_SERIAL_CONSOLE_BAUDRATE;
 
  /* Configure pins - we need only TX and RX */
  gpio_configure(CONFIG_SERIAL_CONSOLE_RX_PIN, CONFIG_SERIAL_CONSOLE_RX_PORT,
                 GPIO_DIRECTION_IN,
                 GPIO_PIN_INPUT_CONNECT, GPIO_NO_PULL,
                 GPIO_PIN_S0S1, GPIO_PIN_SENSE_LOW);

  UART_TX_PORT_CONFIG(UART_BASE_0) = CONFIG_SERIAL_CONSOLE_TX_PIN | ((CONFIG_SERIAL_CONSOLE_TX_PORT & 0x01) << 5);
  UART_RX_PORT_CONFIG(UART_BASE_0) = CONFIG_SERIAL_CONSOLE_RX_PIN | ((CONFIG_SERIAL_CONSOLE_RX_PORT & 0x01) << 5);

  /* Enable UART0 */

  UART_ENABLE(UART_BASE_0) = 0x08;
//  sem_init(&g_uart_low_0_priv.uart_sema, 0, 1);

  UART_SHORTS_CONFIG(UART_BASE_0)     = (1 << 5);
  UART_RXD_PTR_CONFIG(UART_BASE_0)    = (uint32_t)g_uart_low_0.rx_buffer;
  UART_RX_MAXCNT_CONFIG(UART_BASE_0)  = 1;
  UART_TASK_START_RX_CFG(UART_BASE_0) = 1;

  return 0;
}

int uart_low_send(char *msg)
{
#if 0
  sem_wait(&g_uart_low_0_priv.sema);

  UART_EVENTS_TXSTOPPED(UART_BASE_0) = 0;
  UART_ENDTX(UART_BASE_0)            = 0;

  int i = 0;
  for (msg; *msg != 0; msg++)
  {
    g_uart_tx_buffer[i] = *msg;
    i++;
  }

  UART_TXD_PTR_CONFIG(UART_BASE_0)     = (uint32_t)g_uart_tx_buffer;
  UART_TXD_MAXCNT_CONFIG(UART_BASE_0)  = i;
  UART_TX_START_TASK(UART_BASE_0)      = 1;

  while (UART_ENDTX(UART_BASE_0) == 0);

  /* Stop the UART TX */
  UART_STOP_TX_TASK(UART_BASE_0) = 1;

  /* Wait until we receive the stopped event */
  while (UART_EVENTS_TXSTOPPED(UART_BASE_0) == 0);

  UART_TX_START_TASK(UART_BASE_0) = 0;
  sem_post(&g_uart_low_0_priv.sema);
#endif
  return 0;
}

int putchar(int c)
{
#if 0
  UART_EVENTS_TXSTOPPED(UART_BASE_0) = 0;
  UART_ENDTX(UART_BASE_0)            = 0;

  g_uart_tx_buffer[0] = (unsigned char)c;

  UART_TXD_PTR_CONFIG(UART_BASE_0)     = (uint32_t)g_uart_tx_buffer;
  UART_TXD_MAXCNT_CONFIG(UART_BASE_0)  = sizeof(unsigned char);
  UART_TX_START_TASK(UART_BASE_0)      = 1;

  while (UART_ENDTX(UART_BASE_0) == 0);

  /* Stop the UART TX */
  UART_STOP_TX_TASK(UART_BASE_0) = 1;

  /* Wait until we receive the stopped event */
  while (UART_EVENTS_TXSTOPPED(UART_BASE_0) == 0);

  UART_TX_START_TASK(UART_BASE_0) = 0;
#endif
}

char uart_low_receive(void)
{
  char c = 0;
  return c;
}

static void nrf52840_handle_rx_end(void)
{
  if (UART_RX_AMOUNT_CFG(UART_BASE_0) > 0)
  {
    g_uart_low_0.index_write_rx_buffer += UART_RX_AMOUNT_CFG(UART_BASE_0);
    g_uart_low_0.index_write_rx_buffer = g_uart_low_0.index_write_rx_buffer %
      UART_RX_BUFFER;
    sem_post(&g_uart_low_0.rx_notify);
   }

  UART_EVENTS_ENDRX_CFG(UART_BASE_0)  = 0;
  UART_TASK_START_RX_CFG(UART_BASE_0) = 1;
  UART_EVENTS_RXDRDY_CFG(UART_BASE_0) = 0;
}

static void nrf52840_handle_rx_rdy(void)
{
    UART_EVENTS_RXDRDY_CFG(UART_BASE_0) = 0;
}

static void nrf52840_handle_rx_started(void)
{
    UART_EVENTS_RXSTARTED_CFG(UART_BASE_0) = 0;

    UART_RXD_PTR_CONFIG(UART_BASE_0)    = (uint32_t)g_uart_low_0.rx_buffer;// + g_uart_low_0.index_write_rx_buffer;
    uint8_t available_rx_bytes = 0;

    if (g_uart_low_0.index_read_rx_buffer == g_uart_low_0.index_write_rx_buffer)
    {
//      UART_RX_MAXCNT_CONFIG  = 1;
    }
    else
    {
      uint8_t available_rx_bytes = 0;
      if (g_uart_low_0.index_read_rx_buffer > g_uart_low_0.index_write_rx_buffer)
      {
        available_rx_bytes = g_uart_low_0.index_read_rx_buffer - g_uart_low_0.index_write_rx_buffer - 1;
      }
      else
      {
        available_rx_bytes = UART_RX_BUFFER - (g_uart_low_0.index_write_rx_buffer - g_uart_low_0.index_read_rx_buffer) - 1;
      }
    }

  //  UART_RX_MAXCNT_CONFIG = available_rx_bytes > 1 ? 1 : available_rx_bytes;
}

static void nrf52840_lpuart_int(void)
{
  /* EVENTS_RXDRDY is received every time - also EVENTS_TXDRDY */

  if (UART_EVENTS_RXSTARTED_CFG(UART_BASE_0) == 1)
  {
    nrf52840_handle_rx_started();
  }
  else if (UART_EVENTS_ENDRX_CFG(UART_BASE_0) == 1)
  {
    nrf52840_handle_rx_end();
  }
  else if (UART_EVENTS_RXDRDY_CFG(UART_BASE_0) == 1)
  {
    nrf52840_handle_rx_rdy();
  }
}

static int nrf52840_lpuart_open(const struct uart_lower_s *lower)
{
  struct nrf52840_uart_priv_s *uart_priv = lower->priv; 

  /* Initialize the semaphore */

  sem_init(&((struct uart_lower_s *)lower)->rx_notify, 0, 0);
  sem_init(&uart_priv->uart_sema, 0, 1);

  /* Attach the uart interrupt */

  UART_EVENTS_RXDRDY_CFG(uart_priv->base_peripheral_ptr)    = 0;
  UART_EVENTS_ENDRX_CFG(uart_priv->base_peripheral_ptr)     = 0;
  UART_EVENTS_RXSTARTED_CFG(uart_priv->base_peripheral_ptr) = 0;

  disable_int();

  attach_int(uart_priv->irq, nrf52840_lpuart_int);
  NVIC_EnableIRQ(uart_priv->irq);
  NVIC_SetPriority(uart_priv->irq, 0x07);

  enable_int();

  return 0;
}

static int nrf52840_lpuart_write(const struct uart_lower_s *lower,
                                 const void *ptr_data,
                                 unsigned int sz)
{
  struct nrf52840_uart_priv_s *uart_priv = lower->priv; 

  sem_wait(&uart_priv->uart_sema);

  UART_EVENTS_TXSTOPPED(uart_priv->base_peripheral_ptr) = 0;
  UART_ENDTX(uart_priv->base_peripheral_ptr)            = 0;

  memcpy(uart_priv->uart_tx_buffer, ptr_data, sz);

  UART_TXD_PTR_CONFIG(uart_priv->base_peripheral_ptr) = (uint32_t)uart_priv->uart_tx_buffer;
  UART_TXD_MAXCNT_CONFIG(uart_priv->base_peripheral_ptr)  = sz;
  UART_TX_START_TASK(uart_priv->base_peripheral_ptr)      = 1;

  while (UART_ENDTX(uart_priv->base_peripheral_ptr) == 0);

  /* Stop the UART TX */
  UART_STOP_TX_TASK(uart_priv->base_peripheral_ptr) = 1;

  /* Wait until we receive the stopped event */
  while (UART_EVENTS_TXSTOPPED(uart_priv->base_peripheral_ptr) == 0);

  UART_TX_START_TASK(uart_priv->base_peripheral_ptr) = 0;
  sem_post(&uart_priv->uart_sema);

  return 0;
}

int uart_init(void)
{
  return uart_register("/dev/ttyUSB0", &g_uart_low_0);
}
