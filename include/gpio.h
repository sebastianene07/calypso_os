#ifndef __GPIO_H
#define __GPIO_H

typedef enum gpio_direction_e {
  GPIO_DIRECTION_IN,
  GPIO_DIRECTION_OUT
} gpio_direction_t;

void gpio_init(void);

void gpio_configure(int pin, int port, gpio_direction_t cfg);

void gpio_toogle(int enable, int pin, int port);

int gpio_read(int pin, int port);

#endif /* __GPIO_H */
