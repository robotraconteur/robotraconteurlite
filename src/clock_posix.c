#ifndef _POSIX_C_SOURCE
/* NOLINTNEXTLINE(bugprone-reserved-identifier) */
#define _POSIX_C_SOURCE 199309L
#endif

#include <time.h>
#include <stdio.h>

#include "robotraconteurlite/clock.h"
#include "robotraconteurlite/err.h"

robotraconteurlite_status robotraconteurlite_clock_init(struct robotraconteurlite_clock* clock)
{
    struct timespec monotonic_time;
    struct timespec realtime_time;
    int64_t monotonic_ms = 0;
    int64_t realtime_ms = 0;
    clock_gettime(CLOCK_MONOTONIC, &monotonic_time);
    clock_gettime(CLOCK_REALTIME, &realtime_time);

    /* Convert both times to uint64_t ms */
    monotonic_ms = (monotonic_time.tv_sec * 1000) + (monotonic_time.tv_nsec / 1000000);
    realtime_ms = (realtime_time.tv_sec * 1000) + (realtime_time.tv_nsec / 1000000);

    /* Calculate the offset */
    clock->clock_epoch_offset = realtime_ms - monotonic_ms;

    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

robotraconteurlite_status robotraconteurlite_clock_gettime(struct robotraconteurlite_clock* clock,
                                                           robotraconteurlite_timespec* now)
{
    struct timespec monotonic_time;
    int64_t monotonic_ms = 0;
    clock_gettime(CLOCK_MONOTONIC, &monotonic_time);

    monotonic_ms = (monotonic_time.tv_sec * 1000) + (monotonic_time.tv_nsec / 1000000);

    *now = monotonic_ms + clock->clock_epoch_offset;

    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}
