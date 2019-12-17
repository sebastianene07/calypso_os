SRC_DIRS := display sensors serial storage

all:
	for name in $(SRC_DIRS); do			\
		make -C $$name all;			\
	done;						\

clean:
	for name in $(SRC_DIRS); do			\
		make -C $$name clean;		 	\
	done;						\
