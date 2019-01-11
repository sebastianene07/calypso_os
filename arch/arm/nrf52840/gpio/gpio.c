#include <stdint.h>

#include <gpio.h>

#define GPIO_BASE   0x50000000
#define GPIO_OUTSET (*((uint32_t*) (GPIO_BASE + 0x508)))
#define GPIO_OUTCLR (*((uint32_t*) (GPIO_BASE + 0x50C)))
#define GPIO_PIN_CNF(pin) (*((uint32_t*) (GPIO_BASE + 0x700 + pin * 4)))


void gpio_init(void)
{

}

void gpio_configure(int pin)
{
  GPIO_PIN_CNF(pin) = 1;
}

void gpio_toogle(int enable, int pin)
{
  if (enable)
  {
    GPIO_OUTSET = (1 << pin);
  }
  else
  {
    GPIO_OUTCLR = (1 << pin);
  }
}
