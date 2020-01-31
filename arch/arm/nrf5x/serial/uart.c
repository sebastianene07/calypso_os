#include <board.h>

#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <semaphore.h>
#include <serial.h>
#include <stdlib.h>
#include <string.h>
#include <scheduler.h>
#include <gpio.h>
#include <stdbool.h>

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
#define UART_TXD_DMA_OFFSET                 (0x544)
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

#define UART_TXD_OFFSET                     (0x51C)
#define UART_RXD_OFFSET                     (0x518)
#define UART_TX_READY_OFFSET                (0x11C)

/* UART configuration fields */

#define UART_CONFIG(base_peripheral, offset_r)  ((*((volatile uint32_t *)((base_peripheral) + (offset_r)))))

#define UART_ENABLE(base_peripheral)            UART_CONFIG((base_peripheral) , UART_ENABLE_OFFSET)
#define UART_CONFIG_REG(base_peripheral)        UART_CONFIG((base_peripheral) , UART_CONFIG_OFFSET)
#define UART_BAUDRATE(base_peripheral)          UART_CONFIG((base_peripheral) , UART_BAUDRATE_OFFSET)
#define UART_TX_PORT_CONFIG(base_peripheral)    UART_CONFIG((base_peripheral) , UART_PIN_SEL_TXD_OFFSET)
#define UART_RX_PORT_CONFIG(base_peripheral)    UART_CONFIG((base_peripheral) , UART_PIN_SEL_RXD_OFFSET)
#define UART_CTS_PORT_CONFIG(base_peripheral)    UART_CONFIG((base_peripheral) , UART_PIN_SEL_CTS_OFFSET)
#define UART_RTS_PORT_CONFIG(base_peripheral)    UART_CONFIG((base_peripheral) , UART_PIN_SEL_RTS_OFSSET)

#define UART_TXD_PTR_CONFIG(base_peripheral)    UART_CONFIG((base_peripheral) , UART_TXD_DMA_OFFSET)
#define UART_TXD_MAXCNT_CONFIG(base_peripheral) UART_CONFIG((base_peripheral) , UART_TXD_MAXCNT)
#define UART_TX_START_TASK(base_peripheral)     UART_CONFIG((base_peripheral) , UART_TASK_START_TX_OFFSET)
#define UART_RX_START_TASK(base_peripheral)     UART_CONFIG((base_peripheral) , UART_TASK_START_RX_OFFSET)
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

#define UART_DMA_RX_LEN                         (UART_RX_BUFFER / 8)

#define UART_RXD_CONFIG(base_peripheral)        UART_CONFIG((base_peripheral) , UART_RXD_OFFSET)
#define UART_TXD_CONFIG(base_peripheral)        UART_CONFIG((base_peripheral) , UART_TXD_OFFSET)
#define UART_EVENT_TXRDY(base_peripheral)       UART_CONFIG((base_peripheral), UART_TX_READY_OFFSET)

/****************************************************************************
 * Private Types
 ****************************************************************************/

struct nrf52840_uart_priv_s {
  void *base_peripheral_ptr;
  IRQn_Type irq;

  uint8_t rx_pin;
  uint8_t rx_port;
  uint8_t tx_pin;
  uint8_t tx_port;
  uint8_t cts_pin;
  uint8_t cts_port;
  uint8_t rts_pin;
  uint8_t rts_port;

  bool is_initialized;
  bool is_flow_control;
  bool is_dma_control;
  bool is_parity_included;
  bool is_one_stopbit;

  bool is_byte_received_event;
  bool is_error_event;
  bool is_timeout_event;

  uint32_t baud_rate;
};

/****************************************************************************
 * Private Functions Definition
 ****************************************************************************/

static int nrf52840_lpuart_open(const struct uart_lower_s *lower);
static int nrf52840_lpuart_write(const struct uart_lower_s *lower,
                                 const void *ptr_data,
                                 unsigned int sz);
static int nrf52840_lpuart_config(struct nrf52840_uart_priv_s *config);

/****************************************************************************
 * Private Data
 ****************************************************************************/

/* Private data for the UART 0 peripheral */
static struct nrf52840_uart_priv_s g_uart_low_0_priv = 
{
  .base_peripheral_ptr = (void *)UART_BASE_0,
  .irq                 = UARTE0_UART0_IRQn,
  .is_initialized      = false,
  .is_flow_control     = false,
  .is_dma_control      = false,
  .is_parity_included  = false,
  .is_one_stopbit      = true,
  
