SRC:=$(wildcard *.c)
OBJS:=$(patsubst %.c,%.o,$(SRC))
SCHED_FLAGS := ${CFLAGS} -I$(TOPDIR)/include -I$(TOPDIR)/s_alloc

all: $(OBJS)
	echo tests
	${PREFIX}ar -rc $(TOPDIR)/$(TMP_LIB) $(OBJS)

%.o : %.c
	${PREFIX}gcc $(SCHED_FLAGS) -c $< -o $@

clean:
	find . -iname "*.o" -exec rm -f {} \;
