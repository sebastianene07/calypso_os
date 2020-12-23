#ifndef __VFS_H
#define __VFS_H

#include <board.h>

#include <list.h>
#include <mtd.h>
#include <scheduler.h>
#include <unistd.h>

/* The max path length in bytes */

#define VFS_MAX_PATH_LEN            (80)

/* Path delimitator */

#define VFS_PATH_DELIM              "/"

/****************************************************************************
 * Public Types
 ****************************************************************************/

/* Supported node opeartions - this should be implemented by the driver nodes
 */

typedef int (*open_cb)(struct opened_resource_s *priv, const char *pathname,
                       int flags,
                       mode_t mode);
typedef int (*close_cb)(struct opened_resource_s *priv);

/* 
 * write_cb - this callback writes data specified from buf to the devicee
 *
 * Input Arguments:
 *  priv      - an opened virtual file system node for a task
 *  buf       - buffer from where we write the dataa
 *  count     - the number of bytes that we want to writer
 *
 * Return Values:
 *  On success it returns the number of bytes written. On error it returns a
 *  negative error code. This function blocks the task until the bytes are
 *  written.
 */
typedef int (*write_cb)(struct opened_resource_s *priv, const void *buf,
                        size_t count);

/* 
 * read_cb - this callback returns data read from the devicee
 *
 * Input Arguments:
 *  priv      - an opened virtual file system node for a task
 *  buf       - buffer where we need to place the read data
 *  count     - the number of bytes that we want to place in the buffer
 *
 * Return Values:
 *  On success it returns the number of bytes read. On error it returns a
 *  negative error code. This function blocks the task until the bytes are
 *  available.
 */
typedef int (*read_cb)(struct opened_resource_s *priv, void *buf, size_t count);

/* 
 * ioctl_cb - this callback provides special access functions for a device
 *
 * Input Arguments:
 *  priv      - an opened virtual file system node for a task
 *  request   - the request code
 *  arg       - arguments to the deviced (usually this holds an address)
 *
 * Return Values:
 *  On success (0) is returned otherwise a negative error code.
 */
typedef int (*ioctl_cb)(struct opened_resource_s *priv, unsigned long request,
                        unsigned long arg);

/* 
 * unlink_cb - remove a node from the disk
 *
 * Input Arguments:
 *  pathname - the path that needs to be invalidated
 *
 * Return Values:
 *  On success (0) is returned otherwise a negative error code.
 */
typedef int (*unlink_cb)(const char *pathname);

/* 
 * mkdir_cb - create a new directory on the disk
 *
 * Input Arguments:
 *  pathname - the new directory path
 *  mode     - the mode operations
 *
 * Return Values:
 *  On success (0) is returned otherwise a negative error code.
 */
typedef int (*mkdir_cb)(const char *path, mode_t mode);

/* 
 * poll_cb - poll the device for I/O events
 *
 * Input Arguments:
 *  priv      - an opened virtual file system node for a task
 *  poll_sema - the poll semaphore
 *  revents   - (out) the receive mask which is set when an event is generated
 *              in the driver code.
 *
 * Return Values:
 *  On success (0) is returned otherwise a negative error code.
 */
typedef int (*poll_cb)(struct opened_resource_s *priv, sem_t *poll_sema,
                       short *revents);

/* Generic open/read/write/ioctl/close opeartion structure for a node in the
 * Virtual file system.
 */

struct vfs_ops_s {
  open_cb open;
  close_cb close;
  write_cb write;
  read_cb read;
  ioctl_cb ioctl;
  unlink_cb unlink;
  mkdir_cb mkdir;
  poll_cb poll;
};

/* Type of the nodes */

enum vfs_node_type {
  VFS_TYPE_UNAVAILABLE,
  VFS_TYPE_FILE,
  VFS_TYPE_DIR,
  VFS_TYPE_CHAR_DEVICE,
  VFS_TYPE_BLOCK_DEVICE,
  NUM_VFS_TYPES,
};

/* The supported filesystem types */

enum vfs_types_e {
  VFS_FILESYSTEM_FAT32,             /* FAT32 / fat32 */
  VFS_FILESYSTEM_EXFAT,             /* EXFAT / exfat */

  VFS_FILESYSTEM_UNSUPPORTED,       /* NOT SUPPORTED FILESYSTEM */
};

/* This represents the node structure for the virtual file system tree */

