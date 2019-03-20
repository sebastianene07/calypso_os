#ifndef __VFS_H
#define __VFS_H

#include <list.h>
#include <unistd.h>

/* A device driver it's supposed to registers a new node in the VFS tree */

struct vfs_ops_s {
  int (*open)(const char *pathname, int flags, mode_t mode);
  int (*close)(int fd);
};

enum vfs_node_type {
  VFS_TYPE_DIR,
  VFS_TYPE_FILE,
  VFS_TYPE_DEVICE,
  NUM_VFS_TYPES,
};

/* This represents the node structure for the virtual file system tree */

struct vfs_node_s {
  struct list_head parent_node;
  struct list_head child_node;
  const char *name;
  enum vfs_node_type node_type;
  struct vfs_ops_s *ops;
};

/* The initial VFS mountpoints */

struct vfs_init_mountpoint_s
{
  size_t num_nodes;
  const char *node_name[];
};

int vfs_init(const char *node_name[], size_t num_nodes);

int vfs_register_node(const char *name);

int vfs_unregister_node(const char *name);

struct vfs_init_mountpoint_s *vfs_get_default(void);

#endif /* __VFS_H */
