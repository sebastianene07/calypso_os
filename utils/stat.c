#include <assert.h>
#include <board.h>
#include <errno.h>
#include <string.h>
#include <vfs.h>
#include <scheduler.h>

/*
 * open - search for an entry specified by pathname and open it
 *
 * @pathname - the path to a file/device
 * @flags    - open flags
 *
 *  Open a node and save the information in the current TCB.
 *
 */
int open(const char *pathname, int flags, ...)
{
  /* 1. Look through VFS and find the node identified by pathname */

  size_t name_len = strlen(pathname);
  struct vfs_node_s *node = vfs_get_matching_node(pathname,
                                                  name_len);
  if (node == NULL) {
    return -ENOENT;
  }

  /* Grab an entry from the tcb FILE structure. */

  struct opened_resource_s *res =
    sched_allocate_resource(node->priv, node->ops, 0);
  if (res == NULL) {

    /* Close the object before returining error */

    if (node->ops->close) {
      node->ops->close(node->priv);
    }

    return -ENFILE;
  }

  /* Call the vfs open method */

  int ret = -ENODEV;
  if (node->ops != NULL && node->ops->open != NULL) {
    ret = node->ops->open(node->priv, pathname, flags, 0);
  }

  if (ret < 0) {
    return ret;
  }

  return res->fd;
}

int ioctl(int fd, unsigned long request, unsigned long arg)
{
  int ret = -EINVAL;
  disable_int();

  struct tcb_s *curr_tcb = sched_get_current_task();
  assert(curr_tcb->curr_resource_opened >= 0);

  /* Iterate over opened resource list find the fd and free the resource */

  struct opened_resource_s *resource = sched_find_opened_resource(fd);
  if (resource->ops->ioctl) {
    ret = resource->ops->ioctl(resource->priv, request, arg);
  }

  enable_int();
  return ret;
}
