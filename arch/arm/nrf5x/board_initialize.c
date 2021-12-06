#include <board.h>

#include <assert.h>
#include <errno.h>
#include <gpio.h>

#include <spi.h>
#include <serial.h>
#include <rtc.h>
#include <timer.h>
#include <scheduler.h>


#ifdef CONFIG_DISPLAY_SSD1331
#include <display/ssd_1331.h>
#endif

#ifdef CONFIG_SPI_SDCARD
#include <storage/spi_sdcard.h>
#endif

#ifdef CONFIG_SENSOR_DRIVERS
#include <sensors/sensors.h>
#endif

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* The base clock address */

#define CLOCK_BASE            (0x40000000)

/* Configuration offsets for registers */

#define LFCLKSRC_OFFSET       (0x518)
#define LFCLKSTART_OFFSET     (0x008)
#define EVENTS_LFCLKSTARTED   (0x104)

/* Config registers */

#define CLOCK_CONFIG(offset_r)                                              \
  ((*((volatile uint32_t *)(CLOCK_BASE + (offset_r)))))

#define LFCLKSRC_CFG              CLOCK_CONFIG(LFCLKSRC_OFFSET)
#define LFCLKSTART_CFG            CLOCK_CONFIG(LFCLKSTART_OFFSET)
#define EVENTS_LFCLKSTARTED_CFG   CLOCK_CONFIG(EVENTS_LFCLKSTARTED)

/* The MCU context registers and the indexes used in save/restore context */

#define REG_R0                (12)
#define REG_R1                (13)

#define REG_SP                (1)
#define REG_LR                (11)
#define REG_PC                (0)
#define REG_XPSR              (16)

#define REG_NUMS              (17)

typedef struct cpu_stacking_s
{
  void *r0;
  void *r1;
  void *r2;
  void *r3;
  void *r12;
  void *lr;
  void *pc;
  void *xpsr;
} __attribute__((packed)) cpu_stacking_s;

/****************************************************************************
 * Private Data
 ****************************************************************************/

static unsigned int LED = 13;
static unsigned int BUTTON_1 = 11;

/* System Core Clock Frequency */
static uint32_t g_system_core_clock_freq = CONFIG_SYSTEM_CLOCK_FREQUENCY * 1000000;

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/*
 * clock_init - initialize the system low freq clock
 *
 * Initialize the LF clock.
 */
