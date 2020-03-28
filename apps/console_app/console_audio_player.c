#include <board.h>
#include <console_main.h>

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>

#include <source/ff.h>
#include <vfs.h>
#include <stdio.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* To be efficient read a block size at a time */

#define AUDIO_BUFFER_SIZE                 (512)

/* Audio driver macros */

#define AUDIO_GPIO_PIN                    (10)
#define AUDIO_GPIO_PORT                   (0)

#define AUDIO_CONFIG(base_peripheral, offset_r)     \
                ((*((volatile uint32_t *)((base_peripheral) + (offset_r)))))

/* Bse registers for NRF52840 */

#define NRF52840_PWM0_BASE                (0x4001C000)
#define NRF52840_PWM1_BASE                (0x40021000)
#define NRF52840_PWM2_BASE                (0x40022000)
#define NRF52840_PWM3_BASE                (0x4002D000)

/* Offset registers */

#define NRF52840_PRESCALER                (0x50C)
#define NRF52840_ENABLE                   (0x500)
#define NRF52840_SHORTS                   (0x200)
#define NRF52840_INTENSET                 (0x304)
#define NRF52840_MODE                     (0x504)
#define NRF52840_COUNTERTOP               (0x508)
#define NRF52840_DECODER                  (0x510)

#define NRF52840_SEQ_0_PTR                (0x520)
#define NRF52840_SEQ_0_CNT                (0x524)
#define NRF52840_SEQ_0_REFRESH            (0x528)
#define NRF52840_SEQ_0_ENDDELAY           (0x52C)

#define NRF52840_SEQ_1_PTR                (0x540)
#define NRF52840_SEQ_1_CNT                (0x544)
#define NRF52840_SEQ_1_REFRESH            (0x548)
#define NRF52840_SEQ_1_ENDDELAY           (0x54C)

/* Pin Selection for each channel */

#define NRF52840_PSEL_OUT_0               (0x560)
#define NRF52840_PSEL_OUT_1               (0x564)
#define NRF52840_PSEL_OUT_2               (0x568)
#define NRF52840_PSEL_OUT_3               (0x56C)

/* Task registers */

#define NRF52840_TASKS_STOP               (0x004)
#define NRF52840_TASKS_SEQ_0_START        (0x008)
#define NRF52840_TASKS_SEQ_1_START        (0x00C)
#define NRF52840_TASKS_NEXTSTEP           (0x010)

/* Event registers */

#define NRF52840_EVENTS_STOPPED           (0x104)
#define NRF52840_EVENTS_SEQ_0_STARTED     (0x108)
#define NRF52840_EVENTS_SEQ_1_STARTED     (0x10C)
#define NRF52840_EVENTS_SEQ_0_END         (0x110)
#define NRF52840_EVENTS_SEQ_1_END         (0x114)
#define NRF52840_EVENTS_PWMPERIOD         (0x118)
#define NRF52840_EVENTS_LOOP_DONE         (0x11C)

/****************************************************************************
 * Private Types
 ****************************************************************************/

enum nrf52840_prescaler_e {
  DIV_1 = 0,      /* Divide by 1 (16MHz) */
  DIV_2,          /* Divide by 2 ( 8MHz) */
  DIV_4,          /* Divide by 4 ( 4MHz) */
  DIV_8,          /* Divide by 8 ( 2MHz) */
  DIV_16,         /* Divide by 16 ( 1MHz) */
  DIV_32,         /* Divide by 32 ( 500kHz) */
  DIV_64,         /* Divide by 64 ( 250kHz) */
  DIV_128         /* Divide by 128 ( 125kHz) */
};

/****************************************************************************
 * Private Functions Definition
 ****************************************************************************/

/****************************************************************************
 * Private Data
 ****************************************************************************/

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static int audio_driver_init(void)
{
  AUDIO_CONFIG(NRF52840_PWM0_BASE, NRF52840_PSEL_OUT_0) = AUDIO_GPIO_PIN |
    (AUDIO_GPIO_PORT << 5);
  AUDIO_CONFIG(NRF52840_PWM0_BASE, NRF52840_ENABLE) = 1;

  /* Let's set the clock frequency to 125kHZ (it should be enough) */

  AUDIO_CONFIG(NRF52840_PWM0_BASE, NRF52840_PRESCALER) = DIV_128;

  /* We want a frequency of 500Hz with 50 % duty cycle */

  AUDIO_CONFIG(NRF52840_PWM0_BASE, NRF52840_COUNTERTOP) = 250;

  /* Count up to counter */

  AUDIO_CONFIG(NRF52840_PWM0_BASE, NRF52840_MODE)    = 0;
  AUDIO_CONFIG(NRF52840_PWM0_BASE, NRF52840_DECODER) = 0;
  
  uint16_t pwm_seq[1] = {125};

  AUDIO_CONFIG(NRF52840_PWM0_BASE, NRF52840_SEQ_0_PTR) = pwm_seq; 
  AUDIO_CONFIG(NRF52840_PWM0_BASE, NRF52840_SEQ_1_CNT) =
    sizeof(pwm_seq) / sizeof(pwm_seq[0]);

  AUDIO_CONFIG(NRF52840_PWM0_BASE, NRF52840_TASKS_SEQ_0_START) = 1;

  return OK;
}

static int audio_driver_play_sample()
{

}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int console_audio_player(int argc, const char *argv[])
{
  if (argc < 2) {
    printf("No input file specified\r\n");
    return -EINVAL;
  }

  audio_driver_init();

#if 0
  int fd = open(argv[1], 0);
  if (fd < 0) {
    printf("Error %d open %s\n", fd, argv[1]);
    return fd;
  }

  char buffer[AUDIO_BUFFER_SIZE];
  int nread = 0;
  int ret;

  do {

    memset(buffer, 0, sizeof(buffer));
    ret = 0;
    nread = 0;

    while ((sizeof(buffer) - nread - 1) > 0 &&
           (ret = read(fd, buffer + nread, sizeof(buffer) - nread - 1)) > 0) {
      nread += ret;
    }

    /* Send audio to the PWM driver */


  } while (ret != 0);

  close(fd);
#endif
  return 0;
}
