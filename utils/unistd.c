#include <board.h>

#include <scheduler.h>
#include <vfs.h>
#include <unistd.h>
#include <errno.h>

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

  return res->vfs_node->ops->close(res);
}

static inline void wait_usec(void)
{
  uint32_t counter = 0;

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
