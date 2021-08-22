SRC := board_initialize.c
OBJS += $(patsubst %.c,%.o,$(SRC))
OBJS += bs2_default.o
ARCH_FLAGS:=${CFLAGS} -I$(TOPDIR)/include -I$(TOPDIR)/s_alloc -I./include/ -I$(TOPDIR)/sched/include
LDFLAGS += -Llibbsp.a

all: $(OBJS) libbsp.a
	echo "Building arch specific"
	${PREFIX}ar -rc $(TOPDIR)/$(TMP_LIB) $(OBJS) libbsp.a

%.o : %.c
	${PREFIX}gcc $(ARCH_FLAGS) -c $< -o $@

bs2_default.o: bs2_default.s
	

bs2_default.s: bs2_default.bin
	${PREFIX}as $(ASM_FLAGS) -c $@ -o bs2_default.o

bs2_default.bin: boot/bs2_default.elf
	${PREFIX}objcopy -I elf32-littlearm -O binary boot/bs2_default.elf bs2_default.bin --rename-section .text=.boot2
	python3 pico-sdk/src/rp2_common/boot_stage2/pad_checksum -s 0xffffffff $@ bs2_default.s

boot/bs2_default.elf: libbsp.a

libbsp.a:
	mkdir build && cd build && cmake .. && make -j4 && cp libbsp.a ..
	rm -rf build

clean:
	rm -rf build
	rm -f libbsp.a bs2_default.bin bs2_default.s
	find . -iname "*.o" -exec rm -f {} \;
