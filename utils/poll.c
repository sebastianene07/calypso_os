/*
 * include/poll.h
 *
 * Created: 22/12/2020
 *  Author: sene
 */

#include <board.h>

#include "poll.h"

/* Mask that verifies if an event is set */

#define IS_POLL_EVENT_SET(mask, evt)      (((mask) & (1 << (evt))) >> (evt))

/****************************************************************************
 * Public Functions 
 ****************************************************************************/

int poll(struct pollfd fds[], size_t nfds, int timeout)
{
  sem_t poll_sema;
  bool is_poll_wait_needed;
  int ret = 0;

  /* Check the pollers if we have to wait for events */

  for (int i = 0; i < nfds; i++)
  {
		if (IS_POLL_EVENT_SET(fds[i].events, POLLIN) ||
        IS_POLL_EVENT_SET(fds[i].events, POLLOUT))
    {
      struct pollfd *poller = &fds[i];
    
      /* Clear out the returned events */

      poller->revents     = 0;

      /* Get the opened resource from the file descriptor */

      irq_state_t irq_state = cpu_disableint();
      struct opened_resource_s *res = sched_find_opened_resource(poller->fd);
      cpu_enableint(irq_state);

      if (res->vfs_node->ops || res->vfs_node->ops->poll)
      {
        return -ENOSYS;
      }

      ret = res->vfs_node->ops->poll(res, &poll_sema, &poller->revents); 
      if (ret == 0)
      {
        is_poll_wait_needed = true;
      }
    }
  }

  if (is_poll_wait_needed)
  {
    sem_timedwait(&poll_sema, POLL_WAIT_FOREVER);
  }

  return ret;
}
