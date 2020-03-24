#include <assert.h>
#include <board.h>
#include <errno.h>
#include <string.h>
#include <vfs.h>
#include <scheduler.h>

/*
 * create_vfs_node - create a new entry in the virtual file system with the
 *                   specified path
 *
 * @path_without_name - the path without the filename
 * @path              - the complete path with the filenamee
 * @path_len          - the length of the full path
 **
 */
static struct vfs_node_s *create_vfs_node(const char *path_without_name,
                                          const char *path,
                                          size_t path_len)
{
  struct vfs_node_s *node;
  struct vfs_ops_s *ops;
  int ret;

  ops = vfs_get_supported_operations(path_without_name);
  ret = vfs_register_node(path,
                          path_len,
                          ops,
                          VFS_TYPE_FILE,
                          NULL);
  if (ret != OK) {
    return NULL;
  }

  node = vfs_get_matching_node(path, path_len);
  return node;
}

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

  size_t name_len = strlen(pathname);
  if (name_len == 0)
    return -EINVAL;

  int ret = OK;
  char *path_without_name;
  int i = name_len - 1;

  /* If the name of the open path ends in '/' ex : /mnt/my_file/
   * move the index i to:                      --------|
   * to extract the parent.
   */

  if (pathname[name_len - 1] == '/') {
    while (i > 0 && pathname[i] == '/') { i--; }
  }

  while (i > 0 && pathname[i] != '/') { i--; }

  /* Allocate memory for the path without the filename */

  path_without_name = calloc(1, i + 2);
  if (path_without_name == NULL)
    return -ENOMEM;

  strncpy(path_without_name, pathname, i + 1);

  /* Look through VFS and find the node identified by pathname */

  if (flags & O_CREATE) {
    node = create_vfs_node(path_without_name, pathname, name_len);
  } else {

    /* If the user process requested O_APPEND bu there is no file
     * we should create a new file.
     */

    node = vfs_get_matching_node(pathname, name_len);
    if (node == NULL && (flags & O_APPEND)) {
      node = create_vfs_node(path_without_name, pathname, name_len);
    }
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

  sem_wait(&node->lock);

  /* Call the vfs open method */

  if (node->ops != NULL && node->ops->open != NULL) {
    ret = node->ops->open(res, pathname, flags, 0);
  }

  if (ret < 0) {
    sched_free_resource(res->fd);
    sem_post(&node->lock);
    goto free_with_path;
  }

  ret = res->fd;
  node->open_count += 1;

  sem_post(&node->lock);

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

/*
 * mount - mounts a file system in the specified patht
 *
 * @type  - the path to a file/device
 * @dir    - open flags
 * @flags  - ignored
 * @data   - The MTD device path
 *
 * Mount a filesystem in the VFS at the specified path.
 *
 */
int mount(const char *type, const char *dir, int flags, void *data)
{
  const char *mtd_dev_path = (const char *)data;

  /* Check if we have a registered file system that supports 'type' */

  struct vfs_registration_s *fs = vfs_get_registered_filesystem(type);
  if (fs == NULL) {
    return -EOPNOTSUPP;
  }

  /* Get the MTD ops from the device */

  int fd = open(mtd_dev_path, 0);
  if (fd < 0) {
    return -ENODEV;
  }

  struct mtd_ops_s *mtd_ops = NULL;
  int ret = ioctl(fd, MTD_GET_OPS, (unsigned long)&mtd_ops);
  if (ret < 0) {
    close(fd);
    return -EINVAL;
  }

  close(fd);

  /* Create a new mount structure and store it in the VFS */

  return vfs_mount_filesystem(fs, mtd_ops, dir);
}

/*
 * umount - mounts a file system in the specified path
 *
 * @type   - the path to a file/device
 * @dir    - open flags
 * @flags  - ignored
 * @data   - The MTD device path
 *
 * Mount a filesystem in the VFS at the specified path.
 *
 */
int umount(const char *dir, int flags)
{
  return vfs_umount_filesystem(dir);
}
