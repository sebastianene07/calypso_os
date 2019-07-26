SRC_DIRS := $(shell find . -type d | tail -n +2)

all:
	for name in $(SRC_DIRS); do			\
		make -C $$name all;				\
	done;												\

clean:
	for name in $(SRC_DIRS); do			\
		make -C $$name clean;		  \
	done;												\
