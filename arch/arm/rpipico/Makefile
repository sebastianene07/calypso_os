SRC := board_initialize.c startup.c uart.c
OBJS += $(patsubst %.c,%.o,$(SRC))
PICO_SDK=arch/arm/rpipico/pico-sdk
ARCH_FLAGS:=${CFLAGS}						\
-I$(TOPDIR)/include						\
-I$(TOPDIR)/s_alloc						\
-I./include/							\
-I$(TOPDIR)/sched/include					\
-I$(TOPDIR)/lib/CMSIS/CMSIS/Include/				\
-I$(TOPDIR)/lib/CMSIS/Device/ARM/ARMCM0plus/Include
LDFLAGS += -Llibbsp.a

SRC_ASM:= context_switch.s
OBJS_ASM:=$(patsubst %.s,%.o,$(SRC_ASM))

all: $(OBJS) $(OBJS_ASM)
	echo "Building arch specific"
	${PREFIX}ar -rc $(TOPDIR)/$(TMP_LIB) $(OBJS) $(OBJS_ASM)

%.o : %.c
	${PREFIX}gcc $(ARCH_FLAGS) -c $< -o $@

%.o : %.s
	${PREFIX}as $(ASM_FLAGS) -c $< -o $@

clean:
	rm -rf build
	rm -f libbsp.a bs2_default.bin bs2_default.s
	find . -iname "*.o" -exec rm -f {} \;
