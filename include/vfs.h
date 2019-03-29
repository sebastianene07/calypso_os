#ifndef __VFS_H
#define __VFS_H

#include <list.h>
#include <unistd.h>

/* A device driver it's supposed to registers a new node in the VFS tree */

struct vfs_ops_s {
  int (*open)(const char *pathname, int flags, mode_t mode);
  int (*close)(int fd);
};

/* Type of the nodes */

enum vfs_node_type {
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

/*
 * vfs_init - initialize the root nodes
 *
 * @node_name - pointer to an array of names
 * @num_nodes - the length of the array
 *
 *  This function initializes the virtual file system
 *  and creates the initial nodes. If NULL is specified
 *  the function will use the default mountpoints.
 *
 */
int vfs_init(const char *node_name[], size_t num_nodes);

/*
 * vfs_register_node - register a node with the virual file system
 *
 * @name      - path name
 * @name_len  - the name length
 * @ops       - supported node operations
 * @node_type - the type of the node
 *
 *  The function creates a new node entry in the virtual file system.
 *  Returns OK if the node is registered -ENOMEM if we run out of memory and
 *  we can't register the node, -EINVAL if the given arguments are wrong.
 */
int vfs_register_node(const char *name,
                      size_t name_len,
                      struct vfs_ops_s *ops,
                      enum vfs_node_type node_type);

/*
 * vfs_unregister_node - remove a node from the virtual file system
 *
 * @name - path name
 * @name_len  - the name length
 *
 *  The function creates a new node entry in the virtual file system.
 *  Returns OK if we managed to unregister the node from the VFS, -EINVAL if
 *  the supplied arguments are wrong or -ENOENT if there is no registered node
 *  with "name".
 *
 */
int vfs_unregister_node(const char *name, size_t name_len);

/*
 * vfs_get_matching_node - get the node by specified path
 *
 * @name      - path name
 * @name_len  - the name length
 *
 *  The function returns the VFS node from the specified name.
 *
 */
struct vfs_node_s vfs_get_matching_node(const char *name, size_t name_len);

/*
 * vfs_get_default - get the paths mounted under "/"
 *
 *  The function returns the mounted paths under the root directory.
 *
 */
struct vfs_init_mountpoint_s *vfs_get_default(void);

#endif /* __VFS_H */
