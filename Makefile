PREFIX := arm-none-eabi-

CFLAGS += -mcpu=cortex-m4 -mthumb -g3 -o0
LDFLAGS += ${CFLAGS} -nostartfiles -nodefaultlibs -Wl,-Tlinker.ld

# Include user config
include .config
$(info machine_type=$(MACHINE_TYPE))

SRC_DIRS := $(shell find . -iname $(MACHINE_TYPE))
SRC_DIRS += sched
TOPDIR=$(shell pwd)
TMP_LIB=tmp_lib.a

export CFLAGS
export PREFIX
export TOPDIR
export TMP_LIB

all:
	for src_dir in $(SRC_DIRS) ; do \
		$(MAKE) -C $$src_dir	all;	\
	done ;

	${PREFIX}ar xv ${TMP_LIB}
	${PREFIX}gcc ${LDFLAGS} *.o -o build.elf
	${PREFIX}objcopy -O ihex build.elf build.hex

load:
	nrfprog -f nrf52 --program build.hex --sectorerase

.PHONY: clean

clean:
	rm build.* *.o linker* tmp_lib*
	for src_dir in $(SRC_DIRS) ; do \
		$(MAKE) -C $$src_dir	clean;	\
	done ;


distclean:
	rm .config
