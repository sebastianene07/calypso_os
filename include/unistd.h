#ifndef __UNISTD_H
#define __UNISTD_H

#include <board.h>
#include <stdint.h>
#include <stdlib.h>

typedef int ssize_t;

typedef uint32_t mode_t;
typedef uint32_t useconds_t;

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

#endif /* __UNISTD_H */
