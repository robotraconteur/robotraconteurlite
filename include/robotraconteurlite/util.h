/* Copyright 2011-2024 Wason Technology, LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    www.apache.org/licenses/LICENSE-2.0
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

#ifdef __cplusplus
extern "C" {
#endif

static uint32_t robotraconteurlite_util_read_uint32(const void* p)
{
    uint32_t ret = 0;
    (void)memcpy(&ret, p, sizeof(uint32_t));
    return ret;
}

static uint16_t robotraconteurlite_util_read_uint16(const void* p)
{
    uint16_t ret = 0;
    (void)memcpy(&ret, p, sizeof(uint16_t));
    return ret;
}

#define ROBOTRACONTEURLITE_FLAGS_CHECK(flags, mask) (((flags) & (mask)) != 0U)
#define ROBOTRACONTEURLITE_FLAGS_CHECK_ALL(flags, mask) (((flags) & (mask)) == (mask))
#define ROBOTRACONTEURLITE_FLAGS_SET(flags, mask) ((flags) |= (mask))
#define ROBOTRACONTEURLITE_FLAGS_CLEAR(flags, mask) ((flags) &= ~(mask))

#if defined(__GNUC__) && !defined(ROBOTRACONTEURLITE_CODE_QUALITY_CHECK)
#define ROBOTRACONTEURLITE_LIKELY(x) __builtin_expect(!!(x), 1)
#define ROBOTRACONTEURLITE_UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
#define ROBOTRACONTEURLITE_LIKELY(x) (x)
#define ROBOTRACONTEURLITE_UNLIKELY(x) (x)
#endif

#define ROBOTRACONTEURLITE_FAILED(rc) (ROBOTRACONTEURLITE_UNLIKELY((rc) < 0))
#define ROBOTRACONTEURLITE_SUCCEEDED(rc) (ROBOTRACONTEURLITE_LIKELY((rc) >= 0))
#define ROBOTRACONTEURLITE_RETRY(rc) ((rc) == ROBOTRACONTEURLITE_ERROR_RETRY)
#define ROBOTRACONTEURLITE_NO_MORE(rc) ((rc) == ROBOTRACONTEURLITE_ERROR_NO_MORE)

#ifdef __cplusplus
}
#endif

#endif /* ROBOTRACONTEURLITE_UTIL_H */
