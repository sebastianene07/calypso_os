#ifndef __GPIO_H
#define __GPIO_H

void gpio_init(void);

void gpio_configure(int pin);

void gpio_toogle(int enable, int pin);

#endif /* __GPIO_H */
