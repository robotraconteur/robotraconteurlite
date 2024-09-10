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
#include <uuid/uuid.h>
#include <string.h>

int robotraconteurlite_nodeid_newrandom(struct robotraconteurlite_nodeid* a)
{
    /* Use uuid.h to generate a random UUID */
    uuid_t uuid;
    uuid_generate_random(uuid);
    (void)memcpy(a->data, uuid, sizeof(a->data));
    return 0;
}
