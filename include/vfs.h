#ifndef __VFS_H
#define __VFS_H

#include <list.h>
#if 0 /* TODO add poll */
#include <poll.h>
#endif


/* A device driver comes and registers this node in the VFS tree */

struct vfs_ops_s {
  int (*open)(void);
  int (*read)(size_t nread, void *buffer, size_t buff_len);
  int (*write)(size_t nwrite, const void *buffer);
#if 0 /* TODO add poll */
  int (*poll)(struct poll_fds *poll, size);
  int (*ioctl)();
#endif
  int (*close)(void);
};

/* This represents the node structure for the virtual file system tree */

struct vfs_node_s {
  struct list_head vfs_node;
  const char *name;
  struct vfs_ops_s *ops;
};

#endif /* __VFS_H */
