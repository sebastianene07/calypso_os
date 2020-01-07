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
  struct vfs_node_s *node = NULL;

  /* 1. Look through VFS and find the node identified by pathname */

  size_t name_len = strlen(pathname);
  if (name_len == 0)
    return -EINVAL;

  int ret;
  char *path_without_name;
  int i = name_len - 1;

  if (pathname[name_len - 1] == '/') {
    while (pathname[i] == '/') { i--; }
  }

  while (pathname[i] != '/') { i--; }
  path_without_name = calloc(1, i + 1);
  if (path_without_name == NULL)
    return -ENOMEM;

  strncpy(path_without_name, pathname, i);

  if (flags & O_CREATE) {

    struct vfs_node_s *parent_node = vfs_get_matching_node(path_without_name,
      strlen(path_without_name));

    ret = vfs_register_node(pathname,
                            name_len,
                            parent_node->ops,
                            VFS_TYPE_FILE,
                            NULL);
    node = vfs_get_matching_node(pathname, name_len);
  } else if (flags & O_APPEND) {
    node = vfs_get_matching_node(pathname, name_len);
    if (node == NULL) {
      /* Create a new one */

      struct vfs_node_s *parent_node = vfs_get_matching_node(path_without_name,
        strlen(path_without_name));

      ret = vfs_register_node(pathname,
                              name_len,
                              parent_node->ops,
                              VFS_TYPE_FILE,
                              NULL);
      node = vfs_get_matching_node(pathname, name_len);
    }
  } else {
    node = vfs_get_matching_node(pathname, name_len);
  }

  if (node == NULL) {
    ret = -ENOENT;
    goto free_with_path;
  }

  /* Grab an entry from the tcb FILE structure. */

  struct opened_resource_s *res =
    sched_allocate_resource(node, 0);
  if (res == NULL) {
    ret = -ENFILE;
    goto free_with_path;
  }

  /* Call the vfs open method */

  if (node->ops != NULL && node->ops->open != NULL) {
    ret = node->ops->open(res, pathname, flags, 0);
  }

  if (ret < 0) {
    sched_free_resource(res->fd);
    goto free_with_path;
  }

  ret = res->fd;

free_with_path:
  free(path_without_name);
  return ret;
}

int ioctl(int fd, unsigned long request, unsigned long arg)
{
  int ret = -EINVAL;
  disable_int();

  struct tcb_s *curr_tcb = sched_get_current_task();
  assert(curr_tcb->curr_resource_opened >= 0);

  /* Iterate over opened resource list find the fd and free the resource */

  struct opened_resource_s *resource = sched_find_opened_resource(fd);
  if (resource->vfs_node == NULL) {
    goto cancel_ioctl;
  }

  struct vfs_node_s *vfs_node = resource->vfs_node;
  if (vfs_node->ops->ioctl) {
    ret = vfs_node->ops->ioctl(resource, request, arg);
  }

cancel_ioctl:
  enable_int();
  return ret;
}
