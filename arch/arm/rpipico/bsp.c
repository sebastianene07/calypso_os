#include <stdio.h>
#include "pico/stdlib.h"
#include <stdarg.h>
#include <unistd.h>
#include "hardware/resets.h"
#include "hardware/regs/resets.h"

int vprintf(const char * restrict format, va_list ap)
{
  return 0;
}

int puts(const char *s)
{
  return 0;
}

int bsp_main() {

  clocks_init();
  unreset_block_wait(RESETS_RESET_BITS);

  //setup_default_uart();
  //printf("[RPIPICO] Uart initialized\n");
  const uint LED_PIN = PICO_DEFAULT_LED_PIN;
  gpio_init(LED_PIN);
  gpio_set_dir(LED_PIN, GPIO_OUT);
  while (true) {
    gpio_put(LED_PIN, 1);//
    sleep_ms(250);                                                          
    gpio_put(LED_PIN, 0);                                                   
    sleep_ms(250); 
  }

  return 0;
}

int main(void)
{
  return bsp_main();
}
