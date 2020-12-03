#ifndef __NRF5X_BOARD_H
#define __NRF5X_BOARD_H

#include <stdint.h>
#include <board_cfg.h>
#include <nrf52840.h>

#include <irq_manager.h>
#include <scheduler.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define RAM_BASE              (void *)(CONFIG_RAM_BASE)
#define RAM_LENGTH            (CONFIG_RAM_LENGTH)

#define STACK_TOP             (void *)(CONFIG_RAM_BASE + CONFIG_RAM_LENGTH)

#define HEAP_BLOCK_SIZE       (16)

/****************************************************************************
 * Peripheral initialization function for the board
 ****************************************************************************/

void board_init(void);

void board_entersleep(void);

/****************************************************************************
 * Task management functions
 ****************************************************************************/

int cpu_inittask(tcb_t *tcb, int argc, char **argv);

void cpu_destroytask(tcb_t *tcb);

/****************************************************************************
 * CPU context management functions
 ****************************************************************************/

int cpu_savecontext(void *mcu_context);

void cpu_restorecontext(void *mcu_context);

/****************************************************************************
 * CPU interrupt management functions
 ****************************************************************************/

void cpu_enableint(irq_state_t irq_state);

irq_state_t cpu_disableint(void);

int cpu_getirqnum(void);

void cpu_attachint(int irq_num, irq_cb handler);

int cpu_detachint(int irq_num);

#endif /* __NRF5X_BOARD_H */
