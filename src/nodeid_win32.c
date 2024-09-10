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
#include "robotraconteurlite/err.h"
#include <rpc.h>

int robotraconteurlite_nodeid_newrandom(struct robotraconteurlite_nodeid* a)
{
    /* Use UuidCreate to generate a random UUID */

    UUID uuid;
    RPC_STATUS uuid_rv;
    uuid_rv = UuidCreate(&uuid);
    if (uuid_rv == RPC_S_UUID_NO_ADDRESS)
    {
        return ROBOTRACONTEURLITE_ERROR_INTERNAL_ERROR;
    }
    (void)memcpy(a->data, &uuid, sizeof(a->data));
    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}
