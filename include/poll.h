/*
 * include/poll.h
 *
 * Created: 22/12/2020
 *  Author: sene
 */

#ifndef __POLL_H
#define __POLL_H

#include <board.h>

#include <list.h>
#include <stdint.h>
#include <semaphore.h>
#include <stdio.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define POLL_WAIT_FOREVER		(-1)

/****************************************************************************
 * Public Types
 ****************************************************************************/

struct pollfd {
	int    fd;       /* file descriptor */
	short  events;   /* events to look for */
	short  revents;  /* events returned */
};

/****************************************************************************
 * Public Scheduler Functions 
 ****************************************************************************/

int poll(struct pollfd fds[], size_t nfds, int timeout)

#endif /* __POLL_H */
