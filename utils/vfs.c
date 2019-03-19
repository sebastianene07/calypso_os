#include <vfs.h>
#include <semaphore.h>
#include <errno.h>

/* Virtual file system tree */

static struct vfs_node_s g_root_vfs;

/* VFS mutual exclusion sema */

static sem_t g_vfs_sema;

/*
 * vfs_get_ops - get the node operations
 *
 *
 */
static struct vfs_ops_s *vfs_get_ops(enum vfs_node_type node_type)
{ /* TODO */
  return NULL;
}

/*
 * vfs_init - initialize the root nodes
 *
 *  This function initializes the virtual file system
 *  and creates the nodes: /, /dev, /mnt, /var, /bin
 *
 */
int vfs_init(const char *node_name[], size_t num_nodes)
{
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
