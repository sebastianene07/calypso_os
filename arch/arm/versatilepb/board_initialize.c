#include <board.h>

#include <serial.h>
#include <stdint.h>
#include <scheduler.h>
#include <os_start.h>

/* Ram based ISR vector */
void (*g_ram_vectors[NUM_IRQS])(void);

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

void dummy_fn(void);

void Pend_SV_Handler(void);
void SysTick_Handler(void);
static void generic_isr_handler(void);

/* The fault handler implementation calls a function called
prvGetRegistersFromStack(). */
static void HardFault_Handler(void)
{
}

void Pend_SV_Handler(void)
{
}

void SysTick_Handler(void)
{
}

//* Flash based ISR vector */
__attribute__((section(".isr_vector")))
void (*g_vectors[NUM_IRQS])(void) = {
        STACK_TOP,
        _start,
        dummy_fn,
        HardFault_Handler,
        dummy_fn,
        dummy_fn,
        dummy_fn,
        dummy_fn,
        dummy_fn,
        dummy_fn,
        dummy_fn,
        dummy_fn,
        dummy_fn,
        dummy_fn,
        Pend_SV_Handler,
        SysTick_Handler,

        /* External interrupts */

        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
        generic_isr_handler,
};

static void generic_isr_handler(void)
{
  /* get the exception number */
}

void dummy_fn(void)
{
        while(1)
        {

        }
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/*
 * board_init - initialize the board resources
 *
 * Initialize the board specific device drivers and prepare the board.
 */
void board_init(void)
{
  uart_low_init();
  printf("\r\nQEMU versatilepb initializing\r\n.");
}

/*
 * up_initial_task_context - creates the initial state for a task
 *
 */
int up_initial_task_context(struct tcb_s *tcb)
{
  /* Initial MCU context */

  task_tcb->mcu_context[0] = (void *)argc;
  task_tcb->mcu_context[1] = (void *)argv;
  task_tcb->mcu_context[5] = (uint32_t *)sched_default_task_exit_point;
  task_tcb->mcu_context[6] = task_entry_point;
  task_tcb->mcu_context[7] = (uint32_t *)0x1000000;

  /* Stack context in interrupt */
  const int unstacked_regs = 8;   /* R4-R11 */
  int i = 0;
  void *ptr_after_int = task_tcb->stack_ptr_top -
    sizeof(void *) * MCU_CONTEXT_SIZE;

  for (uint8_t *ptr = ptr_after_int;
     ptr < (uint8_t *)task_tcb->stack_ptr_top;
     ptr += sizeof(uint32_t))
  {
    *((uint32_t *)ptr) = (uint32_t)task_tcb->mcu_context[i++];
  }

  task_tcb->sp = ptr_after_int - unstacked_regs * sizeof(void *);

  return 0;
}
