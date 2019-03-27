#ifndef __UNISTD_H
#define __UNISTD_H

#include <board.h>
#include <stdint.h>
#include <stdlib.h>

typedef int ssize_t;
typedef uint32_t mode_t;

ssize_t read(int fd, void *buf, size_t count);

int close(int fd);

#endif /* __UNISTD_H */
