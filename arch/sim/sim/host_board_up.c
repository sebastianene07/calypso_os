/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <stdio.h>
#include <string.h>

#ifdef __APPLE__
  #include <sys/semaphore.h>
#else
  #include <semaphore.h>
#endif

#include <signal.h>
#include <stdarg.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../../../include/chip/simulator.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* Error macro that prints to standard error */

#define _err(fmt, ...)            fprintf(stderr, "[ERROR] "fmt, __VA_ARGS__)

/* The timer interval that generates sys tick events */

#define SCHEDULING_TIME_SLICE_MS  (1)

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

/* This function is the entry point for the Calypso OS */

void _start(void);

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: host_signal_handler
 *
 * Description:
 *   Signal handler for the host process that invokes the context switching
 *   mechanism..
 *
 * Input Parameters:
 *   sig    - signal handler number
 *   si     - the signal info
 *   old_ucontext - the old processor state
 *
 ****************************************************************************/

static void host_signal_handler(int sig, siginfo_t *si, void *old_ucontext)
{
  sched_context_switch();
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: host_console_putc
 *
 * Description:
 *   This function prints out one character to the console. It is invoked from
 *   the lower layer UART to show a character to the console. In a hardware
 *   build this function should be responsible for storing the character in
 *   the UART peripheral register taht will later be transfered on the wire.
 *
 * Input Parameters:
 *   c    - the character that will be printed to the console
 *
 * Assumptions/Limitations:
 *   Anything else that one might need to know to use this function.
 *
 ****************************************************************************/

void host_console_putc(int c)
{
  char buffer[12];
  memset(buffer, 0, sizeof(buffer));

  int ret = snprintf(buffer, sizeof(buffer), "%c", c);
  if (ret > 0) {
    write(1, buffer, ret);
  }
}

/****************************************************************************
 * Name: host_simulated_systick
 *
 * Description:
 *   Simulate a tick event using a host timer. This function sets up the
 *   signal mask for the host process in which we run Caypso OS, it sets
 *   up a timer and it fires it with SCHEDULING_TIME_SLICE_MS interval.
 *   It is called from the board control logic during initialization.
 *
 * Assumptions/Limitations:
 *   Anything else that one might need to know to use this function.
 *
 ****************************************************************************/

void host_simulated_systick(void)
{
  int ret;
  struct itimerval it;

  struct sigaction act;
  sigset_t set;

  act.sa_sigaction = host_signal_handler;
  sigemptyset(&act.sa_mask);
  act.sa_flags = SA_SIGINFO;

  sigemptyset(&set);
  sigaddset(&set, SIGALRM);
  sigaddset(&set, SIGSEGV);
  if ((ret = sigaction(SIGALRM, &act, NULL)) != 0) {
      _err("%d signal handler", ret);
  }

  it.it_interval.tv_sec  = 0;
  it.it_interval.tv_usec = SCHEDULING_TIME_SLICE_MS * 1000;
  it.it_value            = it.it_interval;

  ret = setitimer(ITIMER_REAL, &it, NULL);
  if (ret < 0) {
    _err("%d settimer\n", ret);
    return;
  }

  return;
}

/****************************************************************************
 * Name: __enable_irq
 *
 * Description:
 *   This function enables the SysTick simulated interrupts by changing
 *   the process signal mask.
 *
 ****************************************************************************/

void __enable_irq(void)
{
  sigset_t newset;

  sigfillset(&newset);
  pthread_sigmask(SIG_UNBLOCK, &newset, NULL);
}

/****************************************************************************
 * Name: __disable_irq
 *
 * Description:
 *   This function disablesthe SysTick simulated interrupts by changing
 *   the process signal mask.
 *
 ****************************************************************************/

void __disable_irq(void)
{
  sigset_t newset;

  /* Disable signals */

  sigfillset(&newset);
  pthread_sigmask(SIG_BLOCK, &newset, NULL);
}

/****************************************************************************
 * Name: main
 *
 * Description:
 *   Main entry point for the simulation. This function calls the entry point
 *   for the Calypso OS.
 *
 * Input Arguments:
 *   argc - the number of arguments
 *   argv - the arguments buffer
 *
 ****************************************************************************/

int main(int argc, char **argv)
{
  /* Start the Calypso OS simulation */

  _start();

  return 0;
}
