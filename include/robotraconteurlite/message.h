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

#ifndef __ROBOTRACONTEURLITE_MESSAGE_H__
#define __ROBOTRACONTEURLITE_MESSAGE_H__

#include "robotraconteurlite/config.h"
#include "robotraconteurlite/err.h"
#include "robotraconteurlite/array.h"
#include "robotraconteurlite/nodeid.h"

enum robotraconteurlite_datatype
{
    ROBOTRACONTEURLITE_DATATYPE_VOID = 0,
    ROBOTRACONTEURLITE_DATATYPE_DOUBLE,
    ROBOTRACONTEURLITE_DATATYPE_SINGLE,
    ROBOTRACONTEURLITE_DATATYPE_INT8,
    ROBOTRACONTEURLITE_DATATYPE_UINT8,
    ROBOTRACONTEURLITE_DATATYPE_INT16,
    ROBOTRACONTEURLITE_DATATYPE_UINT16,
    ROBOTRACONTEURLITE_DATATYPE_INT32,
    ROBOTRACONTEURLITE_DATATYPE_UINT32,
    ROBOTRACONTEURLITE_DATATYPE_INT64,
    ROBOTRACONTEURLITE_DATATYPE_UINT64,
    ROBOTRACONTEURLITE_DATATYPE_STRING,
    ROBOTRACONTEURLITE_DATATYPE_CDOUBLE,
    ROBOTRACONTEURLITE_DATATYPE_CSINGLE,
    ROBOTRACONTEURLITE_DATATYPE_BOOL,
    ROBOTRACONTEURLITE_DATATYPE_STRUCTURE = 101,
    ROBOTRACONTEURLITE_DATATYPE_MAP_INT32,
    ROBOTRACONTEURLITE_DATATYPE_MAP_STRING,
    ROBOTRACONTEURLITE_DATATYPE_OBJECT,
    ROBOTRACONTEURLITE_DATATYPE_VARVALUE,
    ROBOTRACONTEURLITE_DATATYPE_VAROBJECT,
    /* ROBOTRACONTEURLITE_DATATYPE_MULTIDIMARRAY, - Deprecated! */
    ROBOTRACONTEURLITE_DATATYPE_LIST = 108,
    ROBOTRACONTEURLITE_DATATYPE_POD,
    ROBOTRACONTEURLITE_DATATYPE_POD_ARRAY,
    ROBOTRACONTEURLITE_DATATYPE_POD_MULTIDIMARRAY,
    ROBOTRACONTEURLITE_DATATYPE_ENUM,
    ROBOTRACONTEURLITE_DATATYPE_NAMEDTYPE,
    ROBOTRACONTEURLITE_DATATYPE_NAMEDARRAY,
    ROBOTRACONTEURLITE_DATATYPE_NAMEDARRAY_ARRAY,
    ROBOTRACONTEURLITE_DATATYPE_NAMEDARRAY_MULTIDIMARRAY,
    ROBOTRACONTEURLITE_DATATYPE_MULTIDIMARRAY /* Use a new data type code for numeric arrays to avoid confusion */
};

