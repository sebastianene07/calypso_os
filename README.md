This OS embedded project is started from scratch and its purpose is for
learning. I started by targeting the nordic nRF52 board because it has good
documentation and there are multiple examples out there on the web.

## Building

Example for building CATOS for Norfic NrF52 board:

```
make config MACHINE_TYPE=nrf52840
make -j
```

## TODO's

1. Task scheduling, synchronization primitives
2. Driver lower half/upper half separation
3. File system support

## Contents

1. Getting Started
2. Porting Guide (TODO)
3. Tools

## Contributions

Contributions are really welcome. You can email me on: Sebastian Ene <sebastian.ene07@gmail.com>
