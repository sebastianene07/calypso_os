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

#define _err(fmt, ...) fprintf(stderr, "[ERROR] "fmt, __VA_ARGS__)
#define SCHEDULING_TIME_SLICE_MS          (1)

void _start(void);
void sched_context_switch(void);


void host_console_putc(int c)
{
  char buffer[12];
  memset(buffer, 0, sizeof(buffer));

  int ret = snprintf(buffer, sizeof(buffer), "%c", c);
  if (ret > 0) {
    write(1, buffer, ret);
  }
}

static void host_signal_handler(int sig, siginfo_t *si, void *old_ucontext)
{
  sched_context_switch();
}

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

int main(int argc, char **argv)
{
  _start();
  return 0;
}
