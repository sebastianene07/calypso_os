SRC:=$(wildcard *.c)
OBJS:=$(patsubst %.c,%.o,$(SRC))
UTILS_FLAGS := ${CFLAGS} -I$(TOPDIR)/include -I$(TOPDIR)/s_alloc -I$(TOPDIR)/sched/include

all: $(OBJS)
	${PREFIX}ar -rc $(TOPDIR)/$(TMP_LIB) $(OBJS)

%.o : %.c
	${PREFIX}gcc $(UTILS_FLAGS) -c $< -o $@

clean:
	find . -iname "*.o" -exec rm -f {} \;
