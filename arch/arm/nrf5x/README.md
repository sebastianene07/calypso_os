### Calypso OS on nordic boards ###

This board has support for SPI, UART with/without DMA. Full pre-emption support
is available on System tick interrupt.

Available drivers for nrf5X family:
  (-) SPI with DMA  (up to 2 SPI interfaces)
  (-) UART with and without DMA (nrf52840 has 2 uarts available)
  (-) RTC support
  (-) timer suport
  (-) full pre-emption support on Sys Tick

To test this:

make config MACHINE_TYPE=nrf5x/nrf52840
make -j         # build in parallel with -j$(NR_CORES)
make load       # to upload the build on the board

There are multipple configs available for this familiy:

nrf5x/nrf52832            - with or without soft device library from nordic
nrf5x/nrf52832_softdevice
nrf5x/nrf52840
