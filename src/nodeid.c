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

#include "robotraconteurlite/nodeid.h"
#include <string.h>

int robotraconteurlite_nodeid_equal(const struct robotraconteurlite_nodeid* a,
                                    const struct robotraconteurlite_nodeid* b)
{
    return memcmp(a->data, b->data, sizeof(a->data)) == 0;
}

int robotraconteurlite_nodeid_isany(const struct robotraconteurlite_nodeid* a)
{
    uint8_t zero_uuid[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    return memcmp(a->data, zero_uuid, sizeof(a->data)) == 0;
}

int robotraconteurlite_nodeid_reset(struct robotraconteurlite_nodeid* a)
{
    (void)memset(a->data, 0, sizeof(a->data));
    return 0;
}

int robotraconteurlite_nodeid_copy_to(const struct robotraconteurlite_nodeid* src,
                                      struct robotraconteurlite_nodeid* dst)
{
    (void)memcpy(dst->data, src->data, sizeof(dst->data));
    return 0;
}
