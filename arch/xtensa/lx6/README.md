### Calypso OS on ESP32 boards ###

1. Build preparation:

Running on Linux:
- download the xtensa toolchain from https://dl.espressif.com/dl/xtensa-esp32-elf-gcc8_2_0-esp-2019r2-linux-amd64.tar.gz

Export the path where you downloaded the toolchain:

:/$> echo "export PATH=$PATH:$(YourDownloadPath)/xtensa-esp32-elf/bin"
:/$> source ~/.bashrc

Go to to calypso OS folder. Configure the build for the ESP32 board:
:/$> make config MACHINE_TYPE=xtensa/esp32dev

Export the PREFIX environment variable OR
:/$> export PREFIX=xtensa-esp32-elf-

build with:
:/$> make PREFIX=xtensa-esp32-elf-
