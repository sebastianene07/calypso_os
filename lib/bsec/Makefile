
ifeq ($(CONFIG_HARDWARE_FP),y)
BSEC_ALGO_PATH := ../bin/gcc/Cortex_M4F/libalgobsec.a
else
BSEC_ALGO_PATH := ../bin/gcc/Cortex_M4/libalgobsec.a
endif

all:
	@echo "Building BSEC library"
	rm -rf _obj
	mkdir _obj && cd _obj && ${PREFIX}ar -x ${BSEC_ALGO_PATH} 
	${PREFIX}ar -rc $(TOPDIR)/$(TMP_LIB)  _obj/*.o
clean:

.PHONY: all clean
