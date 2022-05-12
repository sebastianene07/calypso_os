#ifndef __ARCH_ARM_RPIPICO_BSP_H
#define __ARCH_ARM_RPIPICO_BSP_H

#define REG_R0                (10)
#define REG_R1                (9)

#define REG_SP                (13)
#define REG_LR                (14)
#define REG_PC                (15)
#define REG_XPSR              (16)

#define REG_NUMS              (17)

#include <ARMCM0plus.h>

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
