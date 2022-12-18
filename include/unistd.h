#ifndef __UNISTD_H
#define __UNISTD_H

#include <board.h>
#include <stdint.h>
#include <stdlib.h>

typedef int ssize_t;

typedef uint32_t mode_t;

#ifndef _USECONDS_T_DECLARED
#define _USECONDS_T_DECLARED
typedef uint64_t useconds_t;
#endif

/* Basic operations during open */
#define O_RDONLY                        (1 << 0)
#define O_WRONLY                        (1 << 1)
#define O_CREATE                        (1 << 2)
#define O_RW                            (1 << 3)
#define O_APPEND                        (1 << 4)

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
ssize_t read(int fd, void *buf, size_t count);

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
ssize_t write(int fd, void *buf, size_t count);

/**************************************************************************
 * Name:
 *  close
 *
 * Description:
 *  Close the file descriptor and free the associated resources.
 *
 * Return Value:
 *  Zero on success otherwise a negative value.
 *
 *************************************************************************/
int close(int fd);

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
int usleep(useconds_t microseconds);

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
int unlink(const char *path);

/**************************************************************************
 * Name:
 *  mkdir
 *
 * Description:
 *  Create a new directory entry in the file system.
 *
 * Input Parameters:
 *  path - the path of the new directory
 *  mode - the creation mode
 *
 * Return Value:
 *  Zero on success otherwise a negative value.
 *
 *************************************************************************/
int mkdir(const char *path, mode_t mode);

#endif /* __UNISTD_H */