  .is_byte_received_event = true,

  .baud_rate           = CONFIG_SERIAL_CONSOLE_BAUDRATE,
  .rx_pin              = CONFIG_SERIAL_CONSOLE_RX_PIN,
  .rx_port             = CONFIG_SERIAL_CONSOLE_RX_PORT,

  .tx_pin              = CONFIG_SERIAL_CONSOLE_TX_PIN,
  .tx_port             = CONFIG_SERIAL_CONSOLE_TX_PORT,
};

#ifdef CONFIG_UART_PERIPHERAL_1
static struct nrf52840_uart_priv_s g_uart_low_1_priv = 
{
  .base_peripheral_ptr = (void *)UART_BASE_1,
  .irq                 = UARTE1_IRQn,
  .is_initialized      = false,
  .is_flow_control     = false,
  .is_dma_control      = false,
  .is_parity_included  = false,
  .is_one_stopbit      = true,
  
  .is_byte_received_event = true,

  .baud_rate           = CONFIG_UART_PERIPHERAL_1_BAUDRATE,
  .rx_pin              = CONFIG_UART_PERIPHERAL_1_RX_PIN,
  .rx_port             = CONFIG_UART_PERIPHERAL_1_RX_PORT,

  .tx_pin              = CONFIG_UART_PERIPHERAL_1_TX_PIN,
  .tx_port             = CONFIG_UART_PERIPHERAL_1_TX_PORT,
};
#endif

/* Uart 0 lower half operations. There is no need to provide a read_cb function
 * because we notify the incmming data through rx_notify semaphore and we 
 * copy it in the rx_buffer from interrupt.
 */ 
static struct uart_lower_s g_uart_low_0 =
{
  .priv     = &g_uart_low_0_priv,  
  .open_cb  = nrf52840_lpuart_open,
  .write_cb = nrf52840_lpuart_write,
};

#ifdef CONFIG_UART_PERIPHERAL_1
static struct uart_lower_s g_uart_low_1 =
{
  .priv     = &g_uart_low_1_priv,  
  .open_cb  = nrf52840_lpuart_open,
  .write_cb = nrf52840_lpuart_write,
};
#endif

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static int nrf52840_lpuart_config(struct nrf52840_uart_priv_s *config)
{
  /* Configure UART0 - no hardware flow control, no pairty, one stop bit */

  uint8_t config_reg = 0;

  if (config->is_flow_control) {
    config_reg |= (1 << 0);
  } 

  if (config->is_parity_included) {
    config_reg |= (7 << 1);
  }

  if (!config->is_one_stopbit) {
    config_reg |= (1 << 4);
  }

  UART_ENABLE(config->base_peripheral_ptr) = 0x0;
  UART_CONFIG_REG(config->base_peripheral_ptr) = config_reg;

  /* Enable EndRX and RX started event */
  uint8_t event_reg = 0;

  if (config->is_flow_control) {
    event_reg |= (1 << 0);
  }

  if (config->is_byte_received_event) {
    event_reg |= (1 << 2);
  }

  if (config->is_error_event) {
    event_reg |= (1 << 9);
  }

  if (config->is_timeout_event) {
    event_reg |= (1 << 17);
  }

  /* Configure the event register */
  UART_INTENSET_CONFIG(config->base_peripheral_ptr) =  event_reg;

  /* Configure UART baud rate  */
  UART_BAUDRATE(config->base_peripheral_ptr) = config->baud_rate;
 
  /* Configure pins */

  gpio_configure(config->rx_pin,
                 config->rx_port,
                 GPIO_DIRECTION_IN,
                 GPIO_PIN_INPUT_CONNECT, GPIO_NO_PULL,
                 GPIO_PIN_S0S1, GPIO_PIN_SENSE_LOW);

  UART_TX_PORT_CONFIG(config->base_peripheral_ptr) = config->tx_pin | ((config->tx_port & 0x01) << 5);
  UART_RX_PORT_CONFIG(config->base_peripheral_ptr) = config->rx_pin | ((config->rx_port & 0x01) << 5);


  if (config->is_flow_control) {
    gpio_configure(config->cts_pin,
                   config->cts_port,
                   GPIO_DIRECTION_IN,
                   GPIO_PIN_INPUT_CONNECT, GPIO_NO_PULL,
                   GPIO_PIN_S0S1, GPIO_PIN_SENSE_LOW);

    UART_CTS_PORT_CONFIG(config->base_peripheral_ptr) = config->cts_pin | ((config->cts_port & 0x01) << 5);
    UART_RTS_PORT_CONFIG(config->base_peripheral_ptr) = config->rts_pin | ((config->rts_port & 0x01) << 5);
  }

  uint8_t enable = 0x4;
  if (config->is_dma_control) {
    enable = 0x08;
  }

  UART_ENABLE(config->base_peripheral_ptr) = enable;

  UART_TX_START_TASK(g_uart_low_0_priv.base_peripheral_ptr) = 1;
  UART_RX_START_TASK(g_uart_low_0_priv.base_peripheral_ptr) = 1;
  
  config->is_initialized = true;

  return 0;
}

