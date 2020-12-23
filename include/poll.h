/*
 * include/poll.h
 *
 * Created: 22/12/2020
 *  Author: sene
 */

#ifndef __POLL_H
#define __POLL_H

#include <board.h>

#include <errno.h>
#include <list.h>
#include <stdint.h>
#include <stdbool.h>
#include <semaphore.h>
#include <stdio.h>
#include <vfs.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* This timeout argument for the poll function specifies endless wait for
 * an event.
 */
#define POLL_WAIT_FOREVER		(-1)

/* Data other than high priority data may be read without blocking. This is
 * equivalent to ( POLLRDNORM | POLLRDBAND ).
 */
#define POLLIN							(1)

/* Normal data may be written without blocking.  This is equivalent to
 * POLLWRNORM.
 */
#define POLLOUT							(2)

/* The device or socket has been disconnected. This flag is output only, and
 * ignored if present in the input events bitmask. Note that POLLHUP and POLLOUT
 * are mutually exclusive and should never be present in the revents bitmask at
 * the same time.
 */
#define POLLHUP							(3)

/* An exceptional condition has occurred on the device or socket. This flag is
 * output only, and ignored if present in the input events bitmask.
 */
#define POLLERR             (4)

/* The file descriptor is not open. This flag is output only, and ignored if
 * present in the input events bitmask.
 */
#define POLLNVAL						(5)

/****************************************************************************
 * Public Types
 ****************************************************************************/

struct pollfd {
	int    fd;       /* file descriptor */
	short  events;   /* events to look for */
	short  revents;  /* events returned */
};

/****************************************************************************
 * Public Functions 
 ****************************************************************************/

int poll(struct pollfd fds[], size_t nfds, int timeout);

#endif /* __POLL_H */
