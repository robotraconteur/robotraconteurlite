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

#ifndef ROBOTRACONTEURLITE_UTIL_H
#define ROBOTRACONTEURLITE_UTIL_H

#include <stdint.h>
#include <string.h>

#include "robotraconteurlite/config.h"

static uint32_t robotraconteurlite_util_read_uint32(const void* p)
{
    uint32_t ret = 0;
    memcpy(&ret, p, sizeof(uint32_t));
    return ret;
}

static uint16_t robotraconteurlite_util_read_uint16(const void* p)
{
    uint16_t ret = 0;
    memcpy(&ret, p, sizeof(uint16_t));
    return ret;
}

#endif /* ROBOTRACONTEURLITE_UTIL_H */