enum robotraconteurlite_messageentrytype
{
    ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_NULL = 0,
    ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_STREAMOP = 1,
    ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_STREAMOPRET,
    ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_STREAMCHECKCAPABILITY,
    ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_STREAMCHECKCAPABILITYRET,
    ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_STRINGTABLEOP,
    ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_STRINGTABLEOPRET,
    ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_GETSERVICEDESC = 101,
    ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_GETSERVICEDESCRET,
    ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_OBJECTTYPENAME,
    ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_OBJECTTYPENAMERET,
    ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_SERVICECLOSED,
    ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_SERVICECLOSEDRET,
    ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_CONNECTCLIENT,
    ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_CONNECTCLIENTRET,
    ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_DISCONNECTCLIENT,
    ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_DISCONNECTCLIENTRET,
    ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_CONNECTIONTEST,
    ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_CONNECTIONTESTRET,
    ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_GETNODEINFO,
    ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_GETNODEINFORET,
    ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_RECONNECTCLIENT,
    ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_RECONNECTCLIENTRET,
    ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_NODECHECKCAPABILITY,
    ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_NODECHECKCAPABILITYRET,
    ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_GETSERVICEATTRIBUTES,
    ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_GETSERVICEATTRIBUTESRET,
    ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_CONNECTCLIENTCOMBINED,
    ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_CONNECTCLIENTCOMBINEDRET,
    ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_ENDPOINTCHECKCAPABILITY = 501,
    ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_ENDPOINTCHECKCAPABILITYRET,
    ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_SERVICECHECKCAPABILITYREQ = 1101,
    ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_SERVICECHECKCAPABILITYRET,
    ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_CLIENTKEEPALIVEREQ = 1105,
    ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_CLIENTKEEPALIVERET,
    ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_CLIENTSESSIONOPREQ = 1107,
    ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_CLIENTSESSIONOPRET,
    ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_SERVICEPATHRELEASEDREQ,
    ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_SERVICEPATHRELEASEDRET,
    ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_PROPERTYGETREQ = 1111,
    ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_PROPERTYGETRES,
    ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_PROPERTYSETREQ,
    ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_PROPERTYSETRES,
    ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_FUNCTIONCALLREQ = 1121,
    ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_FUNCTIONCALLRES,
    ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_GENERATORNEXTREQ,
    ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_GENERATORNEXTRES,
    ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_EVENTREQ = 1131,
    ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_EVENTRES,
    ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_PIPEPACKET = 1141,
    ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_PIPEPACKETRET,
    ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_PIPECONNECTREQ,
    ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_PIPECONNECTRET,
    ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_PIPEDISCONNECTREQ,
    ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_PIPEDISCONNECTRET,
    ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_PIPECLOSED,
    ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_PIPECLOSEDRET,
    ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_CALLBACKCALLREQ = 1151,
    ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_CALLBACKCALLRET,
    ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_WIREPACKET = 1161,
    ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_WIREPACKETRET,
    ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_WIRECONNECTREQ,
    ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_WIRECONNECTRET,
    ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_WIREDISCONNECTREQ,
    ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_WIREDISCONNECTRET,
    ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_WIRECLOSED,
    ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_WIRECLOSEDRET,
    ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_MEMORYREAD = 1171,
    ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_MEMORYREADRET,
    ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_MEMORYWRITE,
    ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_MEMORYWRITERET,
    ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_MEMORYGETPARAM,
    ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_MEMORYGETPARAMRET,
    ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_WIREPEEKINVALUEREQ = 1181,
    ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_WIREPEEKINVALUERET,
    ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_WIREPEEKOUTVALUEREQ,
    ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_WIREPEEKOUTVALUERET,
    ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_WIREPOKEOUTVALUEREQ,
    ROBOTRACONTEURLITE_MESSAGEENTRYTYPE_WIREPOKEOUTVALUERET
};

