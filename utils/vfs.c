#include <vfs.h>
#include <semaphore.h>

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
int vfs_init(void)
{
  sem_init(&g_vfs_sema, 0, 1);

  g_root_vfs.name       = "/";
  g_root_vfs.node_type  = VFS_TYPE_DIR;
}
