#include <board.h>
#include <console_main.h>

#include <gpio.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>

#include <source/ff.h>
#include <vfs.h>
#include <stdio.h>
#include <wav.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* To be efficient read a block size at a time */

#define AUDIO_BUFFER_SIZE                 (512)

/* Audio driver macros */

#define AUDIO_GPIO_PIN                    (1)
#define AUDIO_GPIO_PORT                   (1)

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
#define NRF52840_LOOP                     (0x514)

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

/* The PWM counter top value used here to re-sample the received data */

static uint32_t g_pwm_counter;

/* The sequence buffer */

static uint16_t *g_seq_0_buffer;
static uint16_t g_seq_0_buffer_index;

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static void audio_driver_stop(void)
{
  if (AUDIO_CONFIG(NRF52840_PWM0_BASE, NRF52840_EVENTS_SEQ_0_STARTED)) {
    AUDIO_CONFIG(NRF52840_PWM0_BASE, NRF52840_TASKS_STOP) = 1;

    while (AUDIO_CONFIG(NRF52840_PWM0_BASE, NRF52840_EVENTS_STOPPED) == 0);
    AUDIO_CONFIG(NRF52840_PWM0_BASE, NRF52840_EVENTS_STOPPED) = 0;
    AUDIO_CONFIG(NRF52840_PWM0_BASE, NRF52840_EVENTS_SEQ_0_STARTED) = 0;
  }

  free(g_seq_0_buffer);
  g_seq_0_buffer = NULL;
}

static int audio_driver_init(uint16_t frequency)
{
  audio_driver_stop();

  g_seq_0_buffer = calloc(sizeof(uint16_t), AUDIO_BUFFER_SIZE);
  if (!g_seq_0_buffer)
    return -ENOMEM;

  gpio_configure(AUDIO_GPIO_PIN, AUDIO_GPIO_PORT,
                 GPIO_DIRECTION_OUT, GPIO_PIN_INPUT_DISCONNECT,
                 GPIO_NO_PULL, GPIO_PIN_S0S1, GPIO_PIN_NO_SENS);

  AUDIO_CONFIG(NRF52840_PWM0_BASE, NRF52840_PSEL_OUT_0) = AUDIO_GPIO_PIN |
    (AUDIO_GPIO_PORT << 5);
  AUDIO_CONFIG(NRF52840_PWM0_BASE, NRF52840_ENABLE) = 1;

  /* Let's set the clock prescaler freq to 1MhZ (it should be enough) */

  AUDIO_CONFIG(NRF52840_PWM0_BASE, NRF52840_PRESCALER) = DIV_1;

  g_pwm_counter = 16 * 1000000 / frequency;

  printf("Counter : %d\n", g_pwm_counter);
  AUDIO_CONFIG(NRF52840_PWM0_BASE, NRF52840_COUNTERTOP) = g_pwm_counter;

  /* Count up to counter */

  AUDIO_CONFIG(NRF52840_PWM0_BASE, NRF52840_MODE)    = 0;
  AUDIO_CONFIG(NRF52840_PWM0_BASE, NRF52840_DECODER) = 0;
  AUDIO_CONFIG(NRF52840_PWM0_BASE, NRF52840_LOOP)    = 0;
  AUDIO_CONFIG(NRF52840_PWM0_BASE, NRF52840_SEQ_0_PTR) = g_seq_0_buffer; 
  AUDIO_CONFIG(NRF52840_PWM0_BASE, NRF52840_SEQ_0_CNT) = AUDIO_BUFFER_SIZE;
  AUDIO_CONFIG(NRF52840_PWM0_BASE, NRF52840_SEQ_0_REFRESH)     = 0;
  AUDIO_CONFIG(NRF52840_PWM0_BASE, NRF52840_SEQ_0_ENDDELAY)    = 0;
  AUDIO_CONFIG(NRF52840_PWM0_BASE, NRF52840_TASKS_SEQ_0_START) = 1;

  printf("Request SEQ0 start task\n");
  while(AUDIO_CONFIG(NRF52840_PWM0_BASE, NRF52840_EVENTS_SEQ_0_STARTED) == 0);
  printf("Started SEQ0 task\n");

  return OK;
}

