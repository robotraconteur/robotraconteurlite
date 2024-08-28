#include "robotraconteurlite/poll.h"
#include "robotraconteurlite/err.h"
#include <limits.h>

int robotraconteurlite_wait_next_wake(struct robotraconteurlite_clock* clock, struct robotraconteurlite_pollfd* pollfds,
                                      size_t pollfd_count, robotraconteurlite_timespec wake_time)
{
    robotraconteurlite_timespec now = 0;
    int ret = -1;
    int timeout = 0;
    int64_t timeout_i64 = 0;

    ret = robotraconteurlite_clock_gettime(clock, &now);
    if (ret != ROBOTRACONTEURLITE_ERROR_SUCCESS)
    {
        return ret;
    }

    if (wake_time < now)
    {
        return ROBOTRACONTEURLITE_ERROR_SUCCESS;
    }

    timeout_i64 = wake_time - now;
    if (timeout_i64 > INT_MAX)
    {
        /* This should never happen! */
        timeout = INT_MAX;
    }
    else
    {
        timeout = (int)timeout_i64;
    }

    ret = robotraconteurlite_poll(pollfds, pollfd_count, timeout);
    if (ret < 0)
    {
        return ROBOTRACONTEURLITE_ERROR_SYSTEM_ERROR;
    }

    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}
