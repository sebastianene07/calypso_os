#include <board.h>

#include <errno.h>
#include <filesystems.h>
#include <list.h>
#include <semaphore.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include <vfs.h>

/****************************************************************************
 * Private Variables
 ****************************************************************************/

/* Virtual file system tree */

static struct vfs_node_s g_root_vfs;

/* VFS mutual exclusion sema */

static sem_t g_vfs_sema;

/* VFS default mountpoints */

static struct vfs_init_mountpoint_s g_vfs_default_mtpt = {
  .node_name = {"dev", "mnt", "bin", "otp", "home"},
  .num_nodes = 5,
};

/* Known filesystems list */

struct list_head g_known_filesystems;
static sem_t g_known_fs_sema;

/* Mounted filesystems list */

struct list_head g_mounted_filesystems;
static sem_t g_mounted_fs_sema;

/****************************************************************************
 * Private Functions
 ****************************************************************************/

enum vfs_types_e get_fs_type_from_name(const char *fs_type_name)
{
  if (!strcmp(fs_type_name, "FAT") || !strcmp(fs_type_name, "fat"))
    return VFS_FILESYSTEM_FAT32;
  else if (!strcmp(fs_type_name, "EXFAT") || !strcmp(fs_type_name, "exfat"))
    return VFS_FILESYSTEM_EXFAT;

  return VFS_FILESYSTEM_UNSUPPORTED;
}

/****************************************************************************
 * Public Functions
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
int vfs_init(const char *node_name[], size_t num_nodes)
{
  int ret = OK;

  /* Verify is we use the default mount points */

  if (node_name == NULL) {
    struct vfs_init_mountpoint_s *default_mtpt = vfs_get_default();
    node_name = default_mtpt->node_name;
    num_nodes = default_mtpt->num_nodes;
  }

  /* Initialize the VFS semaphores */

  sem_init(&g_vfs_sema, 0, 1);
  sem_init(&g_known_fs_sema, 0, 1);
  sem_init(&g_mounted_fs_sema, 0, 1);

  INIT_LIST_HEAD(&g_known_filesystems);
  INIT_LIST_HEAD(&g_mounted_filesystems);

  /* The root virtual file system node */

  g_root_vfs.parent     = NULL;
  g_root_vfs.name       = VFS_PATH_DELIM;
  g_root_vfs.node_type  = VFS_TYPE_DIR;

  /* Create the VFS child nodes */

  struct vfs_node_s *new_node;
  INIT_LIST_HEAD(&g_root_vfs.child);
  g_root_vfs.num_children  = num_nodes;

  for (int i = 0; i < num_nodes; ++i) {
    new_node = calloc(1, sizeof(struct vfs_node_s));
    if (new_node == NULL) {
      return -ENOMEM;
    }

    new_node->parent      = &g_root_vfs;
    size_t node_len = strlen(node_name[i]);
    new_node->name        = calloc(node_len + 1, sizeof(char));  
    strncpy((char *)new_node->name, node_name[i], node_len);
    new_node->node_type   = VFS_TYPE_DIR;
    new_node->open_count  = 1;  /* We don't allow the default nodes removal */
    sem_init(&new_node->lock, 0, 0);    

    list_add(&new_node->node_child, &g_root_vfs.child);
    INIT_LIST_HEAD(&new_node->child);
  }

#ifdef CONFIG_LIBRARY_FATFS
  ret = fatfs_filesystem_register();
#endif

  return ret;
}

/*
 * vfs_get_default - get the paths mounted under "/"
 *
 *  The function returns the mounted paths under the root directory.
 *
 */
struct vfs_init_mountpoint_s *vfs_get_default(void)
{
  return &g_vfs_default_mtpt;
}

/*
 * vfs_get_matching_node - get the node by specified path
 *
 * @name      - path name
 * @name_len  - the name length
 *
 *  The function returns the VFS node from the specified name.
 *
 */
struct vfs_node_s *vfs_get_matching_node(const char *name, size_t name_len)
{
  char *olds;

