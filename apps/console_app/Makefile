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

ifeq ($(CONFIG_FATFS_MOUNT),y)
SRC+= console_mount.c
endif

ifeq ($(CONFIG_CONSOLE_CAT),y)
SRC+= console_cat.c
endif

OBJS:=$(patsubst %.c,%.o,$(SRC))
CONSOLE_FLAGS := ${CFLAGS}																					\
							-I$(TOPDIR)/include																	\
							-I$(TOPDIR)/s_alloc																	\
							-I$(TOPDIR)/sched/include														\
							-I$(TOPDIR)/apps/console_app												\
							-I$(TOPDIR)/lib/fatfs/
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