enum MessageErrorType
{
    ROBOTRACONTEURLITE_MESSAGEERRORTYPE_NONE = 0,
    ROBOTRACONTEURLITE_MESSAGEERRORTYPE_CONNECTIONERROR = 1,
    ROBOTRACONTEURLITE_MESSAGEERRORTYPE_PROTOCOLERROR,
    ROBOTRACONTEURLITE_MESSAGEERRORTYPE_SERVICENOTFOUND,
    ROBOTRACONTEURLITE_MESSAGEERRORTYPE_OBJECTNOTFOUND,
    ROBOTRACONTEURLITE_MESSAGEERRORTYPE_INVALIDENDPOINT,
    ROBOTRACONTEURLITE_MESSAGEERRORTYPE_ENDPOINTCOMMUNICATIONFATALERROR,
    ROBOTRACONTEURLITE_MESSAGEERRORTYPE_NODENOTFOUND,
    ROBOTRACONTEURLITE_MESSAGEERRORTYPE_SERVICEERROR,
    ROBOTRACONTEURLITE_MESSAGEERRORTYPE_MEMBERNOTFOUND,
    ROBOTRACONTEURLITE_MESSAGEERRORTYPE_MEMBERFORMATMISMATCH,
    ROBOTRACONTEURLITE_MESSAGEERRORTYPE_DATATYPEMISMATCH,
    ROBOTRACONTEURLITE_MESSAGEERRORTYPE_DATATYPEERROR,
    ROBOTRACONTEURLITE_MESSAGEERRORTYPE_DATASERIALIZATIONERROR,
    ROBOTRACONTEURLITE_MESSAGEERRORTYPE_MESSAGEENTRYNOTFOUND,
    ROBOTRACONTEURLITE_MESSAGEERRORTYPE_MESSAGEELEMENTNOTFOUND,
    ROBOTRACONTEURLITE_MESSAGEERRORTYPE_UNKNOWNERROR,
    ROBOTRACONTEURLITE_MESSAGEERRORTYPE_INVALIDOPERATION,
    ROBOTRACONTEURLITE_MESSAGEERRORTYPE_INVALIDARGUMENT,
    ROBOTRACONTEURLITE_MESSAGEERRORTYPE_OPERATIONFAILED,
    ROBOTRACONTEURLITE_MESSAGEERRORTYPE_NULLVALUE,
    ROBOTRACONTEURLITE_MESSAGEERRORTYPE_INTERNALERROR,
    ROBOTRACONTEURLITE_MESSAGEERRORTYPE_SYSTEMRESOURCEPERMISSIONDENIED,
    ROBOTRACONTEURLITE_MESSAGEERRORTYPE_OUTOFSYSTEMRESOURCE,
    ROBOTRACONTEURLITE_MESSAGEERRORTYPE_SYSTEMRESOURCEERROR,
    ROBOTRACONTEURLITE_MESSAGEERRORTYPE_RESOURCENOTFOUND,
    ROBOTRACONTEURLITE_MESSAGEERRORTYPE_IOERROR,
    ROBOTRACONTEURLITE_MESSAGEERRORTYPE_BUFFERLIMITVIOLATION,
    ROBOTRACONTEURLITE_MESSAGEERRORTYPE_SERVICEDEFINITIONERROR,
    ROBOTRACONTEURLITE_MESSAGEERRORTYPE_OUTOFRANGE,
    ROBOTRACONTEURLITE_MESSAGEERRORTYPE_KEYNOTFOUND,
    ROBOTRACONTEURLITE_MESSAGEERRORTYPE_REMOTEERROR = 100,
    ROBOTRACONTEURLITE_MESSAGEERRORTYPE_REQUESTTIMEOUT,
    ROBOTRACONTEURLITE_MESSAGEERRORTYPE_READONLYMEMBER,
    ROBOTRACONTEURLITE_MESSAGEERRORTYPE_WRITEONLYMEMBER,
    ROBOTRACONTEURLITE_MESSAGEERRORTYPE_NOTIMPLEMENTEDERROR,
    ROBOTRACONTEURLITE_MESSAGEERRORTYPE_MEMBERBUSY,
    ROBOTRACONTEURLITE_MESSAGEERRORTYPE_VALUENOTSET,
    ROBOTRACONTEURLITE_MESSAGEERRORTYPE_ABORTOPERATION,
    ROBOTRACONTEURLITE_MESSAGEERRORTYPE_OPERATIONABORTED,
    ROBOTRACONTEURLITE_MESSAGEERRORTYPE_STOPITERATION,
    ROBOTRACONTEURLITE_MESSAGEERRORTYPE_AUTHENTICATIONERROR = 150,
    ROBOTRACONTEURLITE_MESSAGEERRORTYPE_OBJECTLOCKEDERROR,
    ROBOTRACONTEURLITE_MESSAGEERRORTYPE_PERMISSIONDENIED
};

struct robotraconteurlite_message_header
{
    uint32_t message_size;
    uint16_t message_version;
    uint16_t header_size;
    struct robotraconteurlite_nodeid sender_nodeid;
    struct robotraconteurlite_nodeid receiver_nodeid;
    uint32_t sender_endpoint;
    uint32_t receiver_endpoint;
    struct robotraconteurlite_string sender_nodename;
    struct robotraconteurlite_string receiver_nodename;
    struct robotraconteurlite_string extended;
    uint16_t entry_count;
    uint16_t message_id;
    uint16_t message_res_id;
};

struct robotraconteurlite_messageentry_header
{
    uint32_t entry_size;
    uint16_t entry_type;
    struct robotraconteurlite_string service_path;
    struct robotraconteurlite_string member_name;
    uint32_t request_id;
    uint16_t error;
    struct robotraconteurlite_string extended;
    uint16_t element_count;
};

