/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <board.h>

#include <assert.h>
#include <errno.h>
#ifdef CONFIG_SIMULATED_FLASH
  #include <storage/simulated_flash.h>
#endif
#include <scheduler.h>
#include <serial.h>
#include <signal.h>
#include <stdint.h>
#include <string.h>
#include <os_start.h>
#include <rtc.h>

/****************************************************************************
 * Pre-processor Defintions
 ****************************************************************************/

/* The stack alignment in bytes */

#define STACK_ALIGNMENT               (8)

/****************************************************************************
 * Private Types
 ****************************************************************************/

typedef struct sim_mcu_arguments_s
{
  int argv;
  char **argc;
} sim_mcu_arguments_t;

/****************************************************************************
 * Private Types
 ****************************************************************************/

/* Save the simulated interrupt number from the host signal handler */

static volatile int g_simulated_int_num;

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

/* This function starts to simulate systick events using a host timer */

void host_simulated_systick(void);

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static void systick_interrupt(void)
{
  sched_run();
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: board_init
 *
 * Description:
 *   Board initialization function, here we do the peripheral initializations
 *   and we setup the host timer to simulate tick events.
 *
 ****************************************************************************/

void board_init(void)
{
  /* Lower level UART should be available to print individual characters to 
   * the sys console.
   */

  printf("\r\n[board_init] Simulation init\r\n");

  /* Initialize the UART simulated driver */

  size_t num_uart = 0;
  uart_init(&num_uart);

  /* Initialize the RTC simulated driver */

  rtc_init();

#ifdef CONFIG_SIMULATED_FLASH
  /* Initialize the simulated flash */

  sim_flash_init();
#endif

  /* Start the SysTick simulation using the host timer */

  irq_attach(SYSTICK_IRQ, systick_interrupt);
//  host_simulated_systick();
}

/****************************************************************************
 * Name: host_set_simualted_intnum
 *
 * Description:
 *   This sets the simulated interrupt number. On a real board we would
 *   look at the CPU registers to get the interrupt number but here we need
 *   this setter.
 *
 ****************************************************************************/

void host_set_simualted_intnum(int int_num)
{
  g_simulated_int_num = int_num;
}

void task_entry_point(void)
{
  tcb_t *current_task = sched_get_current_task();
  sim_mcu_arguments_t *args = current_task->mcu_context;

  current_task->entry_point(args->argv, args->argc);  

  sched_default_task_exit_point(); 
}

/****************************************************************************
 * Task management functions
 ****************************************************************************/

/****************************************************************************
 * Name: cpu_inittask
 *
 * Description:
 *   This function sets up the initial task context.
 *
 * Input Parameters:
 *   tcb  - the task control block
 *   argc - number of arguments for the entry point
 *   argv - arguments buffer for the entry point
 *
 * Return Value:
 *   On success returns 0 otherwise a negative error code.
 *
 ****************************************************************************/

int cpu_inittask(struct tcb_s *tcb, int argv, char **argc)
{
  /* Make sure the address is aligned to 8 */

  tcb->sp = (void *)((unsigned long)tcb->stack_ptr_top & ~(7));

  /* Let's create a frame on the stack in the similar way cpu_savecontext
   * will do.
   */

  void *mcu_context[7] = {0};

  mcu_context[0] = task_entry_point;
  mcu_context[5] = tcb->sp;
  tcb->sp = tcb->sp - sizeof(mcu_context);

  memcpy(tcb->sp, mcu_context, sizeof(mcu_context));

  sim_mcu_arguments_t *args = calloc(1, sizeof(sim_mcu_arguments_t));

  /* Copy the arguments */

  args->argv = argv;
  args->argc = calloc(argv + 1, sizeof(char *));

  for (int i = 0; i < argv; i++)
  {
    args->argc[i] = calloc(strlen(argc[i]) + 1, sizeof(char));
    memcpy(args->argc[i], argc[i], strlen(argc[i]));
  }

  tcb->mcu_context = args;

  return 0;
}

/****************************************************************************
 * Name: cpu_destroytask
 *
 * Description:
 *   This function destroys the task context and its associated resources.
 *   It is called from the Idle task when the Idle task detects that we
 *   have pending tasks that need to be destroyed.
 *
 * Input Parameters:
 *   tcb  - the task control block
 *
 * Return Value:
 *   On success returns 0 otherwise a negative error code.
 *
 ****************************************************************************/

void cpu_destroytask(struct tcb_s *tcb)
{
  sim_mcu_arguments_t *args = tcb->mcu_context;

  /* Release the memory allocated for the arguments */

  for (int i = 0; i < args->argv; i++)
  {
    free(args->argc[i]);
  }

  free(args->argc);
  free(args);
  free(tcb);
}

/****************************************************************************
 * CPU interrupt management functions
 ****************************************************************************/

int cpu_getirqnum(void)
{
  return g_simulated_int_num;
}
