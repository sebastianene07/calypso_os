#ifndef __GPIO_H
#define __GPIO_H

typedef enum gpio_direction_e {
  GPIO_DIRECTION_IN,
  GPIO_DIRECTION_OUT
} gpio_direction_t;

typedef enum gpio_pull_cfg_e {
  GPIO_NO_PULL,
  GPIO_PULLDOWN,
  GPIO_PULLUP,
} gpio_pull_cfg_t;

typedef enum gpio_drive_cfg_e {
  GPIO_PIN_S0S1 = 0,
} gpio_drive_cfg_t;

typedef enum
{
  GPIO_PIN_INPUT_CONNECT,
  GPIO_PIN_INPUT_DISCONNECT,
} gpio_pin_input_t;

typedef enum
{
  GPIO_PIN_NO_SENS= 0,
  GPIO_PIN_SENSE_LOW = 3,
  GPIO_PIN_SENSE_HIGH = 2
} gpio_pin_sense_t;

void gpio_init(void);

void gpio_configure(int pin, int port, gpio_direction_t cfg, gpio_pin_input_t input,
                    gpio_pull_cfg_t pull_cfg, gpio_drive_cfg_t drive_cfg, gpio_pin_sense_t sense_input);

void gpio_toogle(int enable, int pin, int port);

int gpio_read(int pin, int port);

#endif /* __GPIO_H */
