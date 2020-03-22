#ifndef __SIMULATED_FLASH_H
#define __SIMULATED_FLASH_H

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/*
 * sim_flash_init - Initialize and register the simulated flash node.
 *
 *  This function initializes the simulated flash and mounts the node
 *  in the virtual file system.
 */
int sim_flash_init(void);

#endif /* __SIMULATED_FLASH_H */
