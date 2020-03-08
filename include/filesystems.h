#ifndef __FILESYSTEMS_H
#define __FILESYSTEMS_H

#include <board.h>

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

#ifdef CONFIG_LIBRARY_FATFS
/*
 * fatfs_init - Register the FAT file system with the virtual file system
 *
 *  This method will store the FAT filesystem in a list that will allow
 *  mounting at a later time.
 *
 */
int fatfs_filesystem_register(void);
#endif


#endif /* __FILESYSTEMS_H */
