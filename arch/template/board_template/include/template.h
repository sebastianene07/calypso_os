/* Template file - add license here
 *
 * This header file contains board specific definitions like:
 * - interrupt numbers, priorities, sources
 */

#ifndef __TEMPLATE_H
#define __TEMPLATE_H

/* ! NUM_IRQS has to be defined ! */
typedef enum {
  DEFAULT = 0,
  NUM_IRQS = 255
} IRQn_Type;

typedef uint32_t irq_state_t;

#endif /* __TEMPLATE_H */
