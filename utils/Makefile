SRC := $(wildcard *.c)
OBJS += $(patsubst %.c,%.o,$(SRC))
MY_FLAGS:=${CFLAGS} -I$(TOPDIR)/s_alloc/

all:
	echo "Building utils dir"
	${PREFIX}ar -rc $(TOPDIR)/$(TMP_LIB) $(OBJS)

%.o : %.c
	${PREFIX}gcc $(ARCH_FLAGS) -c $< -o $@

clean:
	find . -iname "*.o" -exec rm -f {} \;
