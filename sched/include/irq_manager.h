#ifndef __IRQ_MANAGER_H
#define __IRQ_MANAGER_H

#include <board.h>

/****************************************************************************
 * Public Typess
 ****************************************************************************/

/* The interrupt handler definition */

typedef void (* irq_cb)(void);

/****************************************************************************
 * Public Methods
 ****************************************************************************/

void irq_attach(int irq_num, irq_cb handler);

void irq_detach(int irq_num);

void irq_generic_handler(void);

#endif /* __IRQ_MANAGER_H */