  /* We should do a copy of the path to prevent the string from being
   * changed.
   */

  char *name_copy = calloc(1, strlen(name) + 1);
  memcpy(name_copy, name, strlen(name));

  /* TODO : implement reader-writer with readers priority */
  /* Prevent concurent access to VFS while we iterate through nodes */

  sem_wait(&g_vfs_sema);

  char *ptr_copy = name_copy;
  struct vfs_node_s *current_node = NULL;
  struct vfs_node_s *parent = &g_root_vfs;
  char *node_name = NULL;
  bool is_found;

  if (name_len == 1 && strcmp(name, VFS_PATH_DELIM) == 0) {
    current_node = parent;
    goto free_with_sem;
  }

  do {
    node_name = strtok_r(ptr_copy, VFS_PATH_DELIM, &olds);
    ptr_copy = NULL;

    if (node_name && strlen(node_name) == 0)
      continue;
    else if (node_name == NULL)
      break;


    /* Look at the names of the child nodes and verify which one has the name
     * 'node_name'.
     */

    struct vfs_node_s *node = NULL;
    is_found = false;

    list_for_each_entry(node, &parent->child, node_child) {
      if (!strcmp(node->name, node_name)) {
        is_found = true;
        current_node = node;
        break;
      }
    }

    if (!is_found) {
      current_node = NULL;
      goto free_with_sem;
    }

    parent = current_node;
  } while (node_name != NULL);

free_with_sem:
  sem_post(&g_vfs_sema);
  free(name_copy);

  return current_node;
}

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
                      void *priv)
{
  /* Extract the name */

  bool is_delim_found = false;
  int i;
  for (i = name_len; i >= 0; i--) {
    if (*(name + i) == '/') {
      is_delim_found = true;
      break;
    }
  }

  if (!is_delim_found) {
    /* Bad path name */
    return -EINVAL;
  }

  /* We found the deimiter now we can extract the name */

  char *node_name = (char *)(name + i + 1);
  int copy_name_len = strlen(node_name); 
  char *node_name_copy = calloc(copy_name_len + 1, sizeof(char));
  if (!node_name_copy) {
    return -ENOMEM;
  }
  
  strncpy(node_name_copy, node_name, copy_name_len);

  /* Find the place where we should insert the node */

  struct vfs_node_s *current_node = NULL;
  char *name_copy = NULL;
  
  if (i == 0) {
    
    /* We insert the node at the root path "/" */

    sem_wait(&g_vfs_sema);
    current_node = &g_root_vfs;
    goto free_with_sem;
  }

  name_copy = calloc(1, i);
  if (name_copy == NULL) {
    return -ENOMEM;
  }

  /* Copy the name so that we can tokenize it by following '/'
   */

  memcpy(name_copy, name, i);

  sem_wait(&g_vfs_sema);

  char *ptr_copy = name_copy;
  bool is_found = false;
  struct vfs_node_s *parent = &g_root_vfs;
  char *olds;

  do {
    node_name = strtok_r(ptr_copy, VFS_PATH_DELIM, &olds);
    ptr_copy = NULL;

    if (node_name && strlen(node_name) == 0)
      continue;
    else if (node_name == NULL)
      break;

    struct vfs_node_s *node = NULL;
    is_found = false;
    
    list_for_each_entry(node, &parent->child, node_child) {
      if (!strcmp(node->name, node_name)) {
        is_found = true;
        current_node = node;
        break;
      }
    }

    if (!is_found) {
      current_node = NULL;
      goto free_with_sem;
    }

    parent = current_node;
  } while (node_name != NULL);

free_with_sem:
  sem_post(&g_vfs_sema);
  free(name_copy);

  if (!current_node) {
    return -ENOENT;
  }

  struct vfs_node_s *new_node = calloc(1, sizeof(struct vfs_node_s));
  if (new_node == NULL) {
    return -ENOMEM;
  }

  /* Create and populate the new node */

  new_node->parent       = current_node;
  new_node->num_children = 0;
  new_node->ops          = ops;
  new_node->node_type    = node_type;
  new_node->priv         = priv;
  new_node->name         = node_name_copy;
  new_node->open_count   = 0;

  sem_init(&new_node->lock, 0, 0);

  INIT_LIST_HEAD(&new_node->child);

  list_add(&new_node->node_child, &current_node->child);
  current_node->num_children++;

  return OK;
}

