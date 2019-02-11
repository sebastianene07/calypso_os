PREFIX := arm-none-eabi-
TOPDIR=$(shell pwd)
DEBUG_PORT=2771

# Include user config

include .config
$(info machine_type=$(MACHINE_TYPE))
TARGET=$(MACHINE_TYPE)

# List the directories that contain the MACHINE_TYPE name

SRC_DIRS := $(shell find . -iname $(MACHINE_TYPE))
SRC_DIRS += sched s_alloc utils

# This is the archive where we will bundle the object files

TMP_LIB=tmp_lib.a

# Export varios variables that will be used across Makefiles

export CFLAGS
export PREFIX
export TOPDIR
export TMP_LIB
export TARGET

all: create_board_file
	for src_dir in $(SRC_DIRS) ; do \
  	$(MAKE) -C $$src_dir	all;\
	done ;

	mkdir -p build && cd build && \
	${PREFIX}ar xv ${TOPDIR}/${TMP_LIB} && \
	${PREFIX}gcc ${LDFLAGS} *.o -o build.elf && \
	${PREFIX}objcopy -O ihex build.elf build.hex

create_board_file:
	cp arch/*/$(MACHINE_TYPE)/include/board.h include/.

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
