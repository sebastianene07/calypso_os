#include <stdint.h>

#include <gpio.h>

#define GPIO_BASE_PORT0   0x50000000
#define GPIO_BASE_PORT1   0x50000300

#define GPIO_OUTSET(BASE) (*((uint32_t*) ((BASE) + 0x508)))
#define GPIO_OUTCLR(BASE) (*((uint32_t*) ((BASE) + 0x50C)))
#define GPIO_IN(BASE)     (*((uint32_t*) ((BASE) + 0x510)))
#define GPIO_DIR(BASE)    (*((uint32_t*) ((BASE) + 0x514)))
#define GPIO_PIN_CNF(BASE, pin) (*((uint32_t*) ((BASE) + 0x700 + (pin) * 4)))

#define GPIO_PULLUP_VALUE   (3)
#define GPIO_PULLUP_OFFSET  (2)

static uint8_t *g_base_addr[] =
{
  (uint8_t *)GPIO_BASE_PORT0,
  (uint8_t *)GPIO_BASE_PORT1,
};

void gpio_init(void)
{

}

void gpio_configure(int pin, int port, gpio_direction_t cfg)
{
  if (port < 0 || port >= 2)
  {
    return;
  }

  if (cfg == GPIO_DIRECTION_IN)
  {
    GPIO_PIN_CNF(g_base_addr[port], pin) = GPIO_DIRECTION_IN | (GPIO_PULLUP_VALUE << GPIO_PULLUP_OFFSET);
  }
  else
  {
    GPIO_PIN_CNF(g_base_addr[port], pin) = GPIO_DIRECTION_OUT;
  }
}

int gpio_read(int pin, int port)
{
  if (port < 0 || port >= 2)
  {
    return -1;
  }

  return ((GPIO_IN(g_base_addr[port]) & (1 << pin)) >> pin);
}

void gpio_toogle(int enable, int pin, int port)
{
  if (port < 0 || port >= 2)
  {
    return;
  }

  if (enable)
  {
    GPIO_OUTSET(g_base_addr[port]) = (1 << pin);
  }
  else
  {
    GPIO_OUTCLR(g_base_addr[port]) = (1 << pin);
  }
}