static void vfs_remove_node(struct vfs_node_s *node)
{
  if (node->num_children > 0) {
    struct list_head *it, *temp;
    list_for_each_safe(it, temp, &node->child) {
      struct vfs_node_s *current_node = container_of(it, struct vfs_node_s,
                                                     node_child);
      vfs_remove_node(current_node);
    }
  } 

  if (node->open_count > 0) {
    printf("cannot remove node %s is opened !\n", node->name);
    return;
  }

  printf("remove node %s\n", node->name);

  /* Remove the node from the parent */
  struct vfs_node_s *parent = node->parent;
  parent->num_children--;

  list_del(&node->node_child);

  free((void *)node->name);
  free(node); 
}

/*
 * vfs_unregister_node - remove a node from the virtual file system and his
 *                       children.
 *
 * @name - path name
 * @name_len  - the name length
 *
 */
int vfs_unregister_node(const char *name, size_t name_len)
{
  /* Find the parent node */

  struct vfs_node_s *current_node = vfs_get_matching_node(name, name_len);
  if (current_node == NULL) {
    return -EINVAL;
  }

  sem_wait(&current_node->lock);
  if (current_node->num_children > 0) {
    vfs_remove_node(current_node);
    sem_post(&current_node->lock);
    return OK;
  } 
  sem_post(&current_node->lock);

  struct vfs_node_s *parent = current_node->parent;
  if (parent == NULL) {
    return -EINVAL;
  }

  sem_wait(&parent->lock);
  if (parent->open_count > 0) {
    sem_post(&parent->lock);
    return -ENFILE;
  }

  /* Remove the node from the parent */

  list_del(&current_node->node_child);
  parent->num_children--;

  free((void *)current_node->name);
  sem_post(&parent->lock);

  free(current_node);

  return OK;
}

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
                            filesystem_umount_cb umount_cb)
{
  enum vfs_types_e fs_type = get_fs_type_from_name(type);
  if (fs_type == VFS_FILESYSTEM_UNSUPPORTED) {
    return -EINVAL;
  }

  sem_wait(&g_known_fs_sema);

  struct list_head *it, *temp;
  list_for_each_safe(it, temp, &g_known_filesystems) {
    struct vfs_registration_s *fs = container_of(it, struct vfs_registration_s,
                                                 known_filesystems);
    if (fs_type == fs->fs_type) {

      /* Already registered filesystem type */
      sem_post(&g_known_fs_sema);
      return -EEXIST;
    }
  }

  sem_post(&g_known_fs_sema);

  struct vfs_registration_s *new_fs = calloc(1, sizeof(struct vfs_registration_s));
  if (new_fs == NULL) {
    return -ENOMEM;
  }

  new_fs->mount_cb  = mount_cb;
  new_fs->umount_cb = umount_cb;
  new_fs->file_ops  = file_ops;
  new_fs->fs_type      = fs_type;

  sem_wait(&g_known_fs_sema);
  list_add(&new_fs->known_filesystems, &g_known_filesystems);
  sem_post(&g_known_fs_sema);

  return OK;
}

/*
 * vfs_unregister_filesystem - unregister a new filesystem
 *
 * @type      - the file system typee
 *
 *  The function removes a registered filesystem from the registration list.
 *
 */
