PATH_FATFS = ./fatfs/source
SRCS_FATFS := $(wildcard $(PATH_FATFS)/*.c)
OBJS_FATFS := $(patsubst %.c, %.o, $(SRCS_FATFS))

FATFS_FLAGS := ${CFLAGS}																					\
							-I$(TOPDIR)/include																	\
							-I$(TOPDIR)/s_alloc																	\
							-I$(TOPDIR)/sched/include														\

all: fatfs

fatfs: $(OBJS_FATFS)
	@echo "Building FATFS library " $(OBJS_FATFS)
	${PREFIX}ar -rc $(TOPDIR)/$(TMP_LIB) $(OBJS_FATFS)

%.o : %.c
	${PREFIX}gcc $(FATFS_FLAGS) -c $< -o $@

clean:
	find . -iname "*.o" -exec rm -f {} \;

.PHONY: fatfs clean all
