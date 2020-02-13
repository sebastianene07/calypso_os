## Calypso OS on versatilepb

Currently I implemented a minimal support for this config: only the console
uart is working. More work has to be added to have a basic shell.

To test this :

qemu-system-arm -M versatilepb -m 128M -nographic -kernel build/build.bin

You can also attach a gdb debugger to it with "-s". The "-S" argument 
allows qemu to wait untill a debugger is attached.
