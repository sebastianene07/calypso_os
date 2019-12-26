all:
	@echo "Building BSEC library"
	rm -rf _obj
	mkdir _obj && cd _obj && ${PREFIX}ar -x ../bin/gcc/Cortex_M4/libalgobsec.a
	${PREFIX}ar -rc $(TOPDIR)/$(TMP_LIB)  _obj/*.o
clean:

.PHONY: all clean
