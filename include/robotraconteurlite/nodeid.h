/* Copyright 2011-2019 Wason Technology, LLC
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

#ifndef __ROBOTRACONTEURLITE_NODEID_H__
#define __ROBOTRACONTEURLITE_NODEID_H__

#include <stdint.h>
#include "robotraconteurlite/config.h"

struct robotraconteurlite_nodeid
{
	uint8_t data[16];
};

ROBOTRACONTEURLITE_DECL int robotraconteurlite_nodeid_equal(const struct robotraconteurlite_nodeid *a, const struct robotraconteurlite_nodeid *b);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_nodeid_isany(const struct robotraconteurlite_nodeid *a);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_nodeid_newrandom(struct robotraconteurlite_nodeid *a);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_nodeid_reset(struct robotraconteurlite_nodeid *a);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_nodeid_copy_to(const struct robotraconteurlite_nodeid *src, struct robotraconteurlite_nodeid *dst);

#endif /* __ROBOTRACONTEURLITE_NODEID_H__ */