struct robotraconteurlite_messageelement_header
{
    uint32_t element_size;
    struct robotraconteurlite_string element_name;
    uint16_t element_type;
    struct robotraconteurlite_string element_type_name;
    struct robotraconteurlite_string extended;
    uint32_t data_count;
};

struct robotraconteurlite_message_reader
{
    struct robotraconteurlite_buffer_vec* buffer;
    size_t buffer_offset;
    size_t buffer_count;
};

struct robotraconteurlite_messageentry_reader
{
    struct robotraconteurlite_buffer_vec* buffer;
    size_t buffer_offset;
    size_t buffer_count;
    size_t buffer_remaining;
    size_t entry_count_offset;
    size_t current_entry;
};

struct robotraconteurlite_messageelement_reader
{
    struct robotraconteurlite_buffer_vec* buffer;
    size_t buffer_offset;
    size_t buffer_count;
    size_t buffer_remaining;
    size_t element_count_offset;
    size_t element_count_uint32;
    size_t current_element;
};

struct robotraconteurlite_message_writer
{
    struct robotraconteurlite_buffer_vec* buffer;
    size_t buffer_offset;
    size_t buffer_count;
    /* uint8_t dry_run; */
};

struct robotraconteurlite_messageentry_writer
{
    struct robotraconteurlite_buffer_vec* buffer;
    size_t buffer_offset;
    size_t buffer_count;
    size_t entries_written_count;
    size_t entries_written_size;
    /* uint8_t dry_run; */
};

struct robotraconteurlite_messageelement_writer
{
    struct robotraconteurlite_buffer_vec* buffer;
    size_t buffer_offset;
    size_t buffer_count;
    size_t elements_written_count;
    size_t elements_written_size;
    /* uint8_t dry_run; */
};

/* Message Reader */

