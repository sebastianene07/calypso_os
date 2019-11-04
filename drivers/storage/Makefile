SRC:=

ifeq ($(CONFIG_SPI_SDCARD), y)
SRC+= spi_sdcard.c
endif

OBJS:=$(patsubst %.c,%.o,$(SRC))
CONSOLE_FLAGS := ${CFLAGS}																				\
							-I$(TOPDIR)/include																	\
							-I$(TOPDIR)/s_alloc																	\
							-I$(TOPDIR)/sched/include														\
              -I$(TOPDIR)/drivers/storage                         \

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