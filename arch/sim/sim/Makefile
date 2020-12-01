SRC := board_initialize.c
SRC += board_uart.c
SRC += board_rtc.c

ifeq ($(CONFIG_HOST_OS),"Darwin")
CFLAGS+= -mstack-alignment=16 -mno-sse -mstackrealign
endif

OBJS := $(patsubst %.c,%.o,$(SRC))
INCLUDE_PATHS:=-I$(TOPDIR)/include -I$(TOPDIR)/s_alloc -I./include/ -I$(TOPDIR)/sched/include -I$(TOPDIR)/drivers/
ARCH_FLAGS:=${CFLAGS} ${INCLUDE_PATHS}
SRC_REL := board_memory.c
OBJS_REL := $(patsubst %.c,%.o,$(SRC_REL))

SRC_ASM:=$(wildcard *.s)
OBJS_ASM:=$(patsubst %.s,%.o,$(SRC_ASM))

all: $(OBJS) $(OBJS_ASM) $(OBJS_REL) host_board_up.o
	echo "Building arch specific"
	${PREFIX}ar -rc $(TOPDIR)/$(TMP_LIB) $(OBJS) $(OBJS_ASM)

%.o : %.c
	${PREFIX}gcc $(ARCH_FLAGS) -c $< -o $@

host_board_up.o : host_board_up.c
	@echo "AICI :" $<
	${PREFIX}gcc -c -Wall -Wstrict-prototypes -Wshadow -Wundef -g -fno-builtin -fno-common  $< -o $@

%.o : %.s
	${PREFIX}as $(ASM_FLAGS) -c $< -o $@

clean:
	find . -iname "*.o" -exec rm -f {} \;
