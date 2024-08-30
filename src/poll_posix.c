#ifndef _XOPEN_SOURCE
/* NOLINTNEXTLINE(bugprone-reserved-identifier) */
#define _XOPEN_SOURCE 500
#endif

#include "robotraconteurlite/poll.h"
#include <poll.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

int robotraconteurlite_poll(struct robotraconteurlite_pollfd* fds, int nfds, int timeout)
{
    int ret = -1;
    ret = poll((struct pollfd*)fds, (int)nfds, (int)timeout);
    if (ret < 0 && errno == EINTR)
    {
        return 0;
    }
    return ret;
}