ROBOTRACONTEURLITE_DECL int robotraconteurlite_message_reader_init(struct robotraconteurlite_message_reader* reader,
                                                                   struct robotraconteurlite_buffer_vec* buffer,
                                                                   size_t offset, size_t count);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_message_reader_read_header(
    struct robotraconteurlite_message_reader* reader, struct robotraconteurlite_message_header* header);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_message_reader_begin_read_entries(
    struct robotraconteurlite_message_reader* reader, struct robotraconteurlite_messageentry_reader* entry_reader);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_messageentry_reader_move_next(
    struct robotraconteurlite_messageentry_reader* entry_reader);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_messageentry_reader_read_header(
    struct robotraconteurlite_messageentry_reader* entry_reader, struct robotraconteurlite_messageentry_header* header);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_messageentry_reader_begin_read_elements(
    struct robotraconteurlite_messageentry_reader* entry_reader,
    struct robotraconteurlite_messageelement_reader* element_reader);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_messageelement_reader_move_next(
    struct robotraconteurlite_messageelement_reader* element_reader);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_messageentry_reader_find_element(
    struct robotraconteurlite_messageentry_reader* entry_reader, const struct robotraconteurlite_string* element_name,
    struct robotraconteurlite_messageelement_reader* element_reader);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_messageentry_reader_find_element_verify_scalar(
    struct robotraconteurlite_messageentry_reader* entry_reader, const struct robotraconteurlite_string* element_name,
    struct robotraconteurlite_messageelement_reader* element_reader, uint16_t expected_data_type);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_messageentry_reader_find_element_verify_array(
    struct robotraconteurlite_messageentry_reader* entry_reader, const struct robotraconteurlite_string* element_name,
    struct robotraconteurlite_messageelement_reader* element_reader, uint16_t expected_data_type, uint32_t expected_len,
    uint8_t var_length);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_messageentry_reader_find_element_verify_string(
    struct robotraconteurlite_messageentry_reader* entry_reader, const struct robotraconteurlite_string* element_name,
    struct robotraconteurlite_messageelement_reader* element_reader, uint32_t expected_max_len);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_messageelement_reader_read_header(
    struct robotraconteurlite_messageelement_reader* element_reader,
    struct robotraconteurlite_messageelement_header* header);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_messageelement_reader_read_data_double_array(
    struct robotraconteurlite_messageelement_reader* element_reader, struct robotraconteurlite_array_double* array);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_messageelement_reader_read_data_double(
    struct robotraconteurlite_messageelement_reader* element_reader, double* value);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_messageelement_reader_read_data_single_array(
    struct robotraconteurlite_messageelement_reader* element_reader, struct robotraconteurlite_array_single* array);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_messageelement_reader_read_data_single(
    struct robotraconteurlite_messageelement_reader* element_reader, float* value);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_messageelement_reader_read_data_int8_array(
    struct robotraconteurlite_messageelement_reader* element_reader, struct robotraconteurlite_array_int8* array);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_messageelement_reader_read_data_int8(
    struct robotraconteurlite_messageelement_reader* element_reader, int8_t* value);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_messageelement_reader_read_data_uint8_array(
    struct robotraconteurlite_messageelement_reader* element_reader, struct robotraconteurlite_array_uint8* array);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_messageelement_reader_read_data_uint8(
    struct robotraconteurlite_messageelement_reader* element_reader, uint8_t* value);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_messageelement_reader_read_data_int16_array(
    struct robotraconteurlite_messageelement_reader* element_reader, struct robotraconteurlite_array_int16* array);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_messageelement_reader_read_data_int16(
    struct robotraconteurlite_messageelement_reader* element_reader, int16_t* value);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_messageelement_reader_read_data_uint16_array(
    struct robotraconteurlite_messageelement_reader* element_reader, struct robotraconteurlite_array_uint16* array);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_messageelement_reader_read_data_uint16(
    struct robotraconteurlite_messageelement_reader* element_reader, uint16_t* value);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_messageelement_reader_read_data_int32_array(
    struct robotraconteurlite_messageelement_reader* element_reader, struct robotraconteurlite_array_int32* array);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_messageelement_reader_read_data_int32(
    struct robotraconteurlite_messageelement_reader* element_reader, int32_t* value);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_messageelement_reader_read_data_uint32_array(
    struct robotraconteurlite_messageelement_reader* element_reader, struct robotraconteurlite_array_uint32* array);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_messageelement_reader_read_data_uint32(
    struct robotraconteurlite_messageelement_reader* element_reader, uint32_t* value);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_messageelement_reader_read_data_int64_array(
    struct robotraconteurlite_messageelement_reader* element_reader, struct robotraconteurlite_array_int64* array);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_messageelement_reader_read_data_int64(
    struct robotraconteurlite_messageelement_reader* element_reader, int64_t* value);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_messageelement_reader_read_data_uint64_array(
    struct robotraconteurlite_messageelement_reader* element_reader, struct robotraconteurlite_array_uint64* array);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_messageelement_reader_read_data_uint64(
    struct robotraconteurlite_messageelement_reader* element_reader, uint64_t* value);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_messageelement_reader_read_data_cdouble_array(
    struct robotraconteurlite_messageelement_reader* element_reader, struct robotraconteurlite_array_cdouble* array);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_messageelement_reader_read_data_cdouble(
    struct robotraconteurlite_messageelement_reader* element_reader, struct robotraconteurlite_cdouble* value);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_messageelement_reader_read_data_csingle_array(
    struct robotraconteurlite_messageelement_reader* element_reader,
    struct robotraconteurlite_array_csingle* dest_array);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_messageelement_reader_read_data_csingle(
    struct robotraconteurlite_messageelement_reader* element_reader, struct robotraconteurlite_csingle* value);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_messageelement_reader_read_data_bool_array(
    struct robotraconteurlite_messageelement_reader* element_reader, struct robotraconteurlite_array_bool* dest_array);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_messageelement_reader_read_data_bool(
    struct robotraconteurlite_messageelement_reader* element_reader, struct robotraconteurlite_bool* value);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_messageelement_reader_read_data_string(
    struct robotraconteurlite_messageelement_reader* element_reader, struct robotraconteurlite_string* dest_str);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_messageelement_reader_begin_read_nested_elements(
    struct robotraconteurlite_messageelement_reader* element_reader,
    struct robotraconteurlite_messageelement_reader* nested_element_reader);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_messageelement_reader_find_nested_element(
    struct robotraconteurlite_messageelement_reader* element_reader,
    const struct robotraconteurlite_string* nested_element_name,
    struct robotraconteurlite_messageelement_reader* nested_element_reader);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_messageelement_reader_find_nested_element_verify_scalar(
    struct robotraconteurlite_messageelement_reader* element_reader,
    const struct robotraconteurlite_string* nested_element_name,
    struct robotraconteurlite_messageelement_reader* nested_element_reader, uint16_t expected_data_type);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_messageelement_reader_find_nested_element_verify_array(
    struct robotraconteurlite_messageelement_reader* element_reader,
    const struct robotraconteurlite_string* nested_element_name,
    struct robotraconteurlite_messageelement_reader* nested_element_reader, uint16_t expected_data_type,
    uint32_t expected_len, uint8_t var_length);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_messageelement_reader_find_nested_element_verify_string(
    struct robotraconteurlite_messageelement_reader* element_reader,
    const struct robotraconteurlite_string* nested_element_name,
    struct robotraconteurlite_messageelement_reader* nested_element_reader, uint32_t expected_max_len);

