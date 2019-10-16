PATH_FATFS = ./fatfs/source

SRCS_FATFS := small_printf.c small_sscanf.c

ifeq ($(CONFIG_FATFS_SUPPORT),y)
SRCS_FATFS += $(wildcard $(PATH_FATFS)/*.c) fatfs_glue.c
endif

OBJS_FATFS := $(patsubst %.c, %.o, $(SRCS_FATFS))

FATFS_FLAGS := ${CFLAGS}																					\
							-I$(TOPDIR)/include																	\
							-I$(TOPDIR)/s_alloc																	\
							-I$(TOPDIR)/sched/include														\

all: fatfs

fatfs: $(OBJS_FATFS)
	@echo "Building FATFS library " $(OBJS_FATFS)
	${PREFIX}ar -rc $(TOPDIR)/$(TMP_LIB) $(OBJS_FATFS)

%.o : %.c
	${PREFIX}gcc $(FATFS_FLAGS) -c $< -o $@

clean:
	find $(TOPDIR)/$(PATH_FATFS) -iname "*.o" -exec rm -f {} \;

.PHONY: fatfs clean all
