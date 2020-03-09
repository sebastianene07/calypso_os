SRC:=

ifeq ($(CONFIG_DISPLAY_SSD1331), y)
SRC+= ssd_1331.c
endif

OBJS:=$(patsubst %.c,%.o,$(SRC))
CONSOLE_FLAGS := ${CFLAGS}																					\
							-I$(TOPDIR)/include																	\
							-I$(TOPDIR)/include/chip														\
							-I$(TOPDIR)/s_alloc																	\
							-I$(TOPDIR)/sched/include														\

SRC_ASM:=$(wildcard *.s)
OBJS_ASM:=$(patsubst %.s,%.o,$(SRC_ASM))

ifeq ($(SRC),)
all:
	@echo "No display specified"
else
all: $(OBJS) $(OBJS_ASM)
	${PREFIX}ar -rc $(TOPDIR)/$(TMP_LIB) $(OBJS) $(OBJS_ASM)

%.o : %.c
	${PREFIX}gcc $(CONSOLE_FLAGS) -c $< -o $@

%.o : %.s
	${PREFIX}as $(CONSOLE_FLAGS) -c $< -o $@
endif

clean:
	find . -iname "*.o" -exec rm -f {} \;
