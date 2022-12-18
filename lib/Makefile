PATH_FATFS = ./fatfs/source

SRCS:= small_printf.c small_sscanf.c
SYSCALLS_SRCS:=

ifeq ($(CONFIG_LIBRARY_FATFS),y)
SRCS_FATFS += $(wildcard $(PATH_FATFS)/*.c) fatfs_glue.c
endif

ifeq ($(CONFIG_SEPARATE_BUNDLES),y)
SYSCALLS_SRCS += syscall_stubs.c
endif

OBJS_FATFS := $(patsubst %.c, %.o, $(SRCS_FATFS))
OBJS := $(patsubst %.c, %.o, $(SRCS))
SYSCALLS_OBJS :=$(patsubst %.c, %.o, $(SYSCALLS_SRCS))$

LIB_FLAGS := $(CFLAGS)		\
	-I$(TOPDIR)/include	\
	-I$(TOPDIR)/include/chip	\
	-I$(TOPDIR)/s_alloc		\
	-I$(TOPDIR)/sched/include	\
	-I$(TOPDIR)/drivers/

all: fatfs bsec small_print syscalls

fatfs: $(OBJS_FATFS)
ifeq ($(CONFIG_LIBRARY_FATFS),y)
	@echo "Building FATFS library " $(OBJS_FATFS)
	${PREFIX}ar -rc $(TOPDIR)/$(TMP_LIB) $(OBJS_FATFS)
endif

bsec:
ifeq ($(CONFIG_LIBRARY_BSEC),y)
	@echo "Building BSEC library "
	$(MAKE) -C bsec all
endif

syscalls: $(SYSCALLS_OBJS)
	-${PREFIX}ar -rcs libapp.a $<

small_print: $(OBJS)
	${PREFIX}ar -rc $(TOPDIR)/$(TMP_LIB) $(OBJS)

%.o : %.c
	@echo Flaugurile sunt: $(LIB_FLAGS)
	${PREFIX}gcc $(LIB_FLAGS) -c $< -o $@

clean:
	find . -iname "*.o" -exec rm -f {} \;
	find $(PATH_FATFS) -iname "*.o" -exec rm -f {} \;
	rm -f libapp*

.PHONY: fatfs clean all bsec small_print
