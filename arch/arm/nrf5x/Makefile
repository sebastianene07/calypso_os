SRC := $(wildcard *.c)
SRC += $(wildcard */*.c)
OBJS += $(patsubst %.c,%.o,$(SRC))
ARCH_FLAGS:=${CFLAGS} -I$(TOPDIR)/include 	\
		      -I$(TOPDIR)/s_alloc 	\
		      -I./include/ 	  	\
		      -I$(TOPDIR)/sched/include \
		      -I$(TOPDIR)/drivers	\
		      -I$(TOPDIR)/lib		\
		      -I$(TOPDIR)/arch/arm/nrf5x/

ASM_FLAGS :=-mcpu=cortex-m4 -mthumb -mfloat-abi=soft
SRC_ASM:= context_switch_fp.s

OBJS_ASM:=$(patsubst %.s,%.o,$(SRC_ASM))

DIRS :=

ifeq ($(CONFIG_CONSOLE_NRF_INIT_SOFTDEVICE_APP),y)
DIRS+=softdevice/ble_proximity/
endif

all: ARCH_LIBRARIES $(OBJS) $(OBJS_ASM)
	echo "Building arch specific"
	${PREFIX}ar -rc $(TOPDIR)/$(TMP_LIB) $(OBJS) $(OBJS_ASM)

ARCH_LIBRARIES:
	echo "Building arch libraries"
	for lib_dir in $(DIRS) ; do \
	make -C $$lib_dir archive ;\
	done ;

%.o : %.c
	${PREFIX}gcc $(ARCH_FLAGS) -c $< -o $@

%.o : %.s
	${PREFIX}as $(ASM_FLAGS) -c $< -o $@

clean:
	find . -iname "*.o" -exec rm -f {} \;

