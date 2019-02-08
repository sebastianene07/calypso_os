SRC := $(wildcard *.c)
SRC += $(wildcard */*.c)
OBJS += $(patsubst %.c,%.o,$(SRC))
ARCH_FLAGS:=${CFLAGS} -I$(TOPDIR)/include -I$(TOPDIR)/s_alloc -I./include/

all: $(OBJS)
	echo "Building arch specific"
	${PREFIX}ar -rc $(TOPDIR)/$(TMP_LIB) $(OBJS)

%.o : %.c
	${PREFIX}gcc $(ARCH_FLAGS) -c $< -o $@

clean:
	find . -iname "*.o" -exec rm -f {} \;
