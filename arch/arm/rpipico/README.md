### Calypso OS on Raspberry Pi Pico board ###

## Flashing the board

Flashing the board can be done in two ways:

1) using drag-ndrop when the RpiPico connects in mass storage mode
2) flash using JLINK SWD

JLinkGDBServer -device RP2040_M0_0 -speed 4000 -if SWD -port 2331
