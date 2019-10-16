SRC := $(wildcard *.c)
SRC += $(wildcard */*.c)
OBJS += $(patsubst %.c,%.o,$(SRC))
ARCH_FLAGS:=${CFLAGS} -I$(TOPDIR)/include -I$(TOPDIR)/s_alloc -I./include/ -I$(TOPDIR)/sched/include
ASM_FLAGS := -march=armv6

SRC_ASM:=$(wildcard *.s)
OBJS_ASM:=$(patsubst %.s,%.o,$(SRC_ASM))

all: $(OBJS) $(OBJS_ASM)
	echo "Building arch specific"
	${PREFIX}ar -rc $(TOPDIR)/$(TMP_LIB) $(OBJS) $(OBJS_ASM)

%.o : %.c
	${PREFIX}gcc $(ARCH_FLAGS) -c $< -o $@

%.o : %.s
	${PREFIX}as $(ASM_FLAGS) -c $< -o $@

clean:
	find . -iname "*.o" -exec rm -f {} \;
