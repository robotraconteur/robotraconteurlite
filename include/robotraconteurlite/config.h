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

#ifndef ROBOTRACONTEURLITE_CONFIG_H
#define ROBOTRACONTEURLITE_CONFIG_H

#ifdef _WIN32
#include "robotraconteurlite_export.h"
#endif

#ifndef ROBOTRACONTEURLITE_API
#define ROBOTRACONTEURLITE_API
#endif

#define ROBOTRACONTEURLITE_NODE_VERSION "0.18.0"

#define ROBOTRACONTEURLITE_UNUSED(x) (void)(x)

#ifdef WIN32
#ifdef _WIN64
#define ROBOTRACONTEURLITE_SOCKET unsigned long long
#else
#define ROBOTRACONTEURLITE_SOCKET unsigned long
#endif
#else
#define ROBOTRACONTEURLITE_SOCKET int
#endif

#endif /* ROBOTRACONTEURLITE_CONFIG_H */
