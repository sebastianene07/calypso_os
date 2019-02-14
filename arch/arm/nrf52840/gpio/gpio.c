#include <stdint.h>

#include <gpio.h>

#define GPIO_BASE   0x50000000

#define GPIO_OUTSET (*((uint32_t*) (GPIO_BASE + 0x508)))
#define GPIO_OUTCLR (*((uint32_t*) (GPIO_BASE + 0x50C)))
#define GPIO_IN     (*((uint32_t*) (GPIO_BASE + 0x510)))
#define GPIO_DIR    (*((uint32_t*) (GPIO_BASE + 0x514)))
#define GPIO_PIN_CNF(pin) (*((uint32_t*) (GPIO_BASE + 0x700 + pin * 4)))

#define GPIO_PULLUP_VALUE   (3)
#define GPIO_PULLUP_OFFSET  (2)

void gpio_init(void)
{

}

void gpio_configure(int pin, gpio_direction_t cfg)
{
  if (cfg == GPIO_DIRECTION_IN)
  {
    GPIO_PIN_CNF(pin) = GPIO_DIRECTION_IN | (GPIO_PULLUP_VALUE << GPIO_PULLUP_OFFSET);
  }
  else
  {
    GPIO_PIN_CNF(pin) = GPIO_DIRECTION_OUT;
  }
}

int gpio_read(int pin)
{
  return ((GPIO_IN & (1 << pin)) >> pin);
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
