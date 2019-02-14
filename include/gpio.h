#ifndef __GPIO_H
#define __GPIO_H

typedef enum gpio_direction_e {
  GPIO_IN,
  GPIO_OUT
} gpio_direction_t;

void gpio_init(void);

void gpio_configure(int pin, gpio_direction_t cfg);

void gpio_toogle(int enable, int pin);

int gpio_read(int pin);

#endif /* __GPIO_H */
