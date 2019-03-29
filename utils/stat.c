#include <board.h>
#include <errno.h>

/*
 * open - search for an entry specified by pathname and open it
 *
 * @pathname - the path to a file/device
 * @flags    - open flags
 *
 *  Open a node and save the information in the current TCB.
 *
 */
int open(const char *pathname, int flags)
{
  /* 1. Look through VFS and find the node identified by pathname */

  struct vfs_init_mountpoint_s *node = vfs_get_matching_node(pathname);
  if (node == NULL)
  {
    return -ENOENT;
  }

  /* 2. Allocate a new file_structure in the task TCB and populate it */

  /* 3. Call the vfs open method */

  return -ENOSYS;
}
