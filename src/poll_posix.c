
#define _XOPEN_SOURCE 500
#include "robotraconteurlite/poll.h"
#include <poll.h>
#include <stdio.h>
#include <unistd.h>


int robotraconteurlite_poll(struct robotraconteurlite_pollfd* fds, int nfds, int timeout)
{
    int ret;
    ret = poll((struct pollfd*)fds, (int)nfds, (int)timeout); 
    return ret;
}