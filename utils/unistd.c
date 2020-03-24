#include <board.h>

#include <errno.h>
#include <scheduler.h>
#include <string.h>
#include <vfs.h>
#include <unistd.h>

/****************************************************************************
 * Private Inline Functions
 ****************************************************************************/

/**************************************************************************
 * Name:
 *  unlink_node
 *
 * Description:
 *  Remove a node from the VFS and from the media.
 *
 * Input Parameters:
 *  path - the full path
 *  node - the node that we want to removefrom the VFS
 *
 * Return Value:
 *  OK(0) or a negative value in case of error.
 *
 *************************************************************************/
static inline int unlink_node(const char *path, struct vfs_node_s *node)
{
  int ret = vfs_unregister_node(path, strlen(path));
  if (ret == OK) {

    /* If this call fails the file is removed from the VFS but not from the
     * disk. Is still can be accessible after the file-system is remounted.
     */

    ret = node->ops->unlink(path);
  }

  return ret;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/**************************************************************************
 * Name:
 *  read
 *
 * Description:
 *  Read up to count bytes in the buffer pointed by buf, from an opened resource
 *  identified by fd.
 *
 * Return Value:
 *  The number of bytes read or a negative value in case of error.
 *
 *************************************************************************/
ssize_t read(int fd, void *buf, size_t count)
{
  disable_int();
  struct opened_resource_s *res = sched_find_opened_resource(fd);
  enable_int();

  if (res == NULL) {
    return -EINVAL;
  }

  if (!res->vfs_node->ops || !res->vfs_node->ops->read) {
    return -ENOSYS;
  }

  return res->vfs_node->ops->read(res, buf, count);
}

/**************************************************************************
 * Name:
 *  write
 *
 * Description:
 *  Write up to count bytes from the buffer pointed by buf, from an opened resource
 *  identified by fd.
 *
 * Return Value:
 *  The number of bytes read or a negative value in case of error.
 *
 *************************************************************************/
ssize_t write(int fd, void *buf, size_t count)
{
  disable_int();
  struct opened_resource_s *res = sched_find_opened_resource(fd);
  enable_int();

  if (res == NULL) {
    return -EINVAL;
  }

  if (!res->vfs_node->ops || !res->vfs_node->ops->read) {
    return -ENOSYS;
  }

  return res->vfs_node->ops->write(res, buf, count);
}

/**************************************************************************
 * Name:
 *  close
 *
 * Description:
 *  Close the current opened resource identified by fd.
 *
 * Return Value:
 *  OK in case the resource was closed without issues.
 *
 *************************************************************************/
int close(int fd)
{
  disable_int();
  struct opened_resource_s *res = sched_find_opened_resource(fd);
  enable_int();

  if (res == NULL) {
    return -EINVAL;
  }

  if (!res->vfs_node->ops || !res->vfs_node->ops->close) {
    return -ENOSYS;
  }

  sem_wait(&res->vfs_node->lock);

  int ret = res->vfs_node->ops->close(res);
  sched_free_resource(fd);
  res->vfs_node->open_count -= 1;

  sem_post(&res->vfs_node->lock);

  return ret;
}

static inline void wait_usec(void)
{
  volatile uint32_t counter = 0;

  /* 1 us takes this time */
  for (counter; counter < CONFIG_SYSTEM_CLOCK_FREQUENCY; ++counter);
}

/**************************************************************************
 * Name:
 *  usleep
 *
 * Description:
 *  Put the calling process into sleep state for 'microseconds'
 *
 * Return Value:
 *  Zero on success otherwise a negative value.
 *
 *************************************************************************/
int usleep(useconds_t microseconds)
{
  while (microseconds-- > 0)
    wait_usec();

  return OK;
}

/**************************************************************************
 * Name:
 *  unlink
 *
 * Description:
 *  Remove the file from the filesystem. 
 *
 * Input Parameters:
 *  path - the path of the file that we want to remove
 *
 * Return Value:
 *  Zero on success otherwise a negative value.
 *
 *************************************************************************/
int unlink(const char *path)
{
  int ret = 0;

  disable_int();
  struct vfs_node_s *node = vfs_get_matching_node(path, strlen(path));
  enable_int();

  if (node == NULL) {
    return -EINVAL;
  }

  if (!node->ops || !node->ops->unlink) {
    return -ENOSYS;
  }

  sem_wait(&node->lock);
  if (node->open_count > 0) {
    printf("Node %s opened %d times\n", node->name, node->open_count);
    sem_post(&node->lock);
    return -ENFILE;
  }
  sem_post(&node->lock);

  if (node->node_type == VFS_TYPE_FILE) {
    ret = unlink_node(path, node);
  } else if (node->node_type == VFS_TYPE_DIR) {

    /* If the directory has entries don't remove it */

    if (node->num_children > 0) {
      return -ENOTEMPTY;
    }

    ret = unlink_node(path, node);
  } else {
    return -EOPNOTSUPP;
  }

  return ret;
}

/**************************************************************************
 * Name:
 *  mkdir
 *
 * Description:
 *  Create a new directory entry in the file system.
 *
 * Input Parameters:
 *  path - the path of the new directory
 *  mode - the creation modeh
 *
 * Return Value:
 *  Zero on success otherwise a negative value.
 *
 *************************************************************************/
int mkdir(const char *path, mode_t mode)
{
}