static void audio_show_file_info(wav_header *file)
{
  printf("\n######## WAV HEADER ##########\n");

  printf("RIFF header %c%c%c%c\n",
         file->riff_header[0],
         file->riff_header[1],
         file->riff_header[2],
         file->riff_header[3]);

  printf("size wav_portion %u\n", file->wav_size);
  printf("WAVE header %c%c%c%c\n",
         file->wave_header[0],
         file->wave_header[1],
         file->wave_header[2],
         file->wave_header[3]);
  printf("fmt header %c%c%c%c chunk_size:%d audio_format:%d num_chan:%d\n",
         file->fmt_header[0],
         file->fmt_header[1],
         file->fmt_header[2],
         file->fmt_header[3],
         file->fmt_chunk_size,
         file->audio_format,
         file->num_channels);

  printf("sample rate: %d bit_depth %d sample_align:%d byte_rate:%d\n",
         file->sample_rate,
         file->bit_depth,
         file->sample_alignment,
         file->byte_rate);

  printf("\n######## END WAV HEADER ##########\n\n");
}

static int audio_file_is_valid(wav_header *file)
{
  return 0;
}

static void audio_convert_pcm_to_seq(uint8_t *pcm_buffer,
                                     size_t buffer_len,
                                     uint16_t bit_depth)
{
  AUDIO_CONFIG(NRF52840_PWM0_BASE, NRF52840_SEQ_0_PTR) = g_seq_0_buffer; 
  AUDIO_CONFIG(NRF52840_PWM0_BASE, NRF52840_SEQ_0_CNT) = buffer_len;

  /* Assuming each sample take 1 byte */

  for (int i = 0; i < buffer_len; i++) {
    g_seq_0_buffer[i] = g_pwm_counter * 2 * ((int16_t)pcm_buffer[i] + 128) / 0xFF;

    if (g_seq_0_buffer[i] > g_pwm_counter)
      g_seq_0_buffer[i] = g_pwm_counter;
  }

  AUDIO_CONFIG(NRF52840_PWM0_BASE, NRF52840_TASKS_SEQ_0_START) = 1;

  while(AUDIO_CONFIG(NRF52840_PWM0_BASE, NRF52840_EVENTS_SEQ_0_STARTED) == 0);

  while(AUDIO_CONFIG(NRF52840_PWM0_BASE, NRF52840_EVENTS_SEQ_0_END) == 0);
  AUDIO_CONFIG(NRF52840_PWM0_BASE, NRF52840_EVENTS_SEQ_0_END) = 0;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int console_audio_player(int argc, const char *argv[])
{
  if (argc != 2) {
    printf("aplayer <filename>\r\n");
    return -EINVAL;
  }

  g_seq_0_buffer_index = 0;
  int fd = open(argv[1], 0);
  if (fd < 0) {
    printf("Error %d open %s\n", fd, argv[1]);
    return fd;
  }

  disable_int();
  char buffer[AUDIO_BUFFER_SIZE];
  int nread = 0, total_read = 0;
  int ret;
  int is_header_interpretted = 0;

  do {

    memset(buffer, 0, sizeof(buffer));
    ret = 0;
    nread = 0;

    while ((sizeof(buffer) - nread - 1) > 0 &&
           (ret = read(fd, buffer + nread, sizeof(buffer) - nread - 1)) > 0) {
      nread += ret;
      total_read += ret;
    }

    if (ret <= 0)
      break;

    if (is_header_interpretted == 0 &&
        nread > sizeof(wav_header)) {

      wav_header *song_info = (wav_header *)buffer;
      audio_show_file_info(song_info);
      if (audio_file_is_valid(song_info) < 0) {
        ret = -1;
        break;
      }

      /* Send audio to the PWM driver */

      audio_driver_init(song_info->sample_rate);
      nread -= sizeof(wav_header);

      if (nread > 0) {
        audio_convert_pcm_to_seq(song_info->bytes, nread, 8); 
      }

      is_header_interpretted = 1;
    }

    //printf("--- Convert %d samples --- \n", nread);
    audio_convert_pcm_to_seq(buffer, nread, 8); 
  } while (ret != 0);

  close(fd);
  enable_int();

  printf("aplayer will close now total read: %d bytes\n", total_read);
  audio_driver_stop();
  return 0;
}
