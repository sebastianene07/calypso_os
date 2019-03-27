#include <board.h>

#include <unistd.h>
#include <errno.h>

ssize_t read(int fd, void *buf, size_t count)
{
  return -ENOSYS;
}

int close(int fd)
{
  return -ENOSYS;
}
