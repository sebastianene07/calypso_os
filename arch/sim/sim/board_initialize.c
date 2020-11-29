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

/* We save the following registers indexed from 0 in this order:
 * RBX  ..... 0
 * RBP  ..... 1 (the frame pointer)
 * R12  ..... 2
 * R13  ..... 3
 * R14  ..... 4
 * R15  ..... 5
 * PC   ..... 6 (return address)
 * RSP  ..... 7 (the stack pointer)
 */

#define REG_BP                        (1)
#define REG_PC                        (6)
#define REG_SP                        (7)

#define NUM_REGS                      (8)

/* The context size contains the registers and a pointer to the 
 * sim_mcu_arguments_t where we keep the arguments.
 */

#define CONTEXT_SIZE                  (NUM_REGS + 1)

/* The sim_mcu_arguments_t index in the mcu_context */

#define CONTEXT_ARGS                  (NUM_REGS)

/* The stack alignment in bytes */

#define STACK_ALIGNMENT               (64)
#define ALIGN_STACK_DOWN(addr)((void *)((unsigned long)(addr) & ~(STACK_ALIGNMENT - 1)))

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

/****************************************************************************
 * Name: task_entry_point
 *
 * Description:
 *   The task entry point sets up the arguments..
 *
 ****************************************************************************/

void task_entry_point(void)
{
  tcb_t *current_task = sched_get_current_task();
  void **mcu_context = (void **)current_task->mcu_context;
  sim_mcu_arguments_t *args = mcu_context[CONTEXT_ARGS];

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
  /* Let's create a frame on the stack in the similar way cpu_savecontext
   * will do.
   */

  void **mcu_context = calloc(CONTEXT_SIZE, sizeof(void *));

  /* Make sure the address is aligned to 8 */

  mcu_context[REG_PC] = task_entry_point;
  mcu_context[REG_BP] = ALIGN_STACK_DOWN(tcb->stack_ptr_top);
  mcu_context[REG_SP] = ALIGN_STACK_DOWN(tcb->stack_ptr_top);

  /* Allocate memory to keep the arguments */

  sim_mcu_arguments_t *args = calloc(1, sizeof(sim_mcu_arguments_t));

  /* Copy the arguments */

  args->argv = argv;
  args->argc = calloc(argv + 1, sizeof(char *));

  for (int i = 0; i < argv; i++)
  {
    args->argc[i] = calloc(strlen(argc[i]) + 1, sizeof(char));
    memcpy(args->argc[i], argc[i], strlen(argc[i]));
  }

  mcu_context[CONTEXT_ARGS] = args;
  tcb->mcu_context = (void *)mcu_context;
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
  void **mcu_context = tcb->mcu_context;
  sim_mcu_arguments_t *args = mcu_context[CONTEXT_ARGS];

  /* Release the memory allocated for the arguments */

  for (int i = 0; i < args->argv; i++)
  {
    free(args->argc[i]);
  }

  free(tcb->mcu_context);

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
