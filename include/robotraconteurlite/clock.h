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

#ifndef __ROBOTRACONTEURLITE_CLOCK_H__
#define __ROBOTRACONTEURLITE_CLOCK_H__

#include <stdint.h>
#include "robotraconteurlite/config.h"

struct robotraconteurlite_clock
{
    uint64_t clock_epoch_offset;
};

typedef uint64_t robotraconteurlite_timespec;

ROBOTRACONTEURLITE_DECL int robotraconteurlite_clock_init(struct robotraconteurlite_clock* clock);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_clock_gettime(struct robotraconteurlite_clock* clock,
                                                             robotraconteurlite_timespec* now);

#endif /*__ROBOTRACONTEURLITE_CLOCK_H__ */
