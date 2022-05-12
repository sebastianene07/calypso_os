### Calypso OS on Raspberry Pi Pico board ###

## Memory layout

The board is expected to have the second stage bootloader flashed:
between 0x10000000 and 0x10000100. The second stage bootloader can be flashed
on the RPIPico by uploading an example image from pico-sdk.

I have not included th second stage bootloader in this repo.

## Flashing the board

Flashing the board can be done in two ways:

1) using drag-ndrop when the RpiPico connects in mass storage mode
2) flash using JLINK SWD

JLinkGDBServer -device RP2040_M0_0 -speed 4000 -if SWD -port 2331
