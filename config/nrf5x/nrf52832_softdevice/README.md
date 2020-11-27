Flash the soft device first before flashing the Calypso OS.

## How to do this ?

	nrfjprog --eraseall	
	nrfjprog -f nrf52 --program s132_nrf52_1.0.0-3.alpha_softdevice.hex	
