#include <board.h>
#include <serial.h>
#include <gpio.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define XTAL                  (50000000UL)     /* Oscillator frequency */
#define SYSTEM_CLOCK          (XTAL / 2U)

/****************************************************************************
 * Private Data
 ****************************************************************************/

static unsigned int LED = 13;
static unsigned int BUTTON_1 = 11;

static uint32_t SystemCoreClock = SYSTEM_CLOCK;  /* System Core Clock Frequency */

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

  uart_low_init();
  uart_low_send(".");

  gpio_init();
  uart_low_send(".");

  gpio_configure(LED, 0, GPIO_DIRECTION_OUT);
  gpio_configure(BUTTON_1, 0, GPIO_DIRECTION_IN);
  uart_low_send(".");

  uart_init();

  SysTick_Config(SystemCoreClock / 100);
}
