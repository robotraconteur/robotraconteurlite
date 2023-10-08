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


#ifndef __ROBOTRACONTEURLITE_ERROR_H__
#define __ROBOTRACONTEURLITE_ERROR_H__

enum robotraconteurlite_error
{
	ROBOTRACONTEURLITE_ERROR_SUCCESS = 0,
	ROBOTRACONTEURLITE_ERROR_GENERIC_ERROR = -1,
	ROBOTRACONTEURLITE_ERROR_INVALID_ARGUMENT = -2,
	ROBOTRACONTEURLITE_ERROR_INVALID_OPERATION = -3,
	ROBOTRACONTEURLITE_ERROR_OUT_OF_RANGE = -4,
	ROBOTRACONTEURLITE_ERROR_INVALID_VERSION = -5,
	ROBOTRACONTEURLITE_ERROR_DATA_ERROR = -6,
	ROBOTRACONTEURLITE_ERROR_KEY_NOT_FOUND = -7,
	ROBOTRACONTEURLITE_ERROR_NOT_IMPLEMENTED = -8,
	ROBOTRACONTEURLITE_ERROR_NO_MORE = -9,
	ROBOTRACONTEURLITE_ERROR_BUSY = -10,
	ROBOTRACONTEURLITE_ERROR_NO_VALUE = -11,
	ROBOTRACONTEURLITE_ERROR_TIMEOUT = -12,
	ROBOTRACONTEURLITE_ERROR_PROTOCOL = -13,
	ROBOTRACONTEURLITE_ERROR_MESSAGEELEMENT_NOT_FOUND = -14,
	ROBOTRACONTEURLITE_ERROR_MESSAGEELEMENT_TYPE_MISMATCH = -15,
	ROBOTRACONTEURLITE_ERROR_CONNECTION_ERROR = -16,
	ROBOTRACONTEURLITE_ERROR_RETRY = -17,
	ROBOTRACONTEURLITE_ERROR_CONSUMED = -18,
	ROBOTRACONTEURLITE_ERROR_INTERNAL_ERROR = -19,
	ROBOTRACONTEURLITE_ERROR_UNHANDLED_EVENT = -20,
	ROBOTRACONTEURLITE_ERROR_REQUEST_REMOTE_ERROR = -21
};


#endif /* __ROBOTRACONTEURLITE_ERROR_H__ */
