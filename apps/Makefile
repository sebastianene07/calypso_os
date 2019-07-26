APPS:=

ifeq ($(CONFIG_CONSOLE_APP),y)
APPS:= 'console_app'
endif

all:
	for name in $(APPS); do			\
		make -C $$name all;				\
	done;												\

clean:
	for name in $(APPS); do			\
		make -C $$name clean;		  \
	done;												\
