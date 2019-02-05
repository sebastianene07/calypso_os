#ifndef __TIMER_H
#define __TIMER_H

#define TIMER_0_BASE_ADDRESS    (0x40008000)

/* Registers offsets */

#define TIMER_BITMODE         (0x508)
#define TIMER_INT_SET         (0x304)
#define TIMER_INT_CLR         (0x308)
#define TIMER_PRESCALER       (0x510)
#define TIMER_COMPARE_COUNTER (0x540)
#define TIMER_SHORTS          (0x200)
#define TIMER_TASK_START      (0x000)
#define TIMER_TASK_STOP       (0x004)

#define TIMER_FUNCTION_REGISTER(REG_OFFSET)    (*(uint32_t *)((TIMER_0_BASE_ADDRESS) \
                                        + (REG_OFFSET)))                      \

int timer_init(void);


#endif /* __TIMER_H */