static void nrf52840_lpuart_int(void)
{
  struct uart_lower_s *lower = NULL;

  int irq_number = up_get_irq_number();
  irq_number -= 16;

  if (irq_number == UARTE0_UART0_IRQn) {
    lower = &g_uart_low_0;
  }
#ifdef CONFIG_UART_PERIPHERAL_1
  else if (irq_number == UARTE1_IRQn) {
    lower = &g_uart_low_1;
  } 
#endif

  assert(lower != NULL);

  struct nrf52840_uart_priv_s *uart_priv = lower->priv; 
 
  if (UART_EVENTS_RXDRDY_CFG(uart_priv->base_peripheral_ptr)) {
    UART_EVENTS_RXDRDY_CFG(uart_priv->base_peripheral_ptr) = 0;

    lower->rx_buffer[lower->index_write_rx_buffer] =
      UART_RXD_CONFIG(uart_priv->base_peripheral_ptr);
    lower->index_write_rx_buffer = (lower->index_write_rx_buffer + 1) % UART_RX_BUFFER;

    sem_post(&lower->rx_notify);
  }
}

static int nrf52840_lpuart_open(const struct uart_lower_s *lower)
{
  struct nrf52840_uart_priv_s *uart_priv = lower->priv; 

  sem_wait(&lower->lock);

  if (!uart_priv->is_initialized) {
    nrf52840_lpuart_config(uart_priv);
  }

  if (uart_priv->is_dma_control) {
    sem_post(&lower->lock);
    return -EOPNOTSUPP;
  }

  disable_int();
  attach_int(uart_priv->irq, nrf52840_lpuart_int);
  NVIC_EnableIRQ(uart_priv->irq);
  NVIC_SetPriority(uart_priv->irq, 0x07);
  enable_int();

  sem_post(&lower->lock);

  return 0;
}

static int nrf52840_lpuart_write(const struct uart_lower_s *lower,
                                 const void *ptr_data,
                                 unsigned int sz)
{
  struct nrf52840_uart_priv_s *uart_priv = lower->priv; 

  sem_wait(&lower->lock);

  for (int i = 0; i < sz; i++) {

    UART_EVENT_TXRDY(uart_priv->base_peripheral_ptr) = 0;
    UART_TXD_CONFIG(uart_priv->base_peripheral_ptr)  = *((unsigned char *)(ptr_data + i));

    while (UART_EVENT_TXRDY(uart_priv->base_peripheral_ptr) == 0);
  }

  sem_post(&lower->lock);

  return 0;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int uart_low_init(void)
{
  sem_init(&g_uart_low_0.lock, 0, 1);
  nrf52840_lpuart_config(&g_uart_low_0_priv);
  return 0;
}

int putchar(int c)
{
  UART_EVENT_TXRDY(UART_BASE_0) = 0;
  UART_TXD_CONFIG(UART_BASE_0) = (unsigned char)c;

  while (UART_EVENT_TXRDY(UART_BASE_0) == 0);
  return 0;
}

int uart_init(void)
{
  int ret = 0;

  sem_init(&g_uart_low_0.rx_notify, 0, 0);
  sem_init(&g_uart_low_0.lock, 0, 1);

  ret = uart_register(CONFIG_CONSOLE_UART_PATH, &g_uart_low_0);
  if (ret != 0) {
    return ret;
  }

#ifdef CONFIG_UART_PERIPHERAL_1
  sem_init(&g_uart_low_1.rx_notify, 0, 0);
  sem_init(&g_uart_low_1.lock, 0, 1);

  ret = uart_register(CONFIG_UART_PERIPHERAL_1_PATH, &g_uart_low_1);
#endif

  return ret;
}

sem_t *get_console_sema(void)
{
  return &g_uart_low_0.lock;
}
