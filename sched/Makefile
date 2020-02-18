SRC:=$(wildcard *.c)
OBJS:=$(patsubst %.c,%.o,$(SRC))
SCHED_FLAGS := ${CFLAGS}																					\
							-I$(TOPDIR)/include																	\
						  -I$(TOPDIR)/include/chip/														\
							-I$(TOPDIR)/s_alloc																	\
							-I$(TOPDIR)/sched/include														\
							-I$(TOPDIR)/apps/

SRC_ASM:=$(wildcard *.s)
OBJS_ASM:=$(patsubst %.s,%.o,$(SRC_ASM))

all: $(OBJS) $(OBJS_ASM)
	${PREFIX}ar -rc $(TOPDIR)/$(TMP_LIB) $(OBJS) $(OBJS_ASM)

%.o : %.c
	${PREFIX}gcc $(SCHED_FLAGS) -c $< -o $@

%.o : %.s
	${PREFIX}as $(SCHED_FLAGS) -c $< -o $@

clean:
	find . -iname "*.o" -exec rm -f {} \;
