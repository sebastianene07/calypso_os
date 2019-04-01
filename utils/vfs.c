#include <vfs.h>
#include <semaphore.h>
#include <errno.h>

/* Virtual file system tree */

static struct vfs_node_s g_root_vfs;

/* VFS mutual exclusion sema */

static sem_t g_vfs_sema;

/* VFS default mountpoints */

static struct vfs_init_mountpoint_s g_vfs_default_mtpt = {
  .node_name = {"dev", "mnt", "bin", "otp", "home"},
  .num_nodes = 5,
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
int vfs_init(const char *node_name[], size_t num_nodes)
{
  /* Verify is we use the default mount points */

  if (node_name == NULL)
  {
    struct vfs_init_mountpoint_s *default_mtpt = vfs_get_default();
    node_name = default_mtpt->node_name;
    num_nodes = default_mtpt->num_nodes;
  }

  sem_init(&g_vfs_sema, 0, 1);

  INIT_LIST_HEAD(&g_root_vfs.parent_node);
  INIT_LIST_HEAD(&g_root_vfs.child_node);

  g_root_vfs.name       = "/";
  g_root_vfs.node_type  = VFS_TYPE_DIR;

  /* Create the VFS child nodes */

  struct vfs_node_s *new_node;

  new_node = calloc(sizeof(struct vfs_node_s), num_nodes);
  if (new_node == NULL)
  {
    return -ENOMEM;
  }

  for (int i = 0; i < num_nodes; ++i)
  {
    INIT_LIST_HEAD(&new_node->child_node);
    new_node->name      = node_name[i];
    new_node->node_type = VFS_TYPE_DIR;

    list_add(&new_node->parent_node, &g_root_vfs.child_node);
  }

  return OK;
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
struct vfs_node_s vfs_get_matching_node(const char *name, size_t name_len)
{
}