struct vfs_node_s {
  struct list_head child;      /* Children nodes for this leaf */
  struct list_head node_child; /* Parent's child list */
  struct vfs_node_s *parent;  /* The parent node */
  unsigned int num_children;  /* Number of children */
  const char *name;           /* The node name is the same with the filename */
  enum vfs_node_type node_type;   /* The type of the node      */
  struct vfs_ops_s *ops;          /* Supported node operations */
  uint8_t open_count;             /* Number of opened structures */
  sem_t lock;                     /* Node lock */

  void *priv;                 /* Private data stored in the node */
};

/* The initial VFS mountpoints */

struct vfs_init_mountpoint_s {
  size_t num_nodes;
  const char *node_name[];
};

/* forward declaration */

struct vfs_mount_filesystem_s;

/* The callback used to mount/unmount a filesystem - These should be
 * implemented by the filesystem and should be provided during
 * registration with vfs_register_filesystem
 * */

typedef int (*filesystem_mount_cb)(struct vfs_mount_filesystem_s *mount);

typedef int (*filesystem_umount_cb)(const char *mount_path);

/* Filesystem registration structure */

struct vfs_registration_s {
  struct list_head known_filesystems; /* We keep the known filesystems in a
                                       * list */

  enum vfs_types_e fs_type;           /* The file system type */
  struct vfs_ops_s *file_ops;         /* File structure operation */

  filesystem_mount_cb mount_cb;
  filesystem_umount_cb umount_cb;
};

/* A structure where we store information about the mounted file system */

struct vfs_mount_filesystem_s {
  struct list_head mounted_filesystems;     /* The list of mounted FS */
  struct vfs_registration_s *registered_fs; /* Pointer to registration struct */
  const char *mount_path;                   /* The path where we mount the FS */
  struct mtd_ops_s *mtd_ops;                /* MTD / block dev operations */
  void *priv;
};

/****************************************************************************
 * Public Function Definitions
 ****************************************************************************/

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
                      enum vfs_node_type node_type,
                      void *priv);

/*
 * vfs_unregister_node - remove a node from the virtual file system
 *
 * @name - path name
 * @name_len  - the name length
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
struct vfs_node_s *vfs_get_matching_node(const char *name, size_t name_len);

/*
 * vfs_get_default - get the paths mounted under "/"
 *
 *  The function returns the mounted paths under the root directory.
 *
 */
struct vfs_init_mountpoint_s *vfs_get_default(void);

/*
 * vfs_register_filesystem - register a new filesystem
 *
 * @type      - the file system typee
 * @file_ops  - file operation structure
 * @mount_cb  - the mount filesystem callback
 * @umount_cb - the unmount callback
 *
 *  The function registers a new file system that can be mounted later
 *  using the mount() function call. This function should be called
 *  by the filesystem driver.
 */
int vfs_register_filesystem(const char *type,
                            struct vfs_ops_s *file_ops,
                            filesystem_mount_cb mount_cb,
                            filesystem_umount_cb umount_cb);
/*
 * vfs_unregister_filesystem - unregister a new filesystem
 *
 * @type      - the file system typee
 *
 *  The function removes a registered filesystem from the registration list.
 *
 */
int vfs_unregister_filesystem(const char *type);

/*
 * vfs_get_registered_filesystem - get the registered filesystem from type
 *
 * @type      - the file system typee
 *
 *  The function returns the registered filesystem
 *
 */
struct vfs_registration_s *vfs_get_registered_filesystem(const char *type);

/*
 * vfs_mount_filesystem - mount a new file system in the specified path
 *
 * @file_ops - the file operation structure
 * @mtd_ops  - the MTD operation structure
 * @mount_path  - the path were we mount the filesystem
 *
 *  The function mounts a file system in the speicifed mount path.
 *
 */
int vfs_mount_filesystem(struct vfs_registration_s *file_ops,
                         struct mtd_ops_s *mtd_ops,
                         const char *mount_path);

/*
 * vfs_umount_filesystem - unmount a file systemh
 *
 * @mount_path  - the path were we mount the filesystem
 *
 *  The function unmounts a file system from the mounte path.
 *
 */
int vfs_umount_filesystem(const char *mount_path);

/*
 * vfs_get_supported_operations - get supported operations for a path
 *
 * @path - the path of the mounted filesystem
 *
 *  The function retrieves the ops for a mounted filesystem.
 */
struct vfs_ops_s *vfs_get_supported_operations(const char *path);

#endif /* __VFS_H */
