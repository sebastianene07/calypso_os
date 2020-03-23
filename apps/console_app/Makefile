SRC:=console_main.c

ifeq ($(CONFIG_CONSOLE_TEST_DISPLAY),y)
SRC+= console_display_test.c test.c
endif

ifeq ($(CONFIG_CONSOLE_DATE_ON),y)
SRC+= console_date.c
endif

ifeq ($(CONFIG_CONSOLE_FREE),y)
SRC+= console_free.c
endif

ifeq ($(CONFIG_CONSOLE_LS),y)
SRC+= console_ls.c
endif

ifeq ($(CONFIG_CONSOLE_MOUNT),y)
SRC+= console_mount.c
endif

ifeq ($(CONFIG_CONSOLE_CAT),y)
SRC+= console_cat.c
endif

ifeq ($(CONFIG_CONSOLE_SENSOR_MEASURE),y)
SRC+= console_sensor_measure.c
endif

ifeq ($(CONFIG_LIBRARY_BSEC),y)
SRC+= bsec_lib/bsec_integration.c
endif

ifeq ($(CONFIG_CONSOLE_SLEEP),y)
SRC+= console_sleep.c
endif

ifeq ($(CONFIG_CONSOLE_ECHO),y)
SRC+= console_echo.c
endif

ifeq ($(CONFIG_CONSOLE_NRF_INIT_SOFTDEVICE_APP),y)
SRC += console_nrf_init.c
endif

ifeq ($(CONFIG_CONSOLE_RM),y)
SRC += console_rm.c
endif

OBJS:=$(patsubst %.c,%.o,$(SRC))
CONSOLE_FLAGS := ${CFLAGS}  \
	-I$(TOPDIR)/include \
	-I$(TOPDIR)/include/chip \
	-I$(TOPDIR)/drivers \
  -I$(TOPDIR)/s_alloc \
	-I$(TOPDIR)/sched/include	\
	-I$(TOPDIR)/apps/console_app	\
	-I$(TOPDIR)/fatfs/		\
	-I$(TOPDIR)/lib/
SRC_ASM:=$(wildcard *.s)
OBJS_ASM:=$(patsubst %.s,%.o,$(SRC_ASM))

all: $(OBJS) $(OBJS_ASM)
	${PREFIX}ar -rc $(TOPDIR)/$(TMP_LIB) $(OBJS) $(OBJS_ASM)

%.o : %.c
	${PREFIX}gcc $(CONSOLE_FLAGS) -c $< -o $@

%.o : %.s
	${PREFIX}as $(CONSOLE_FLAGS) -c $< -o $@

clean:
	find . -iname "*.o" -exec rm -f {} \;