int vfs_unregister_filesystem(const char *type)
{
  enum vfs_types_e fs_type = get_fs_type_from_name(type);
  if (fs_type == VFS_FILESYSTEM_UNSUPPORTED) {
    return -EINVAL;
  }

  sem_wait(&g_known_fs_sema);

  struct list_head *it, *temp;
  list_for_each_safe(it, temp, &g_known_filesystems) {
    struct vfs_registration_s *fs = container_of(it, struct vfs_registration_s,
                                                 known_filesystems);
    if (fs_type == fs->fs_type) {

      list_del(it);
      free(fs);

      /* Already registered filesystem type */
      sem_post(&g_known_fs_sema);
      return OK;
    }
  }

  sem_post(&g_known_fs_sema);

  return -EINVAL;
}

/*
 * vfs_get_registered_filesystem - get the registered filesystem from type
 *
 * @type      - the file system typee
 *
 *  The function returns the registered filesystem
 *
 */
struct vfs_registration_s *vfs_get_registered_filesystem(const char *type)
{
  enum vfs_types_e fs_type = get_fs_type_from_name(type);
  if (fs_type == VFS_FILESYSTEM_UNSUPPORTED) {
    return NULL;
  }

  sem_wait(&g_known_fs_sema);

  struct list_head *it, *temp;
  list_for_each_safe(it, temp, &g_known_filesystems) {
    struct vfs_registration_s *fs = container_of(it, struct vfs_registration_s,
                                                 known_filesystems);
    if (fs_type == fs->fs_type) {

      /* Already registered filesystem type */
      sem_post(&g_known_fs_sema);
      return fs;
    }
  }

  sem_post(&g_known_fs_sema);
  return NULL;
}

/*
 * vfs_mount_filesystem - mount a new file system in the specified path
 *
 * @fs          - the registered file system structure that contains file ops
 * @mtd_ops     - the MTD operation structure
 * @mount_path  - the path were we mount the filesystem
 *
 *  The function mounts a file system in the speicifed mount path.
 *
 */
int vfs_mount_filesystem(struct vfs_registration_s *fs,
                         struct mtd_ops_s *mtd_ops,
                         const char *mount_path)
{
  struct vfs_mount_filesystem_s *fs_mount =
    calloc(1, sizeof(struct vfs_mount_filesystem_s));
  if (fs_mount == NULL) {
    return -ENOMEM;
  }

  size_t mnt_path_len = strlen(mount_path);
  char *mount_path_copy = calloc(mnt_path_len + 1, sizeof(char));
  if (mount_path_copy == NULL) {
    free(fs_mount);
    return -ENOMEM;
  }

  strncpy(mount_path_copy, mount_path, mnt_path_len);

  fs_mount->mtd_ops     = mtd_ops;
  fs_mount->mount_path  = mount_path_copy;
  fs_mount->registered_fs = fs;

  /* Call the file system mount function */

  int ret = fs->mount_cb(fs_mount);
  if (ret != OK) {
    free(mount_path_copy);
    free(fs_mount);
    return ret;
  }

  /* Add the mount structure to the list */

  sem_wait(&g_mounted_fs_sema);
  list_add(&fs_mount->mounted_filesystems, &g_mounted_filesystems);
  sem_post(&g_mounted_fs_sema);

  return OK;
}

/*
 * vfs_umount_filesystem - unmount a file systemh
 *
 * @mount_path  - the path were we mount the filesystem
 *
 *  The function unmounts a file system from the mounte path.
 *
 */
int vfs_umount_filesystem(const char *mount_path)
{
  sem_wait(&g_mounted_fs_sema);

  struct vfs_mount_filesystem_s *fs;
  struct list_head *it, *temp;

  list_for_each_safe(it, temp, &g_mounted_filesystems) {
    fs = container_of(it, struct vfs_mount_filesystem_s, mounted_filesystems);
    if (!strcmp(fs->mount_path, mount_path)) {

      fs->registered_fs->umount_cb(mount_path);
      vfs_unregister_node(mount_path, strlen(mount_path));

      list_del(it);
      free((void *)fs->mount_path);
      free(fs);

      sem_post(&g_mounted_fs_sema);
      return OK;
    }
  }

  sem_post(&g_mounted_fs_sema);

  return -EINVAL;
}