/* Message Writer */

ROBOTRACONTEURLITE_DECL int robotraconteurlite_message_writer_init(struct robotraconteurlite_message_writer* writer,
                                                                   struct robotraconteurlite_buffer_vec* buffer,
                                                                   size_t offset, size_t count);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_message_writer_begin_message(
    struct robotraconteurlite_message_writer* writer, struct robotraconteurlite_message_header* header,
    struct robotraconteurlite_messageentry_writer* entry_writer);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_message_writer_end_message(
    struct robotraconteurlite_message_writer* writer, struct robotraconteurlite_message_header* header,
    struct robotraconteurlite_messageentry_writer* entry_writer);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_messageentry_writer_begin_entry(
    struct robotraconteurlite_messageentry_writer* entry_writer, struct robotraconteurlite_messageentry_header* header,
    struct robotraconteurlite_messageelement_writer* element_writer);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_messageentry_writer_end_entry(
    struct robotraconteurlite_messageentry_writer* entry_writer, struct robotraconteurlite_messageentry_header* header,
    struct robotraconteurlite_messageelement_writer* element_writer);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_messageelement_writer_begin_nested_element(
    struct robotraconteurlite_messageelement_writer* element_writer,
    struct robotraconteurlite_messageelement_header* header,
    struct robotraconteurlite_messageelement_writer* nested_element_writer);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_messageelement_writer_end_nested_element(
    struct robotraconteurlite_messageelement_writer* element_writer,
    struct robotraconteurlite_messageelement_header* header,
    struct robotraconteurlite_messageelement_writer* nested_element_writer);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_messageelement_writer_write_double(
    struct robotraconteurlite_messageelement_writer* element_writer,
    const struct robotraconteurlite_string* element_name, double value);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_messageelement_writer_write_double_array(
    struct robotraconteurlite_messageelement_writer* element_writer,
    const struct robotraconteurlite_string* element_name, const struct robotraconteurlite_array_double* value);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_messageelement_writer_write_single(
    struct robotraconteurlite_messageelement_writer* element_writer,
    const struct robotraconteurlite_string* element_name, float value);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_messageelement_writer_write_single_array(
    struct robotraconteurlite_messageelement_writer* element_writer,
    const struct robotraconteurlite_string* element_name, const struct robotraconteurlite_array_single* value);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_messageelement_writer_write_int8(
    struct robotraconteurlite_messageelement_writer* element_writer,
    const struct robotraconteurlite_string* element_name, int8_t value);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_messageelement_writer_write_int8_array(
    struct robotraconteurlite_messageelement_writer* element_writer,
    const struct robotraconteurlite_string* element_name, const struct robotraconteurlite_array_int8* value);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_messageelement_writer_write_uint8(
    struct robotraconteurlite_messageelement_writer* element_writer,
    const struct robotraconteurlite_string* element_name, uint8_t value);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_messageelement_writer_write_uint8_array(
    struct robotraconteurlite_messageelement_writer* element_writer,
    const struct robotraconteurlite_string* element_name, const struct robotraconteurlite_array_uint8* value);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_messageelement_writer_write_int16(
    struct robotraconteurlite_messageelement_writer* element_writer,
    const struct robotraconteurlite_string* element_name, int16_t value);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_messageelement_writer_write_int16_array(
    struct robotraconteurlite_messageelement_writer* element_writer,
    const struct robotraconteurlite_string* element_name, const struct robotraconteurlite_array_int16* value);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_messageelement_writer_write_uint16(
    struct robotraconteurlite_messageelement_writer* element_writer,
    const struct robotraconteurlite_string* element_name, uint16_t value);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_messageelement_writer_write_uint16_array(
    struct robotraconteurlite_messageelement_writer* element_writer,
    const struct robotraconteurlite_string* element_name, const struct robotraconteurlite_array_uint16* value);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_messageelement_writer_write_int32(
    struct robotraconteurlite_messageelement_writer* element_writer,
    const struct robotraconteurlite_string* element_name, int32_t value);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_messageelement_writer_write_int32_array(
    struct robotraconteurlite_messageelement_writer* element_writer,
    const struct robotraconteurlite_string* element_name, const struct robotraconteurlite_array_int32* value);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_messageelement_writer_write_uint32(
    struct robotraconteurlite_messageelement_writer* element_writer,
    const struct robotraconteurlite_string* element_name, uint32_t value);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_messageelement_writer_write_uint32_array(
    struct robotraconteurlite_messageelement_writer* element_writer,
    const struct robotraconteurlite_string* element_name, const struct robotraconteurlite_array_uint32* value);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_messageelement_writer_write_int64(
    struct robotraconteurlite_messageelement_writer* element_writer,
    const struct robotraconteurlite_string* element_name, int64_t value);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_messageelement_writer_write_int64_array(
    struct robotraconteurlite_messageelement_writer* element_writer,
    const struct robotraconteurlite_string* element_name, const struct robotraconteurlite_array_int64* value);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_messageelement_writer_write_uint64(
    struct robotraconteurlite_messageelement_writer* element_writer,
    const struct robotraconteurlite_string* element_name, uint64_t value);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_messageelement_writer_write_uint64_array(
    struct robotraconteurlite_messageelement_writer* element_writer,
    const struct robotraconteurlite_string* element_name, const struct robotraconteurlite_array_uint64* value);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_messageelement_writer_write_cdouble(
    struct robotraconteurlite_messageelement_writer* element_writer,
    const struct robotraconteurlite_string* element_name, struct robotraconteurlite_cdouble value);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_messageelement_writer_write_cdouble_array(
    struct robotraconteurlite_messageelement_writer* element_writer,
    const struct robotraconteurlite_string* element_name, const struct robotraconteurlite_array_cdouble* value);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_messageelement_writer_write_csingle(
    struct robotraconteurlite_messageelement_writer* element_writer,
    const struct robotraconteurlite_string* element_name, struct robotraconteurlite_csingle value);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_messageelement_writer_write_csingle_array(
    struct robotraconteurlite_messageelement_writer* element_writer,
    const struct robotraconteurlite_string* element_name, const struct robotraconteurlite_array_csingle* value);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_messageelement_writer_write_bool(
    struct robotraconteurlite_messageelement_writer* element_writer,
    const struct robotraconteurlite_string* element_name, struct robotraconteurlite_bool value);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_messageelement_writer_write_bool_array(
    struct robotraconteurlite_messageelement_writer* element_writer,
    const struct robotraconteurlite_string* element_name, const struct robotraconteurlite_array_bool* value);

ROBOTRACONTEURLITE_DECL int robotraconteurlite_messageelement_writer_write_string(
    struct robotraconteurlite_messageelement_writer* element_writer,
    const struct robotraconteurlite_string* element_name, const struct robotraconteurlite_string* value);

static int robotraconteurlite_messageelement_writer_write_string_c_str(
    struct robotraconteurlite_messageelement_writer* element_writer, const char* element_name, const char* value)
{
    struct robotraconteurlite_string element_name_str;
    struct robotraconteurlite_string value_str;
    robotraconteurlite_string_from_c_str(element_name, &element_name_str);
    robotraconteurlite_string_from_c_str(value, &value_str);
    return robotraconteurlite_messageelement_writer_write_string(element_writer, &element_name_str, &value_str);
}

#endif /* __ROBOTRACONTEURLITE_MESSAGE_H__ */
