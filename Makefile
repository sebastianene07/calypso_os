PREFIX := arm-none-eabi-
TOPDIR=$(shell pwd)
DEBUG_PORT=2331

# Include user config
ifeq ($(MACHINE_TYPE),)
include .config
MACHINE_TYPE=$(subst $\",,$(CONFIG_MACHINE_TYPE))
-include Make.defs
CFLAGS=$(subst $\",,$(CONFIG_CFLAGS))
LDFLAGS=$(subst $\",,$(CONFIG_LDFLAGS))
endif

$(info machine_type=$(MACHINE_TYPE))
TARGET=$(MACHINE_TYPE)

# List the directories that contain the MACHINE_TYPE name

SRC_DIRS := $(shell find . -wholename $(MACHINE_TYPE))
SRC_DIRS += sched s_alloc utils apps lib drivers

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
	${PREFIX}gcc *.o -o build.elf ${LDFLAGS} && \
	${PREFIX}objcopy -O ihex build.elf build.hex && \
	${PREFIX}objcopy -O binary build.elf build.bin && \
	echo "Build finished successfully."

create_board_file:
	cp arch/*/$(MACHINE_TYPE)/include/*.h include/.
	echo "#ifndef __BOARD_CFG_H\n#define __BOARD_CFG_H" > include/board_cfg.h
	cat .config | grep -v "^#" | grep -v "^$$" | tail -n +4 | sed 's/^/#define /' | sed 's/=/ /' >> include/board_cfg.h
	echo "#endif /* __BOARD_CFG_H */" >> include/board_cfg.h
	cat .config | grep -v "^#" | grep -v "^$$" | tail -n +4 | sed 's/^/export /' | sed 's/=/ /' > Make.defs

load:
	nrfjprog -f nrf52 --program build/build.hex --sectorerase

config:
	cp config/$(MACHINE_TYPE)/release/defconfig .config
	cat .config | grep -v '^#' | grep -v '^$$' | tail -n +4 | sed 's/^/export /' | sed 's/=/ /' > Make.defs

savedefconfig:
	cp .config defconfig

debug:
	JLinkGDBServer -device nRF52 -speed 4000 -if SWD -port ${DEBUG_PORT}

menuconfig:
	cp arch/*/$(MACHINE_TYPE)/Kconfig include/.
	kconfig-mconf Kconfig

.PHONY: clean debug config load create_board_file distclean menuconfig savedefconfig

clean:
	for src_dir in $(SRC_DIRS) ; do \
		$(MAKE) -C $$src_dir	clean;	\
	done ;
	rm -rf build/ && rm -f tmp_lib*
	rm -f include/Kconfig 2> /dev/null

distclean: clean
	rm -f .config
	rm -f Make.defs
	rm -f include/Kconfig
