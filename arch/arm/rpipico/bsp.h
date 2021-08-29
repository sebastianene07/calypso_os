#ifndef __ARCH_ARM_RPIPICO_BSP_H
#define __ARCH_ARM_RPIPICO_BSP_H

/****************************************************************************
 * UART bsp functions
 ****************************************************************************/

void bsp_serial_console_init(void);

void bsp_serial_console_putc(int c);

/****************************************************************************
 * GPIO bsp functions
 ****************************************************************************/

void bsp_gpio_led_init();

#endif /* __ARCH_ARM_RPIPICO_BSP_H */