static void clock_init(void)
{
  /* Select internal crystal osc source, no bypass */

  LFCLKSRC_CFG = 0x01;

  /* Start the low frequency clock task */

  LFCLKSTART_CFG = 0x01;

  /* Wait for the started event */

  while (EVENTS_LFCLKSTARTED == 0x01);
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/*
 * board_init - initialize the board resources
 *
 * Initialize the board specific device drivers and prepare the board.
 */
void board_init(void)
{
  /* Driver initialization logic */
#ifdef CONFIG_NRF5X_CLOCK
  clock_init();
#endif

#ifdef CONFIG_NRF5X_RTC
  rtc_init();
#endif

#ifdef CONFIG_NRF5X_TIMER
  timer_init();
#endif

  struct spi_master_config_s spi[] = {
#ifdef CONFIG_SPI_0
    {
      .miso_pin  = CONFIG_SPI_0_MISO_PIN,
      .miso_port = CONFIG_SPI_0_MISO_PORT,

      .mosi_pin  = CONFIG_SPI_0_MOSI_PIN,
      .mosi_port = CONFIG_SPI_0_MOSI_PORT,

      .sck_pin   = CONFIG_SPI_0_SCK_PIN,
      .sck_port  = CONFIG_SPI_0_SCK_PORT,

      .cs_pin    = CONFIG_SPI_0_CS_PIN,
      .cs_port   = CONFIG_SPI_0_CS_PORT,

      .freq      = CONFIG_SPI_0_FREQUENCY,
      .mode      = SPI_M_MODE_0,
    },
#endif

#ifdef CONFIG_SPI_1
    {
      .miso_pin  = CONFIG_SPI_1_MISO_PIN,
      .miso_port = CONFIG_SPI_1_MISO_PORT,

      .mosi_pin  = CONFIG_SPI_1_MOSI_PIN,
      .mosi_port = CONFIG_SPI_1_MOSI_PORT,

      .sck_pin   = CONFIG_SPI_1_SCK_PIN,
      .sck_port  = CONFIG_SPI_1_SCK_PORT,

      .cs_pin    = CONFIG_SPI_1_CS_PIN,
      .cs_port   = CONFIG_SPI_1_CS_PORT,

      .freq      = CONFIG_SPI_1_FREQUENCY,
      .mode      = SPI_M_MODE_0,
    },
#endif
  };

#if defined(CONFIG_SPI_0) || defined(CONFIG_SPI_1)
  spi_master_dev_t *spi_devs = spi_init(spi, ARRAY_LEN(spi));
#endif

  uart_low_init();
  printf("\r\n.");

  gpio_init();
  printf(".");

  gpio_configure(LED, 0, GPIO_DIRECTION_OUT, GPIO_PIN_INPUT_DISCONNECT,
                 GPIO_NO_PULL, GPIO_PIN_S0S1, GPIO_PIN_NO_SENS);
  printf(".\r\n");

  size_t num_uart = 0;
  struct uart_lower_s *uart_peripheral = uart_init(&num_uart);

#ifdef CONFIG_DISPLAY_SSD1331
  ssd1331_config_t display_config = {
    .spi_dev = &spi_devs[CONFIG_DISPLAY_DRIVER_SSD1331_SPI_ID],
    .dc_pin  = CONFIG_DISPLAY_DC_PIN,
    .dc_port = CONFIG_DISPLAY_DC_PORT,
    .cs_pin  = spi_devs[0].dev_cfg.cs_pin,
    .cs_port = spi_devs[0].dev_cfg.cs_port,
  };

  ssd1331_display_init(&display_config);
#endif

#ifdef CONFIG_SPI_SDCARD
  gpio_configure(CONFIG_SPI_SDCARD_VSYS_PIN, CONFIG_SPI_SDCARD_VSYS_PORT,
    GPIO_DIRECTION_OUT, GPIO_PIN_INPUT_DISCONNECT,
    GPIO_NO_PULL, GPIO_PIN_S0S1, GPIO_PIN_NO_SENS);

  gpio_toogle(0, CONFIG_SPI_SDCARD_VSYS_PIN, CONFIG_SPI_SDCARD_VSYS_PORT);
  gpio_toogle(1, CONFIG_SPI_SDCARD_VSYS_PIN, CONFIG_SPI_SDCARD_VSYS_PORT);
  sd_spi_init(&spi_devs[CONFIG_SPI_SDCARD_SPI_ID]);
#endif

#ifdef CONFIG_SENSOR_BME680
  bme680_sensor_register(CONFIG_SENSOR_BME680_PATH_NAME,
      &spi_devs[CONFIG_SENSOR_BME680_SPI_ID]);
#endif

#ifdef CONFIG_SENSOR_PMSA003
  assert(CONFIG_SENSOR_PSMA003_UART_ID < num_uart);
  pmsa_sensor_register(CONFIG_SENSOR_PMSA003_PATH_NAME,
      &uart_peripheral[CONFIG_SENSOR_PSMA003_UART_ID]);
#endif

  /* Enable system tick */

//  SysTick_Config(g_system_core_clock_freq / CONFIG_SYSTEM_SCHEDULER_SLICE_FREQUENCY);
}

/*
 * board_entersleep - Place the board in sleep. It wakes up on interrupt.
 *
 */
void board_entersleep(void)
{
  __WFI();
}

/*
 * cpu_inittask - creates the initial state for a task
 *
 */
int cpu_inittask(tcb_t *task_tcb, int argc, char **argv)
{
  void **mcu_context = calloc(REG_NUMS, sizeof(void *));
  if (mcu_context == NULL)
  {
    return -ENOMEM;
  }

  void *bottom_sp = (unsigned int)task_tcb->stack_ptr_top - 8 * sizeof(void *);

  /* Initial MCU context */

  mcu_context[REG_R0]   = (void *)argc;
  mcu_context[REG_R1]   = (void *)argv;
  mcu_context[REG_LR]   = (void *)sched_default_task_exit_point;
  mcu_context[REG_PC]   = (void *)task_tcb->entry_point;
  mcu_context[REG_XPSR] = (void *)0x1000000;
  mcu_context[REG_SP]   = bottom_sp;

 /* Setup the initial stack context  */

  task_tcb->mcu_context = mcu_context;
  return 0;
}

/*
 * cpu_destroytask - creates the initial state for a task
 *
 */
void cpu_destroytask(tcb_t *tcb)
{
  free(tcb->mcu_context);
  free(tcb);
}

/*
 * cpu_disableint - Disable all the interrupts and return the primask register.
 *
 */
irq_state_t cpu_disableint(void)
{
  unsigned short primask;

  __asm volatile("mrs %0, primask\n"
                 "cpsid i\n"
                 : "=r" (primask)
                 :
                 : "memory");
  return (irq_state_t)primask;
}

/*
 * cpu_enableint - Enable the interrupts if the first bit of the irq_state is 1.
 *
 */
void cpu_enableint(irq_state_t irq_state)
{
  __asm volatile("tst %0, #1\n"
                 "bne.n 1f\n"
                 "cpsie i\n"
                 "1:\n"
                 :
                 : "r" (irq_state)
                 : "memory");
}

/*
 * cpu_getirqnum - Return the interrupt number.
 *
 * Assumptions: This should be called only from ISR handler.
 *
 */
int cpu_getirqnum(void)
{
  int ipsr;

  __asm volatile("mrs %0, ipsr\n"
                 : "=r" (ipsr)
                 :
                 : "memory");

  ipsr -= 16;
  return ipsr;
}
