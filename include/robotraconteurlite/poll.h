/* Copyright 2011-2023 Wason Technology, LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ROBOTRACONTEURLITE_POLL_H
#define ROBOTRACONTEURLITE_POLL_H

#include <stdint.h>
#include <stdlib.h>
#include "robotraconteurlite/clock.h"

struct robotraconteurlite_pollfd
{
    int fd;
    short int events;
    short int revents;
};

int robotraconteurlite_poll(struct robotraconteurlite_pollfd* fds, int nfds, int timeout);

ROBOTRACONTEURLITE_DECL robotraconteurlite_status
robotraconteurlite_wait_next_wake(struct robotraconteurlite_clock* clock, struct robotraconteurlite_pollfd* pollfds,
                                  size_t pollfd_count, robotraconteurlite_timespec wake_time);

#endif /* ROBOTRACONTEURLITE_POLL_H */
