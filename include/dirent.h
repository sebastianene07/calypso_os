#ifndef __DIRENT_H
#define __DIRENT_H

#include <vfs.h>

typedef struct vfs_node_s DIR;

/****************************************************************************
 * Public Function Signatures
 ****************************************************************************/

DIR *opendir(const char *name);

int closedir(DIR *dirp);

#endif /* __DIRENT_H */
