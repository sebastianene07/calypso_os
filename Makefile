PREFIX := arm-none-eabi-
TOPDIR=$(shell pwd)
CFLAGS += -mcpu=cortex-m4 -mthumb -g3 -o0
LDFLAGS += ${CFLAGS} -nostartfiles -nodefaultlibs -Wl,-T${TOPDIR}/linker.ld
DEBUG_PORT=2771

# Include user config
include .config
$(info machine_type=$(MACHINE_TYPE))

SRC_DIRS := $(shell find . -iname $(MACHINE_TYPE))
SRC_DIRS += sched s_alloc utils
TMP_LIB=tmp_lib.a
TARGET=$(MACHINE_TYPE)

export CFLAGS
export PREFIX
export TOPDIR
export TMP_LIB
export TARGET

all:
	for src_dir in $(SRC_DIRS) ; do \
		$(MAKE) -C $$src_dir	all;	\
	done ;

	mkdir -p build && cd build && \
	${PREFIX}ar xv ${TOPDIR}/${TMP_LIB} && \
	${PREFIX}gcc ${LDFLAGS} *.o -o build.elf && \
	${PREFIX}objcopy -O ihex build.elf build.hex

load:
	nrfprog -f nrf52 --program build/build.hex --sectorerase

debug:
	JLinkGDBServer -device nRF52 -speed 4000 -if SWD -select usb=683388138 -port ${DEBUG_PORT} -RTTTelnetPort 56481

.PHONY: clean

clean:
	rm -rf build/ && rm linker* tmp_lib*
	for src_dir in $(SRC_DIRS) ; do \
		$(MAKE) -C $$src_dir	clean;	\
	done ;


distclean:
	rm .config
