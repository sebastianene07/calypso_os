#include <board.h>

#include <stdint.h>
#include <s_heap.h>
#include <scheduler.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define RAM_BASE   (void *)(0x20000000)
#define STACK_TOP  (void *)(RAM_BASE + 0x5000)

#define HEAP_START (void *)(RAM_BASE + 0x5004)
#define HEAP_END   (void *)(RAM_BASE + 0x19000)
#define HEAP_BLOCK_SIZE (16)

#define XTAL            (50000000UL)     /* Oscillator frequency */
#define SYSTEM_CLOCK    (XTAL / 2U)

/****************************************************************************
 * Public Data
 ****************************************************************************/

uint32_t SystemCoreClock = SYSTEM_CLOCK;  /* System Core Clock Frequency */

extern unsigned long _stext;
extern unsigned long _sbss;
extern unsigned long _sdata;
extern unsigned long _etext;
extern unsigned long _ebss;
extern unsigned long _edata;
extern unsigned long _srodata;

/* Heap definition */

heap_t g_my_heap;

typedef uint32_t useconds_t;

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

void c_startup(void);

void dummy_fn(void);

void os_startup(void);

void Pend_SV_Handler(void);
void SysTick_Handler(void);

volatile int is_enabled = 0;

/* The prototype shows it is a naked function - in effect this is just an
assembly function. */
static void HardFault_Handler( void ) __attribute__( ( naked ) );

void prvGetRegistersFromStack( uint32_t *pulFaultStackAddress )
{
/* These are volatile to try and prevent the compiler/linker optimising them
away as the variables never actually get used.  If the debugger won't show the
values of the variables, make them global my moving their declaration outside
of this function. */
volatile uint32_t r0;
volatile uint32_t r1;
volatile uint32_t r2;
volatile uint32_t r3;
volatile uint32_t r12;
volatile uint32_t lr; /* Link register. */
volatile uint32_t pc; /* Program counter. */
volatile uint32_t psr;/* Program status register. */

    r0 = pulFaultStackAddress[ 0 ];
    r1 = pulFaultStackAddress[ 1 ];
    r2 = pulFaultStackAddress[ 2 ];
    r3 = pulFaultStackAddress[ 3 ];

    r12 = pulFaultStackAddress[ 4 ];
    lr = pulFaultStackAddress[ 5 ];
    pc = pulFaultStackAddress[ 6 ];
    psr = pulFaultStackAddress[ 7 ];

    /* When the following line is hit, the variables contain the register values. */
    for( ;; );
}

/* The fault handler implementation calls a function called
prvGetRegistersFromStack(). */
static void HardFault_Handler(void)
{
    __asm volatile
    (
        " tst lr, #4                                                \n"
        " ite eq                                                    \n"
        " mrseq r0, msp                                             \n"
        " mrsne r0, psp                                             \n"
        " ldr r1, [r0, #24]                                         \n"
        " ldr r2, handler2_address_const                            \n"
        " bx r2                                                     \n"
        " handler2_address_const: .word prvGetRegistersFromStack    \n"
    );
}

__attribute__((section(".isr_vector")))
void (*vectors[])(void) = {
        STACK_TOP,
        c_startup,
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
};

void dummy_fn(void)
{
        while(1)
        {

        }
}

void usleep(const useconds_t sec)
{
  for (uint32_t i = 0; i < (uint32_t)sec; i++);
}

void c_startup(void)
{
  unsigned long *src, *dst;

  src = &_etext;
  dst = &_sdata;
  while(dst < &_edata)
      *(dst++) = *(src++);

  src = &_sbss;
  while(src < &_ebss)
      *(src++) = 0;

  /* TODO: If the stack goes below _ebss we must ASSERT */

  /* Initialize the HEAP memory */

  s_init(&g_my_heap,
         HEAP_START,
         HEAP_END,
         HEAP_BLOCK_SIZE);

  sched_init();
  sched_create_task(os_startup, 1024);

  /* Configure Sys Tick */

  SysTick_Config(SystemCoreClock / 100);

  /* Schedule tasks */

  while (1)
  {
    ;;
  }
}
