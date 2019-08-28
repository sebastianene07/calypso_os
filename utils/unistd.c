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

  if (!res->ops || !res->ops->read)
  {
    return -ENOSYS;
  }

  return res->ops->read(res->priv, buf, count);
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

  if (!res->ops || !res->ops->close)
  {
    return -ENOSYS;
  }

  return res->ops->close(res->priv);
}
