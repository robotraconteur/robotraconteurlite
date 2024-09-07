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

#include <robotraconteurlite/message.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "robotraconteurlite/util.h"

#define FLAGS_CHECK_ALL ROBOTRACONTEURLITE_FLAGS_CHECK_ALL
#define FLAGS_CHECK ROBOTRACONTEURLITE_FLAGS_CHECK
#define FLAGS_SET ROBOTRACONTEURLITE_FLAGS_SET
#define FLAGS_CLEAR ROBOTRACONTEURLITE_FLAGS_CLEAR

static const char rrac_magic[4] = {'R', 'R', 'A', 'C'};

/* Message Reader */

robotraconteurlite_status robotraconteurlite_message_reader_init(struct robotraconteurlite_message_reader* reader,
                                                                 struct robotraconteurlite_buffer_vec* buffer,
                                                                 size_t offset, size_t count)
{
    size_t buffer_len = 0;
    robotraconteurlite_status rv = -1;
    uint8_t magic_data[4];
    struct robotraconteurlite_array_uint8 magic_array;
    uint32_t message_size = 0;

    magic_array.data = magic_data;
    magic_array.len = 4;

    rv = robotraconteurlite_buffer_vec_len(buffer, &buffer_len);
    if (rv < 0)
    {
        return rv;
    }

    if (buffer_len < (offset + count))
    {
        return ROBOTRACONTEURLITE_ERROR_OUT_OF_RANGE;
    }

    if (count < 64U)
    {
        return ROBOTRACONTEURLITE_ERROR_OUT_OF_RANGE;
    }

    rv = robotraconteurlite_buffer_vec_copy_to_uint8_array(buffer, offset, &magic_array, 0, 4);
    if (rv < 0)
    {
        return rv;
    }

    if (memcmp(magic_data, rrac_magic, 4) != 0)
    {
        return ROBOTRACONTEURLITE_ERROR_PROTOCOL;
    }

    rv = robotraconteurlite_buffer_vec_copy_to_uint32(buffer, offset + 4U, &message_size);
    if (rv < 0)
    {
        return rv;
    }

    if (count < message_size)
    {
        return ROBOTRACONTEURLITE_ERROR_OUT_OF_RANGE;
    }

    reader->buffer = buffer;
    reader->buffer_offset = offset;
    reader->buffer_count = count;

    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

static robotraconteurlite_status robotraconteurlite_message_read_header_string(
    struct robotraconteurlite_buffer_vec* buffer, size_t offset, uint16_t str_len,
    struct robotraconteurlite_string* str)
{
    robotraconteurlite_status rv = -1;

    if (str_len == 0U)
    {
        str->len = 0;
        return ROBOTRACONTEURLITE_ERROR_SUCCESS;
    }

    if ((str->data == NULL) && (str->len == 0U))
    {
        str->len = str_len;
        return ROBOTRACONTEURLITE_ERROR_SUCCESS;
    }
    else
    {
        assert(str->data != NULL);
        if (str->len < str_len)
        {
            return ROBOTRACONTEURLITE_ERROR_INVALID_ARGUMENT;
        }

        rv = robotraconteurlite_buffer_vec_copy_to_string(buffer, offset, str, 0, str_len);
        if (rv < 0)
        {
            return rv;
        }

        str->len = str_len;
        return ROBOTRACONTEURLITE_ERROR_SUCCESS;
    }
}

static robotraconteurlite_status robotraconteurlite_message_read_uint32(struct robotraconteurlite_buffer_vec* buffer,
                                                                        size_t* offset, uint32_t* val)
{
    robotraconteurlite_status rv = -1;
    rv = robotraconteurlite_buffer_vec_copy_to_uint32(buffer, *offset, val);
    if (rv < 0)
    {
        return rv;
    }
    *offset += 4U;
    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

static robotraconteurlite_status robotraconteurlite_message_read_uint16(struct robotraconteurlite_buffer_vec* buffer,
                                                                        size_t* offset, uint16_t* val)
{
    robotraconteurlite_status rv = -1;
    rv = robotraconteurlite_buffer_vec_copy_to_uint16(buffer, *offset, val);
    if (rv < 0)
    {
        return rv;
    }
    *offset += 2U;
    return rv;
}

static robotraconteurlite_status robotraconteurlite_message_read_uint8(struct robotraconteurlite_buffer_vec* buffer,
                                                                       size_t* offset, uint8_t* val)
{
    robotraconteurlite_status rv = -1;
    rv = robotraconteurlite_buffer_vec_copy_to_uint8(buffer, *offset, val);
    if (rv < 0)
    {
        return rv;
    }
    *offset += 1U;
    return rv;
}

static robotraconteurlite_status robotraconteurlite_message_read_uint_x(struct robotraconteurlite_buffer_vec* buffer,
                                                                        size_t* offset, uint32_t* val)
{
    robotraconteurlite_status rv = 0;
    uint8_t b = 0;
    rv = robotraconteurlite_message_read_uint8(buffer, offset, &b);
    if (rv < 0)
    {
        return rv;
    }

    if (b <= 252)
    {
        *val = b;
        return ROBOTRACONTEURLITE_ERROR_SUCCESS;
    }
    switch (b)
    {
    case 253: {
        uint16_t val16 = 0;
        rv = robotraconteurlite_message_read_uint16(buffer, offset, &val16);
        if (rv < 0)
        {
            return rv;
        }
        *val = val16;
        return ROBOTRACONTEURLITE_ERROR_SUCCESS;
    }
    case 254:
        return robotraconteurlite_message_read_uint32(buffer, offset, val);
    default:
        return ROBOTRACONTEURLITE_ERROR_PROTOCOL;
    }
}

static robotraconteurlite_status robotraconteurlite_message_read_int_x(struct robotraconteurlite_buffer_vec* buffer,
                                                                       size_t* offset, int32_t* val)
{
    robotraconteurlite_status rv = 0;
    int8_t b = 0;
    rv = robotraconteurlite_buffer_vec_copy_to_int8(buffer, *offset, &b);
    if (rv < 0)
    {
        return rv;
    }
    *offset += 1U;

    if (b <= 124)
    {
        *val = b;
        return ROBOTRACONTEURLITE_ERROR_SUCCESS;
    }
    switch (b)
    {
    case 125: {
        int16_t val16 = 0;
        rv = robotraconteurlite_buffer_vec_copy_to_int16(buffer, *offset, &val16);
        if (rv < 0)
        {
            return rv;
        }
        *val = val16;
        *offset += 2U;
        return ROBOTRACONTEURLITE_ERROR_SUCCESS;
    }
    case 126: {
        int32_t val32 = 0;
        rv = robotraconteurlite_buffer_vec_copy_to_int32(buffer, *offset, &val32);
        if (rv < 0)
        {
            return rv;
        }
        *val = val32;
        *offset += 4U;
        return ROBOTRACONTEURLITE_ERROR_SUCCESS;
    }
    default:
        return ROBOTRACONTEURLITE_ERROR_PROTOCOL;
    }
}

static robotraconteurlite_status robotraconteurlite_message_read_count(struct robotraconteurlite_buffer_vec* buffer,
                                                                       uint16_t ver, size_t* offset, uint32_t* val)
{
    switch (ver)
    {
    case 2:
        return robotraconteurlite_message_read_uint32(buffer, offset, val);
    case 4:
        return robotraconteurlite_message_read_uint_x(buffer, offset, val);
    default:
        return ROBOTRACONTEURLITE_ERROR_PROTOCOL;
    }
}

static robotraconteurlite_status robotraconteurlite_message_read_count2(struct robotraconteurlite_buffer_vec* buffer,
                                                                        uint16_t ver, size_t* offset, uint32_t* val)
{
    switch (ver)
    {
    case 2: {
        uint16_t val16 = 0;
        robotraconteurlite_status rv = robotraconteurlite_message_read_uint16(buffer, offset, &val16);
        if (rv < 0)
        {
            return rv;
        }
        *val = val16;
        return rv;
    }
    case 4:
        return robotraconteurlite_message_read_uint_x(buffer, offset, val);
    default:
        return ROBOTRACONTEURLITE_ERROR_PROTOCOL;
    }
}

static robotraconteurlite_status robotraconteurlite_message_read_nodeid(struct robotraconteurlite_buffer_vec* buffer,
                                                                        size_t* offset,
                                                                        struct robotraconteurlite_array_uint8* val)
{
    robotraconteurlite_status rv = -1;
    rv = robotraconteurlite_buffer_vec_copy_to_uint8_array(buffer, *offset, val, 0, 16);
    if (rv < 0)
    {
        return rv;
    }
    *offset += 16U;
    return rv;
}

struct robotraconteurlite_message_read_header_string_info
{
    size_t offset;
    size_t str_len;
};

static robotraconteurlite_status robotraconteurlite_message_read_header_string_with_len_prefix1(
    struct robotraconteurlite_buffer_vec* buffer, uint16_t ver, size_t* offset,
    struct robotraconteurlite_message_read_header_string_info* str_info)
{
    robotraconteurlite_status rv = -1;
    uint32_t str_len = 0;
    rv = robotraconteurlite_message_read_count2(buffer, ver, offset, &str_len);
    if (rv < 0)
    {
        return rv;
    }
    str_info->offset = *offset;
    str_info->str_len = str_len;
    *offset += str_len;
    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

static robotraconteurlite_status robotraconteurlite_message_read_header_string_with_len_prefix2(
    struct robotraconteurlite_buffer_vec* buffer, uint16_t ver,
    struct robotraconteurlite_message_read_header_string_info* str_info, struct robotraconteurlite_string* str)
{
    ROBOTRACONTEURLITE_UNUSED(ver);
    return robotraconteurlite_message_read_header_string(buffer, str_info->offset, str_info->str_len, str);
}

robotraconteurlite_status robotraconteurlite_message_reader_read_header(
    struct robotraconteurlite_message_reader* reader, struct robotraconteurlite_message_header* header)
{
    struct robotraconteurlite_message_buffer_info buffer_info;
    robotraconteurlite_status rv = -1;
    rv = robotraconteurlite_message_reader_read_header_ex(reader, header, &buffer_info);
    if (rv < 0)
    {
        return rv;
    }
    reader->message_version = header->message_version;
    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

static robotraconteurlite_status robotraconteurlite_message_reader_read_header_ex4(
    struct robotraconteurlite_message_reader* reader, struct robotraconteurlite_message_header* header,
    struct robotraconteurlite_message_buffer_info* buffer_info)
{
    size_t o = reader->buffer_offset;
    size_t start_o = o;
    robotraconteurlite_status rv = -1;
    struct robotraconteurlite_array_uint8 sender_nodeid_array;
    struct robotraconteurlite_array_uint8 recv_nodeid_array;
    struct robotraconteurlite_message_read_header_string_info sender_nodename_info;
    struct robotraconteurlite_message_read_header_string_info receiver_nodename_info;
    struct robotraconteurlite_message_read_header_string_info meta_info;

    struct robotraconteurlite_message_buffer_info temp_buffer_info;

    uint8_t flags = 0;

    memset(&sender_nodename_info, 0, sizeof(sender_nodename_info));
    memset(&receiver_nodename_info, 0, sizeof(receiver_nodename_info));
    memset(&meta_info, 0, sizeof(meta_info));

    temp_buffer_info.start_buffer_offset = reader->buffer_offset;

    sender_nodeid_array.data = header->sender_nodeid.data;
    sender_nodeid_array.len = 16;
    recv_nodeid_array.data = header->receiver_nodeid.data;
    recv_nodeid_array.len = 16;

    /* skip RRAC magic */
    o += 4U;

    rv = robotraconteurlite_message_read_uint32(reader->buffer, &o, &header->message_size);
    if (rv < 0)
    {
        return rv;
    }

    rv = robotraconteurlite_message_read_uint16(reader->buffer, &o, &header->message_version);
    if (rv < 0)
    {
        return rv;
    }

    if (header->message_version != 4U)
    {
        return ROBOTRACONTEURLITE_ERROR_PROTOCOL;
    }

    rv = robotraconteurlite_message_read_count2(reader->buffer, 4, &o, &header->header_size);
    if (rv < 0)
    {
        return rv;
    }

    temp_buffer_info.header_size = header->header_size;

    rv = robotraconteurlite_message_read_uint8(reader->buffer, &o, &flags);
    if (rv < 0)
    {
        return rv;
    }

    if (FLAGS_CHECK(flags, ROBOTRACONTEURLITE_MESSAGE_FLAGS_ROUTING_INFO))
    {
        rv = robotraconteurlite_message_read_nodeid(reader->buffer, &o, &sender_nodeid_array);
        if (rv < 0)
        {
            return rv;
        }

        rv = robotraconteurlite_message_read_nodeid(reader->buffer, &o, &recv_nodeid_array);
        if (rv < 0)
        {
            return rv;
        }

        rv = robotraconteurlite_message_read_header_string_with_len_prefix1(reader->buffer, 4, &o,
                                                                            &sender_nodename_info);
        if (rv < 0)
        {
            return rv;
        }

        rv = robotraconteurlite_message_read_header_string_with_len_prefix1(reader->buffer, 4, &o,
                                                                            &receiver_nodename_info);
        if (rv < 0)
        {
            return rv;
        }
    }
    else
    {
        memset(header->sender_nodeid.data, 0, 16);
        memset(header->receiver_nodeid.data, 0, 16);
    }

    if (FLAGS_CHECK(flags, ROBOTRACONTEURLITE_MESSAGE_FLAGS_ENDPOINT_INFO))
    {
        rv = robotraconteurlite_message_read_count(reader->buffer, 4, &o, &header->sender_endpoint);
        if (rv < 0)
        {
            return rv;
        }

        rv = robotraconteurlite_message_read_count(reader->buffer, 4, &o, &header->receiver_endpoint);
        if (rv < 0)
        {
            return rv;
        }
    }
    else
    {
        header->sender_endpoint = 0;
        header->receiver_endpoint = 0;
    }

    if (FLAGS_CHECK(flags, ROBOTRACONTEURLITE_MESSAGE_FLAGS_PRIORITY))
    {
        rv = robotraconteurlite_message_read_uint16(reader->buffer, &o, &header->priority);
        if (rv < 0)
        {
            return rv;
        }
    }
    else
    {
        header->priority = 0;
    }

    if (FLAGS_CHECK(flags, ROBOTRACONTEURLITE_MESSAGE_FLAGS_META_INFO))
    {
        rv = robotraconteurlite_message_read_header_string_with_len_prefix1(reader->buffer, 4, &o, &meta_info);
        if (rv < 0)
        {
            return rv;
        }

        rv = robotraconteurlite_message_read_uint16(reader->buffer, &o, &header->message_id);
        if (rv < 0)
        {
            return rv;
        }

        rv = robotraconteurlite_message_read_uint16(reader->buffer, &o, &header->message_res_id);
        if (rv < 0)
        {
            return rv;
        }
    }
    else
    {
        header->message_id = 0;
        header->message_res_id = 0;
    }

    if (FLAGS_CHECK(flags, ROBOTRACONTEURLITE_MESSAGE_FLAGS_STRING_TABLE))
    {
        uint32_t string_table_size = 0;
        rv = robotraconteurlite_message_read_count(reader->buffer, 4, &o, &string_table_size);
        if (rv < 0)
        {
            return rv;
        }
        o += string_table_size;
    }

    if (FLAGS_CHECK(flags, ROBOTRACONTEURLITE_MESSAGE_FLAGS_MULTIPLE_ENTRIES))
    {
        temp_buffer_info.entry_count_offset = o;
        rv = robotraconteurlite_message_read_count2(reader->buffer, 4, &o, &header->entry_count);
        if (rv < 0)
        {
            return rv;
        }
    }
    else
    {
        header->entry_count = 1;
        temp_buffer_info.entry_count_offset = SIZE_MAX;
    }

    if (FLAGS_CHECK(flags, ROBOTRACONTEURLITE_MESSAGE_FLAGS_EXTENDED))
    {
        uint32_t extended_size = 0;
        rv = robotraconteurlite_message_read_count(reader->buffer, 4, &o, &extended_size);
        if (rv < 0)
        {
            return rv;
        }
        o += extended_size;
    }

    if ((o - start_o) != header->header_size)
    {
        return ROBOTRACONTEURLITE_ERROR_PROTOCOL;
    }

    if (FLAGS_CHECK(flags, ROBOTRACONTEURLITE_MESSAGE_FLAGS_ROUTING_INFO))
    {
        rv = robotraconteurlite_message_read_header_string_with_len_prefix2(reader->buffer, 4, &sender_nodename_info,
                                                                            &header->sender_nodename);
        if (rv < 0)
        {
            return rv;
        }

        rv = robotraconteurlite_message_read_header_string_with_len_prefix2(reader->buffer, 4, &receiver_nodename_info,
                                                                            &header->receiver_nodename);
        if (rv < 0)
        {
            return rv;
        }
    }
    else
    {
        header->sender_nodename.data = NULL;
        header->sender_nodename.len = 0;
        header->receiver_nodename.data = NULL;
        header->receiver_nodename.len = 0;
    }

    if (FLAGS_CHECK(flags, ROBOTRACONTEURLITE_MESSAGE_FLAGS_META_INFO))
    {
        rv = robotraconteurlite_message_read_header_string_with_len_prefix2(reader->buffer, 2, &meta_info,
                                                                            &header->metadata);
        if (rv < 0)
        {
            return rv;
        }
    }
    else
    {
        header->metadata.data = NULL;
        header->metadata.len = 0;
    }

    header->message_flags = flags;

    temp_buffer_info.entry_start_offset = o;

    *buffer_info = temp_buffer_info;

    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

robotraconteurlite_status robotraconteurlite_message_reader_read_header_ex(
    struct robotraconteurlite_message_reader* reader, struct robotraconteurlite_message_header* header,
    struct robotraconteurlite_message_buffer_info* buffer_info)
{
    size_t o = reader->buffer_offset;
    size_t start_o = o;
    robotraconteurlite_status rv = -1;
    struct robotraconteurlite_array_uint8 sender_nodeid_array;
    struct robotraconteurlite_array_uint8 recv_nodeid_array;
    struct robotraconteurlite_message_read_header_string_info sender_nodename_info;
    struct robotraconteurlite_message_read_header_string_info receiver_nodename_info;
    struct robotraconteurlite_message_read_header_string_info meta_info;

    struct robotraconteurlite_message_buffer_info temp_buffer_info;

    temp_buffer_info.start_buffer_offset = reader->buffer_offset;

    sender_nodeid_array.data = header->sender_nodeid.data;
    sender_nodeid_array.len = 16;
    recv_nodeid_array.data = header->receiver_nodeid.data;
    recv_nodeid_array.len = 16;

    /* skip RRAC magic */
    o += 4U;

    rv = robotraconteurlite_message_read_uint32(reader->buffer, &o, &header->message_size);
    if (rv < 0)
    {
        return rv;
    }

    rv = robotraconteurlite_message_read_uint16(reader->buffer, &o, &header->message_version);
    if (rv < 0)
    {
        return rv;
    }

    switch (header->message_version)
    {
    case 2:
        break;
    case 4:
        return robotraconteurlite_message_reader_read_header_ex4(reader, header, buffer_info);
    default:
        return ROBOTRACONTEURLITE_ERROR_PROTOCOL;
    }

    rv = robotraconteurlite_message_read_count2(reader->buffer, 2, &o, &header->header_size);
    if (rv < 0)
    {
        return rv;
    }

    temp_buffer_info.header_size = header->header_size;

    rv = robotraconteurlite_message_read_nodeid(reader->buffer, &o, &sender_nodeid_array);
    if (rv < 0)
    {
        return rv;
    }

    rv = robotraconteurlite_message_read_nodeid(reader->buffer, &o, &recv_nodeid_array);
    if (rv < 0)
    {
        return rv;
    }

    rv = robotraconteurlite_message_read_count(reader->buffer, 2, &o, &header->sender_endpoint);
    if (rv < 0)
    {
        return rv;
    }

    rv = robotraconteurlite_message_read_count(reader->buffer, 2, &o, &header->receiver_endpoint);
    if (rv < 0)
    {
        return rv;
    }

    rv = robotraconteurlite_message_read_header_string_with_len_prefix1(reader->buffer, 2, &o, &sender_nodename_info);
    if (rv < 0)
    {
        return rv;
    }

    rv = robotraconteurlite_message_read_header_string_with_len_prefix1(reader->buffer, 2, &o, &receiver_nodename_info);
    if (rv < 0)
    {
        return rv;
    }

    rv = robotraconteurlite_message_read_header_string_with_len_prefix1(reader->buffer, 2, &o, &meta_info);
    if (rv < 0)
    {
        return rv;
    }

    temp_buffer_info.entry_count_offset = o;

    rv = robotraconteurlite_message_read_count2(reader->buffer, 2, &o, &header->entry_count);
    if (rv < 0)
    {
        return rv;
    }

    rv = robotraconteurlite_message_read_uint16(reader->buffer, &o, &header->message_id);
    if (rv < 0)
    {
        return rv;
    }

    rv = robotraconteurlite_message_read_uint16(reader->buffer, &o, &header->message_res_id);
    if (rv < 0)
    {
        return rv;
    }

    if ((o - start_o) != header->header_size)
    {
        return ROBOTRACONTEURLITE_ERROR_PROTOCOL;
    }

    rv = robotraconteurlite_message_read_header_string_with_len_prefix2(reader->buffer, 2, &sender_nodename_info,
                                                                        &header->sender_nodename);
    if (rv < 0)
    {
        return rv;
    }

    rv = robotraconteurlite_message_read_header_string_with_len_prefix2(reader->buffer, 2, &receiver_nodename_info,
                                                                        &header->receiver_nodename);
    if (rv < 0)
    {
        return rv;
    }

    rv = robotraconteurlite_message_read_header_string_with_len_prefix2(reader->buffer, 2, &meta_info,
                                                                        &header->metadata);
    if (rv < 0)
    {
        return rv;
    }

    header->message_flags = ROBOTRACONTEURLITE_MESSAGE_FLAGS_VERSION2COMPAT;
    header->priority = 0;

    temp_buffer_info.entry_start_offset = o;

    *buffer_info = temp_buffer_info;

    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

robotraconteurlite_status robotraconteurlite_message_reader_begin_read_entries(
    struct robotraconteurlite_message_reader* reader, struct robotraconteurlite_messageentry_reader* entry_reader)
{
    size_t o = reader->buffer_offset;
    size_t o2 = 0;
    robotraconteurlite_status rv = -1;
    uint32_t entry_size = 0;
    struct robotraconteurlite_message_header message_header;
    struct robotraconteurlite_message_buffer_info message_buffer_info;
    memset(&message_header, 0, sizeof(message_header));

    rv = robotraconteurlite_message_reader_read_header_ex(reader, &message_header, &message_buffer_info);
    if (rv < 0)
    {
        return rv;
    }

    o += message_header.header_size;
    o2 = o;
    rv = robotraconteurlite_message_read_count(reader->buffer, reader->message_version, &o2, &entry_size);
    if (rv < 0)
    {
        return rv;
    }

    if ((o + entry_size) > (reader->buffer_count))
    {
        return ROBOTRACONTEURLITE_ERROR_PROTOCOL;
    }

    entry_reader->buffer = reader->buffer;
    entry_reader->buffer_offset = reader->buffer_offset + o;
    entry_reader->buffer_count = entry_size;
    entry_reader->buffer_remaining = reader->buffer_count - o - entry_size;
    entry_reader->current_entry = 0;
    entry_reader->entry_count_offset = message_buffer_info.entry_count_offset;
    entry_reader->message_version = reader->message_version;

    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

robotraconteurlite_status robotraconteurlite_messageentry_reader_move_next(
    struct robotraconteurlite_messageentry_reader* entry_reader)
{
    size_t o = entry_reader->buffer_offset + entry_reader->buffer_count;
    uint32_t entry_size = 0;
    robotraconteurlite_status rv = -1;

    uint32_t entry_count = 0;
    size_t o2 = entry_reader->entry_count_offset;

    if (o2 == SIZE_MAX)
    {
        return ROBOTRACONTEURLITE_ERROR_NO_MORE;
    }
    rv = robotraconteurlite_message_read_count2(entry_reader->buffer, entry_reader->message_version, &o2, &entry_count);
    if (rv < 0)
    {
        return rv;
    }

    if (!((entry_reader->current_entry + 1U) < entry_count))
    {
        return ROBOTRACONTEURLITE_ERROR_NO_MORE;
    }

    o2 = o;
    rv = robotraconteurlite_message_read_count(entry_reader->buffer, entry_reader->message_version, &o2, &entry_size);
    if (rv < 0)
    {
        return rv;
    }

    if (entry_reader->buffer_remaining < entry_size)
    {
        return ROBOTRACONTEURLITE_ERROR_PROTOCOL;
    }

    entry_reader->buffer_offset = o;
    entry_reader->buffer_count = entry_size;
    entry_reader->buffer_remaining -= entry_size;
    entry_reader->current_entry++;

    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

robotraconteurlite_status robotraconteurlite_messageentry_reader_find_element(
    struct robotraconteurlite_messageentry_reader* entry_reader, const struct robotraconteurlite_string* element_name,
    struct robotraconteurlite_messageelement_reader* element_reader)
{
    char temp_str_data[16];
    robotraconteurlite_status rv = -1;

    assert(element_name != NULL);
    assert(element_name->data != NULL);

    rv = robotraconteurlite_messageentry_reader_begin_read_elements(entry_reader, element_reader);
    if (rv < 0)
    {
        if (rv == ROBOTRACONTEURLITE_ERROR_NO_MORE)
        {
            return ROBOTRACONTEURLITE_ERROR_MESSAGEELEMENT_NOT_FOUND;
        }
        return rv;
    }
    while (1)
    {
        struct robotraconteurlite_messageelement_header element_header;
        struct robotraconteurlite_messageelement_buffer_info element_buffer_info;
        size_t str_len = 0;
        size_t str_offset = 0;
        memset(&element_header, 0, sizeof(element_header));
        memset(&element_buffer_info, 0, sizeof(element_buffer_info));

        rv = robotraconteurlite_messageelement_reader_read_header_ex(element_reader, &element_header,
                                                                     &element_buffer_info);

        if (rv < 0)
        {
            return rv;
        }
        str_len = element_header.element_name.len;

        if (str_len == element_name->len)
        {
            struct robotraconteurlite_string temp_str;
            temp_str.data = temp_str_data;
            temp_str.len = sizeof(temp_str_data);

            do
            {
                size_t str_read = ((str_len <= temp_str.len) ? str_len : temp_str.len);
                rv = robotraconteurlite_buffer_vec_copy_to_string(
                    element_reader->buffer, element_buffer_info.element_name_str_offset + str_offset, &temp_str, 0,
                    str_read);
                if (rv < 0)
                {
                    return rv;
                }

                if (memcmp(temp_str.data, &element_name->data[str_offset], str_read) != 0)
                {
                    break;
                }

                str_len -= str_read;
                if (str_len == 0U)
                {
                    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
                }
                str_offset += str_read;

            } while (str_len > 0U);
        }

        rv = robotraconteurlite_messageelement_reader_move_next(element_reader);
        if (rv < 0)
        {
            if (rv == ROBOTRACONTEURLITE_ERROR_NO_MORE)
            {
                return ROBOTRACONTEURLITE_ERROR_MESSAGEELEMENT_NOT_FOUND;
            }
            return rv;
        }
    }
}

robotraconteurlite_status robotraconteurlite_messageentry_reader_find_element_verify_scalar(
    struct robotraconteurlite_messageentry_reader* entry_reader, const struct robotraconteurlite_string* element_name,
    struct robotraconteurlite_messageelement_reader* element_reader, uint16_t expected_data_type)
{
    struct robotraconteurlite_messageelement_header header;
    robotraconteurlite_status rv = -1;

    rv = robotraconteurlite_messageentry_reader_find_element(entry_reader, element_name, element_reader);
    if (rv < 0)
    {
        return rv;
    }

    (void)memset(&header, 0, sizeof(header));
    rv = robotraconteurlite_messageelement_reader_read_header(element_reader, &header);
    if (rv < 0)
    {
        return rv;
    }

    if (header.element_type != expected_data_type)
    {
        return ROBOTRACONTEURLITE_ERROR_MESSAGEELEMENT_TYPE_MISMATCH;
    }

    if (header.data_count != 1U)
    {
        return ROBOTRACONTEURLITE_ERROR_MESSAGEELEMENT_TYPE_MISMATCH;
    }

    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

robotraconteurlite_status robotraconteurlite_messageentry_reader_find_element_verify_array(
    struct robotraconteurlite_messageentry_reader* entry_reader, const struct robotraconteurlite_string* element_name,
    struct robotraconteurlite_messageelement_reader* element_reader, uint16_t expected_data_type, uint32_t expected_len,
    uint8_t var_length)
{
    struct robotraconteurlite_messageelement_header header;
    robotraconteurlite_status rv = -1;

    rv = robotraconteurlite_messageentry_reader_find_element(entry_reader, element_name, element_reader);
    if (rv < 0)
    {
        return rv;
    }

    (void)memset(&header, 0, sizeof(header));
    rv = robotraconteurlite_messageelement_reader_read_header(element_reader, &header);
    if (rv < 0)
    {
        return rv;
    }

    if (header.element_type != expected_data_type)
    {
        return ROBOTRACONTEURLITE_ERROR_MESSAGEELEMENT_TYPE_MISMATCH;
    }

    if (!var_length)
    {
        if (header.data_count != expected_len)
        {
            return ROBOTRACONTEURLITE_ERROR_MESSAGEELEMENT_TYPE_MISMATCH;
        }
    }
    else
    {
        if (header.data_count > expected_len)
        {
            return ROBOTRACONTEURLITE_ERROR_OUT_OF_RANGE;
        }
    }

    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

robotraconteurlite_status robotraconteurlite_messageentry_reader_find_element_verify_string(
    struct robotraconteurlite_messageentry_reader* entry_reader, const struct robotraconteurlite_string* element_name,
    struct robotraconteurlite_messageelement_reader* element_reader, uint32_t expected_max_len)
{
    struct robotraconteurlite_messageelement_header header;
    robotraconteurlite_status rv = -1;

    rv = robotraconteurlite_messageentry_reader_find_element(entry_reader, element_name, element_reader);
    if (rv < 0)
    {
        return rv;
    }

    (void)memset(&header, 0, sizeof(header));
    rv = robotraconteurlite_messageelement_reader_read_header(element_reader, &header);
    if (rv < 0)
    {
        return rv;
    }

    if (header.element_type != ROBOTRACONTEURLITE_DATATYPE_STRING)
    {
        return ROBOTRACONTEURLITE_ERROR_MESSAGEELEMENT_TYPE_MISMATCH;
    }

    if (header.data_count > expected_max_len)
    {
        return ROBOTRACONTEURLITE_ERROR_OUT_OF_RANGE;
    }

    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

robotraconteurlite_status robotraconteurlite_messageentry_reader_read_header(
    struct robotraconteurlite_messageentry_reader* entry_reader, struct robotraconteurlite_messageentry_header* header)
{
    struct robotraconteurlite_messageentry_buffer_info buffer_info;
    return robotraconteurlite_messageentry_reader_read_header_ex(entry_reader, header, &buffer_info);
}

robotraconteurlite_status robotraconteurlite_messageentry_reader_read_header_ex(
    struct robotraconteurlite_messageentry_reader* entry_reader, struct robotraconteurlite_messageentry_header* header,
    struct robotraconteurlite_messageentry_buffer_info* buffer_info)
{
    size_t o = entry_reader->buffer_offset;
    robotraconteurlite_status rv = -1;
    struct robotraconteurlite_message_read_header_string_info service_path_info;
    struct robotraconteurlite_message_read_header_string_info member_name_info;
    struct robotraconteurlite_message_read_header_string_info meta_info;

    struct robotraconteurlite_messageentry_buffer_info temp_buffer_info;
    uint16_t ver = entry_reader->message_version;
    uint8_t flags = 0;

    memset(&service_path_info, 0, sizeof(service_path_info));
    memset(&member_name_info, 0, sizeof(member_name_info));
    memset(&meta_info, 0, sizeof(meta_info));

    temp_buffer_info.start_buffer_offset = entry_reader->buffer_offset;

    rv = robotraconteurlite_message_read_count(entry_reader->buffer, ver, &o, &header->entry_size);
    if (rv < 0)
    {
        return rv;
    }

    switch (ver)
    {
    case 2:
        flags = ROBOTRACONTEURLITE_MESSAGEENTRY_FLAGS_VERSION2COMPAT;
        break;
    case 4:
        rv = robotraconteurlite_message_read_uint8(entry_reader->buffer, &o, &flags);
        if (rv < 0)
        {
            return rv;
        }
        break;
    default:
        return ROBOTRACONTEURLITE_ERROR_PROTOCOL;
    }

    rv = robotraconteurlite_message_read_uint16(entry_reader->buffer, &o, &header->entry_type);
    if (rv < 0)
    {
        return rv;
    }

    switch (ver)
    {
    case 2:
        /* reserved */
        o += 2;
        break;
    default:
        break;
    }

    if (FLAGS_CHECK(flags, ROBOTRACONTEURLITE_MESSAGEENTRY_FLAGS_SERVICE_PATH_STR))
    {
        rv = robotraconteurlite_message_read_header_string_with_len_prefix1(entry_reader->buffer, ver, &o,
                                                                            &service_path_info);
        if (rv < 0)
        {
            return rv;
        }
    }

    if (FLAGS_CHECK(flags, ROBOTRACONTEURLITE_MESSAGEENTRY_FLAGS_SERVICE_PATH_CODE))
    {
        rv = robotraconteurlite_message_read_count(entry_reader->buffer, ver, &o, &header->service_path_code);
        if (rv < 0)
        {
            return rv;
        }
    }
    else
    {
        header->service_path_code = 0;
    }

    if (FLAGS_CHECK(flags, ROBOTRACONTEURLITE_MESSAGEENTRY_FLAGS_MEMBER_NAME_STR))
    {
        rv = robotraconteurlite_message_read_header_string_with_len_prefix1(entry_reader->buffer, ver, &o,
                                                                            &member_name_info);
        if (rv < 0)
        {
            return rv;
        }
    }

    if (FLAGS_CHECK(flags, ROBOTRACONTEURLITE_MESSAGEENTRY_FLAGS_MEMBER_NAME_CODE))
    {
        rv = robotraconteurlite_message_read_count(entry_reader->buffer, ver, &o, &header->member_name_code);
        if (rv < 0)
        {
            return rv;
        }
    }
    else
    {
        header->member_name_code = 0;
    }

    if (FLAGS_CHECK(flags, ROBOTRACONTEURLITE_MESSAGEENTRY_FLAGS_REQUEST_ID))
    {
        rv = robotraconteurlite_message_read_count(entry_reader->buffer, ver, &o, &header->request_id);
        if (rv < 0)
        {
            return rv;
        }
    }
    else
    {
        header->request_id = 0;
    }

    if (FLAGS_CHECK(flags, ROBOTRACONTEURLITE_MESSAGEENTRY_FLAGS_ERROR))
    {
        rv = robotraconteurlite_message_read_uint16(entry_reader->buffer, &o, &header->error);
        if (rv < 0)
        {
            return rv;
        }
    }
    else
    {
        header->error = 0;
    }

    if (FLAGS_CHECK(flags, ROBOTRACONTEURLITE_MESSAGEENTRY_FLAGS_META_INFO))
    {
        rv = robotraconteurlite_message_read_header_string_with_len_prefix1(entry_reader->buffer, ver, &o, &meta_info);
        if (rv < 0)
        {
            return rv;
        }
    }

    if (FLAGS_CHECK(flags, ROBOTRACONTEURLITE_MESSAGEENTRY_FLAGS_EXTENDED))
    {
        uint32_t extended_size = 0;
        rv = robotraconteurlite_message_read_count(entry_reader->buffer, ver, &o, &extended_size);
        if (rv < 0)
        {
            return rv;
        }
        o += extended_size;
    }

    temp_buffer_info.element_count_offset = o;
    rv = robotraconteurlite_message_read_count2(entry_reader->buffer, 2, &o, &header->element_count);
    if (rv < 0)
    {
        return rv;
    }

    if (FLAGS_CHECK(flags, ROBOTRACONTEURLITE_MESSAGEENTRY_FLAGS_SERVICE_PATH_STR))
    {
        rv = robotraconteurlite_message_read_header_string_with_len_prefix2(entry_reader->buffer, 2, &service_path_info,
                                                                            &header->service_path);
        if (rv < 0)
        {
            return rv;
        }
    }
    else
    {
        header->service_path.data = NULL;
        header->service_path.len = 0;
    }

    if (FLAGS_CHECK(flags, ROBOTRACONTEURLITE_MESSAGEENTRY_FLAGS_MEMBER_NAME_STR))
    {
        rv = robotraconteurlite_message_read_header_string_with_len_prefix2(entry_reader->buffer, 2, &member_name_info,
                                                                            &header->member_name);
        if (rv < 0)
        {
            return rv;
        }
    }
    else
    {
        header->member_name.data = NULL;
        header->member_name.len = 0;
    }
    if (FLAGS_CHECK(flags, ROBOTRACONTEURLITE_MESSAGEENTRY_FLAGS_META_INFO))
    {
        rv = robotraconteurlite_message_read_header_string_with_len_prefix2(entry_reader->buffer, 2, &meta_info,
                                                                            &header->metadata);
        if (rv < 0)
        {
            return rv;
        }
    }
    else
    {
        header->metadata.data = NULL;
        header->metadata.len = 0;
    }

    temp_buffer_info.header_size = o - entry_reader->buffer_offset;
    temp_buffer_info.element_start_offset = o;

    *buffer_info = temp_buffer_info;

    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

robotraconteurlite_status robotraconteurlite_messageentry_reader_begin_read_elements(
    struct robotraconteurlite_messageentry_reader* entry_reader,
    struct robotraconteurlite_messageelement_reader* element_reader)
{
    size_t o = entry_reader->buffer_offset;
    size_t o2 = 0;
    robotraconteurlite_status rv = -1;
    struct robotraconteurlite_messageentry_header entry_header;
    struct robotraconteurlite_messageentry_buffer_info entry_buffer_info;
    uint32_t element_size = 0;

    memset(&entry_header, 0, sizeof(entry_header));
    memset(&entry_buffer_info, 0, sizeof(entry_buffer_info));

    rv = robotraconteurlite_messageentry_reader_read_header_ex(entry_reader, &entry_header, &entry_buffer_info);
    if (rv < 0)
    {
        return rv;
    }

    if (entry_header.element_count == 0U)
    {
        return ROBOTRACONTEURLITE_ERROR_NO_MORE;
    }

    o = entry_buffer_info.element_start_offset;
    o2 = o;

    rv = robotraconteurlite_message_read_count(entry_reader->buffer, 2, &o2, &element_size);
    if (rv < 0)
    {
        return rv;
    }

    if (((o - entry_reader->buffer_offset) + element_size) > entry_reader->buffer_count)
    {
        return ROBOTRACONTEURLITE_ERROR_PROTOCOL;
    }

    element_reader->buffer = entry_reader->buffer;
    element_reader->buffer_offset = o;
    element_reader->buffer_count = element_size;
    element_reader->buffer_remaining = entry_reader->buffer_count - entry_buffer_info.header_size - element_size;
    element_reader->current_element = 0;
    element_reader->total_elements = entry_header.element_count;
    element_reader->message_version = entry_reader->message_version;

    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

robotraconteurlite_status robotraconteurlite_messageelement_reader_move_next(
    struct robotraconteurlite_messageelement_reader* element_reader)
{
    size_t o = (size_t)(element_reader->buffer_offset + element_reader->buffer_count);
    uint32_t element_size = 0;
    robotraconteurlite_status rv = -1;
    size_t o2 = 0;

    size_t element_count = element_reader->total_elements;

    if (!((element_reader->current_element + 1U) < element_count))
    {
        return ROBOTRACONTEURLITE_ERROR_NO_MORE;
    }

    o2 = o;
    rv = robotraconteurlite_message_read_count(element_reader->buffer, 2, &o2, &element_size);
    if (rv < 0)
    {
        return rv;
    }

    if (element_reader->buffer_remaining < element_size)
    {
        return ROBOTRACONTEURLITE_ERROR_PROTOCOL;
    }

    element_reader->buffer_offset = o;
    element_reader->buffer_count = element_size;
    element_reader->buffer_remaining -= element_size;
    element_reader->current_element++;

    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

robotraconteurlite_status robotraconteurlite_messageelement_reader_read_header(
    struct robotraconteurlite_messageelement_reader* element_reader,
    struct robotraconteurlite_messageelement_header* header)
{
    struct robotraconteurlite_messageelement_buffer_info buffer_info;
    return robotraconteurlite_messageelement_reader_read_header_ex(element_reader, header, &buffer_info);
}

robotraconteurlite_status robotraconteurlite_messageelement_reader_read_header_ex(
    struct robotraconteurlite_messageelement_reader* element_reader,
    struct robotraconteurlite_messageelement_header* header,
    struct robotraconteurlite_messageelement_buffer_info* buffer_info)
{
    size_t o = element_reader->buffer_offset;
    robotraconteurlite_status rv = -1;
    struct robotraconteurlite_message_read_header_string_info elementname_info;
    struct robotraconteurlite_message_read_header_string_info elementtypename_info;
    struct robotraconteurlite_message_read_header_string_info meta_info;

    struct robotraconteurlite_messageelement_buffer_info temp_buffer_info;

    uint16_t ver = element_reader->message_version;
    uint8_t flags = 0;

    memset(&elementname_info, 0, sizeof(elementname_info));
    memset(&elementtypename_info, 0, sizeof(elementtypename_info));
    memset(&meta_info, 0, sizeof(meta_info));

    temp_buffer_info.start_buffer_offset = element_reader->buffer_offset;

    rv = robotraconteurlite_message_read_count(element_reader->buffer, ver, &o, &header->element_size);
    if (rv < 0)
    {
        return rv;
    }

    switch (ver)
    {
    case 2:
        flags = ROBOTRACONTEURLITE_MESSAGEELEMENT_FLAGS_VERSION2COMPAT;
        break;
    case 4:
        rv = robotraconteurlite_message_read_uint8(element_reader->buffer, &o, &flags);
        if (rv < 0)
        {
            return rv;
        }
        break;
    default:
        return ROBOTRACONTEURLITE_ERROR_PROTOCOL;
    }

    if (FLAGS_CHECK(flags, ROBOTRACONTEURLITE_MESSAGEELEMENT_FLAGS_ELEMENT_NAME_STR))
    {
        rv = robotraconteurlite_message_read_header_string_with_len_prefix1(element_reader->buffer, ver, &o,
                                                                            &elementname_info);
        if (rv < 0)
        {
            return rv;
        }
    }

    if (FLAGS_CHECK(flags, ROBOTRACONTEURLITE_MESSAGEELEMENT_FLAGS_ELEMENT_NAME_CODE))
    {
        rv = robotraconteurlite_message_read_count(element_reader->buffer, ver, &o, &header->element_name_code);
        if (rv < 0)
        {
            return rv;
        }
    }
    else
    {
        header->element_name_code = 0;
    }

    if (FLAGS_CHECK(flags, ROBOTRACONTEURLITE_MESSAGEELEMENT_FLAGS_ELEMENT_NUMBER))
    {
        rv = robotraconteurlite_message_read_int_x(element_reader->buffer, &o, &header->element_number);
        if (rv < 0)
        {
            return rv;
        }
    }
    else
    {
        header->element_number = 0;
    }

    rv = robotraconteurlite_message_read_uint16(element_reader->buffer, &o, &header->element_type);
    if (rv < 0)
    {
        return rv;
    }

    if (FLAGS_CHECK(flags, ROBOTRACONTEURLITE_MESSAGEELEMENT_FLAGS_ELEMENT_TYPE_NAME_STR))
    {
        rv = robotraconteurlite_message_read_header_string_with_len_prefix1(element_reader->buffer, ver, &o,
                                                                            &elementtypename_info);
        if (rv < 0)
        {
            return rv;
        }
    }

    if (FLAGS_CHECK(flags, ROBOTRACONTEURLITE_MESSAGEELEMENT_FLAGS_ELEMENT_TYPE_NAME_CODE))
    {
        rv = robotraconteurlite_message_read_count(element_reader->buffer, ver, &o, &header->element_type_name_code);
        if (rv < 0)
        {
            return rv;
        }
    }
    else
    {
        header->element_type_name_code = 0;
    }

    if (FLAGS_CHECK(flags, ROBOTRACONTEURLITE_MESSAGEELEMENT_FLAGS_META_INFO))
    {
        rv =
            robotraconteurlite_message_read_header_string_with_len_prefix1(element_reader->buffer, ver, &o, &meta_info);
        if (rv < 0)
        {
            return rv;
        }
    }

    if (FLAGS_CHECK(flags, ROBOTRACONTEURLITE_MESSAGEELEMENT_FLAGS_EXTENDED))
    {
        uint32_t extended_size = 0;
        rv = robotraconteurlite_message_read_count(element_reader->buffer, ver, &o, &extended_size);
        if (rv < 0)
        {
            return rv;
        }
        o += extended_size;
    }

    temp_buffer_info.element_count_offset = o;

    rv = robotraconteurlite_message_read_count(element_reader->buffer, ver, &o, &header->data_count);
    if (rv < 0)
    {
        return rv;
    }

    if (FLAGS_CHECK(flags, ROBOTRACONTEURLITE_MESSAGEELEMENT_FLAGS_ELEMENT_NAME_STR))
    {
        rv = robotraconteurlite_message_read_header_string_with_len_prefix2(element_reader->buffer, ver,
                                                                            &elementname_info, &header->element_name);
        if (rv < 0)
        {
            return rv;
        }
        temp_buffer_info.element_name_str_offset = elementname_info.offset;
    }
    else
    {
        temp_buffer_info.element_name_str_offset = SIZE_MAX;

        header->element_name.data = NULL;
        header->element_name.len = 0;
    }

    if (FLAGS_CHECK(flags, ROBOTRACONTEURLITE_MESSAGEELEMENT_FLAGS_ELEMENT_TYPE_NAME_STR))
    {
        rv = robotraconteurlite_message_read_header_string_with_len_prefix2(
            element_reader->buffer, ver, &elementtypename_info, &header->element_type_name);
        if (rv < 0)
        {
            return rv;
        }
    }
    else
    {
        header->element_type_name.data = NULL;
        header->element_type_name.len = 0;
    }

    if (FLAGS_CHECK(flags, ROBOTRACONTEURLITE_MESSAGEELEMENT_FLAGS_META_INFO))
    {
        rv = robotraconteurlite_message_read_header_string_with_len_prefix2(element_reader->buffer, ver, &meta_info,
                                                                            &header->metadata);
        if (rv < 0)
        {
            return rv;
        }
    }
    else
    {
        header->metadata.data = NULL;
        header->metadata.len = 0;
    }

    temp_buffer_info.header_size = o - element_reader->buffer_offset;
    temp_buffer_info.data_start_offset = o;

    *buffer_info = temp_buffer_info;

    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

robotraconteurlite_status robotraconteurlite_messageelement_reader_get_data_info(
    struct robotraconteurlite_messageelement_reader* element_reader, size_t* data_offset, size_t* data_size,
    uint32_t* data_count, uint16_t data_type, size_t data_element_size)
{
    struct robotraconteurlite_messageelement_header element_header;
    struct robotraconteurlite_messageelement_buffer_info element_buffer_info;
    robotraconteurlite_status rv = -1;

    assert(element_reader != NULL);

    (void)memset(&element_header, 0, sizeof(element_header));
    (void)memset(&element_buffer_info, 0, sizeof(element_buffer_info));
    rv = robotraconteurlite_messageelement_reader_read_header_ex(element_reader, &element_header, &element_buffer_info);
    if (rv < 0)
    {
        return rv;
    }

    if (element_header.element_type != data_type)
    {
        return ROBOTRACONTEURLITE_ERROR_MESSAGEELEMENT_TYPE_MISMATCH;
    }

    *data_offset = element_buffer_info.data_start_offset;
    *data_size = element_header.element_size - element_buffer_info.header_size;
    *data_count = element_header.data_count;

    if ((data_element_size != 0U) && ((data_element_size * element_header.data_count) != *data_size))
    {
        return ROBOTRACONTEURLITE_ERROR_PROTOCOL;
    }

    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

robotraconteurlite_status robotraconteurlite_messageelement_reader_read_data_ex(
    struct robotraconteurlite_messageelement_reader* element_reader, uint8_t* dest_buf, size_t* dest_len,
    uint16_t dest_elem_type, size_t dest_elem_size)
{
    size_t data_offset = 0;
    size_t data_size = 0;
    uint32_t data_count = 0;
    robotraconteurlite_status rv = -1;

    assert(dest_buf != NULL);
    assert(dest_len != NULL);

    rv = robotraconteurlite_messageelement_reader_get_data_info(element_reader, &data_offset, &data_size, &data_count,
                                                                dest_elem_type, dest_elem_size);
    if (rv < 0)
    {
        return rv;
    }

    rv = robotraconteurlite_buffer_vec_copy_to_mem(element_reader->buffer, data_offset, dest_buf, *dest_len, 0,
                                                   dest_elem_size, data_count);

    if (rv < 0)
    {
        return rv;
    }

    *dest_len = data_count;

    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

robotraconteurlite_status robotraconteurlite_messageelement_reader_read_data_scalar_ex(
    struct robotraconteurlite_messageelement_reader* element_reader, uint8_t* dest_scalar, uint16_t dest_elem_type,
    size_t dest_elem_size)
{
    size_t data_offset = 0;
    size_t data_size = 0;
    uint32_t data_count = 0;
    robotraconteurlite_status rv = -1;

    struct robotraconteurlite_buffer dest_buf;
    struct robotraconteurlite_buffer_vec dest_vec;

    dest_buf.data = dest_scalar;
    dest_buf.len = 1;
    dest_vec.buffer_vec = &dest_buf;
    dest_vec.buffer_vec_cnt = 1;

    rv = robotraconteurlite_messageelement_reader_get_data_info(element_reader, &data_offset, &data_size, &data_count,
                                                                dest_elem_type, dest_elem_size);
    if (rv < 0)
    {
        return rv;
    }

    if (data_count != 1U)
    {
        return ROBOTRACONTEURLITE_ERROR_PROTOCOL;
    }

    return robotraconteurlite_buffer_vec_copy_vec_ex(element_reader->buffer, data_offset, 1, dest_elem_size, &dest_vec,
                                                     0, dest_elem_size, 1);
}

robotraconteurlite_status robotraconteurlite_messageelement_reader_begin_read_nested_elements(
    struct robotraconteurlite_messageelement_reader* element_reader,
    struct robotraconteurlite_messageelement_reader* nested_element_reader)
{
    robotraconteurlite_status rv = -1;
    struct robotraconteurlite_messageelement_header header;
    struct robotraconteurlite_messageelement_buffer_info buffer_info;
    size_t o = 0;
    uint32_t nested_element_size = 0;

    (void)memset(&header, 0, sizeof(header));
    (void)memset(&buffer_info, 0, sizeof(buffer_info));
    rv = robotraconteurlite_messageelement_reader_read_header_ex(element_reader, &header, &buffer_info);
    if (rv < 0)
    {
        return rv;
    }

    switch (header.element_type)
    {
    case ROBOTRACONTEURLITE_DATATYPE_STRUCTURE:
    case ROBOTRACONTEURLITE_DATATYPE_MAP_INT32:
    case ROBOTRACONTEURLITE_DATATYPE_MAP_STRING:
    case ROBOTRACONTEURLITE_DATATYPE_LIST:
    case ROBOTRACONTEURLITE_DATATYPE_MULTIDIMARRAY:
    case ROBOTRACONTEURLITE_DATATYPE_POD:
    case ROBOTRACONTEURLITE_DATATYPE_POD_ARRAY:
    case ROBOTRACONTEURLITE_DATATYPE_POD_MULTIDIMARRAY:
    case ROBOTRACONTEURLITE_DATATYPE_NAMEDARRAY_ARRAY:
    case ROBOTRACONTEURLITE_DATATYPE_NAMEDARRAY_MULTIDIMARRAY:
        break;
    default:
        return ROBOTRACONTEURLITE_ERROR_MESSAGEELEMENT_TYPE_MISMATCH;
    }

    if (header.data_count == 0U)
    {
        return ROBOTRACONTEURLITE_ERROR_NO_MORE;
    }

    o = buffer_info.data_start_offset;
    rv = robotraconteurlite_message_read_count(element_reader->buffer, 2, &o, &nested_element_size);
    if (rv < 0)
    {
        return rv;
    }

    if ((o + nested_element_size) > element_reader->buffer_count)
    {
        return ROBOTRACONTEURLITE_ERROR_PROTOCOL;
    }

    nested_element_reader->buffer = element_reader->buffer;
    nested_element_reader->buffer_offset = buffer_info.data_start_offset;
    nested_element_reader->buffer_count = nested_element_size;
    nested_element_reader->buffer_remaining =
        element_reader->buffer_count - buffer_info.header_size - nested_element_size;
    nested_element_reader->current_element = 0;
    nested_element_reader->total_elements = header.data_count;
    nested_element_reader->message_version = element_reader->message_version;

    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

robotraconteurlite_status robotraconteurlite_messageelement_reader_find_nested_element(
    struct robotraconteurlite_messageelement_reader* element_reader,
    const struct robotraconteurlite_string* nested_element_name,
    struct robotraconteurlite_messageelement_reader* nested_element_reader)
{
    char temp_str_data[16];
    robotraconteurlite_status rv = -1;

    assert(nested_element_name != NULL);
    assert(nested_element_name->data != NULL);

    rv = robotraconteurlite_messageelement_reader_begin_read_nested_elements(element_reader, nested_element_reader);
    if (rv < 0)
    {
        if (rv == ROBOTRACONTEURLITE_ERROR_NO_MORE)
        {
            return ROBOTRACONTEURLITE_ERROR_MESSAGEELEMENT_NOT_FOUND;
        }
        return rv;
    }
    while (1)
    {
        struct robotraconteurlite_messageelement_header element_header;
        struct robotraconteurlite_messageelement_buffer_info element_buffer_info;
        size_t str_len = 0;
        size_t str_offset = 0;
        memset(&element_header, 0, sizeof(element_header));
        memset(&element_buffer_info, 0, sizeof(element_buffer_info));

        rv = robotraconteurlite_messageelement_reader_read_header_ex(nested_element_reader, &element_header,
                                                                     &element_buffer_info);
        if (rv < 0)
        {
            return rv;
        }

        str_len = element_header.element_name.len;

        if (str_len == nested_element_name->len)
        {
            struct robotraconteurlite_string temp_str;
            temp_str.data = temp_str_data;
            temp_str.len = sizeof(temp_str_data);

            do
            {
                size_t str_read = ((str_len <= temp_str.len) ? str_len : temp_str.len);
                rv = robotraconteurlite_buffer_vec_copy_to_string(
                    nested_element_reader->buffer, element_buffer_info.element_name_str_offset + str_offset, &temp_str,
                    0, str_read);
                if (rv < 0)
                {
                    return rv;
                }

                if (memcmp(temp_str.data, &nested_element_name->data[str_offset], str_read) != 0)
                {
                    break;
                }

                str_len -= str_read;
                if (str_len == 0U)
                {
                    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
                }
                str_offset += str_read;

            } while (str_len > 0U);
        }

        rv = robotraconteurlite_messageelement_reader_move_next(nested_element_reader);
        if (rv < 0)
        {
            if (rv == ROBOTRACONTEURLITE_ERROR_NO_MORE)
            {
                return ROBOTRACONTEURLITE_ERROR_MESSAGEELEMENT_NOT_FOUND;
            }
            return rv;
        }
    }
}

robotraconteurlite_status robotraconteurlite_messageelement_reader_find_nested_element_verify_scalar(
    struct robotraconteurlite_messageelement_reader* element_reader,
    const struct robotraconteurlite_string* nested_element_name,
    struct robotraconteurlite_messageelement_reader* nested_element_reader, uint16_t expected_data_type)
{
    robotraconteurlite_status rv = -1;
    struct robotraconteurlite_messageelement_header header;
    rv = robotraconteurlite_messageelement_reader_find_nested_element(element_reader, nested_element_name,
                                                                      nested_element_reader);
    if (rv < 0)
    {
        return rv;
    }

    (void)memset(&header, 0, sizeof(header));
    rv = robotraconteurlite_messageelement_reader_read_header(nested_element_reader, &header);
    if (rv < 0)
    {
        return rv;
    }

    if (header.element_type != expected_data_type)
    {
        return ROBOTRACONTEURLITE_ERROR_MESSAGEELEMENT_TYPE_MISMATCH;
    }

    if (header.data_count != 1U)
    {
        return ROBOTRACONTEURLITE_ERROR_MESSAGEELEMENT_TYPE_MISMATCH;
    }

    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

robotraconteurlite_status robotraconteurlite_messageelement_reader_find_nested_element_verify_array(
    struct robotraconteurlite_messageelement_reader* element_reader,
    const struct robotraconteurlite_string* nested_element_name,
    struct robotraconteurlite_messageelement_reader* nested_element_reader, uint16_t expected_data_type,
    uint32_t expected_len, uint8_t var_length)
{
    struct robotraconteurlite_messageelement_header header;
    robotraconteurlite_status rv = -1;

    rv = robotraconteurlite_messageelement_reader_find_nested_element(element_reader, nested_element_name,
                                                                      nested_element_reader);
    if (rv < 0)
    {
        return rv;
    }

    (void)memset(&header, 0, sizeof(header));
    rv = robotraconteurlite_messageelement_reader_read_header(nested_element_reader, &header);
    if (rv < 0)
    {
        return rv;
    }

    if (header.element_type != expected_data_type)
    {
        return ROBOTRACONTEURLITE_ERROR_MESSAGEELEMENT_TYPE_MISMATCH;
    }

    if (!var_length)
    {
        if (header.data_count != expected_len)
        {
            return ROBOTRACONTEURLITE_ERROR_MESSAGEELEMENT_TYPE_MISMATCH;
        }
    }
    else
    {
        if (header.data_count > expected_len)
        {
            return ROBOTRACONTEURLITE_ERROR_OUT_OF_RANGE;
        }
    }

    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

robotraconteurlite_status robotraconteurlite_messageelement_reader_find_nested_element_verify_string(
    struct robotraconteurlite_messageelement_reader* element_reader,
    const struct robotraconteurlite_string* nested_element_name,
    struct robotraconteurlite_messageelement_reader* nested_element_reader, uint32_t expected_max_len)
{
    struct robotraconteurlite_messageelement_header header;
    robotraconteurlite_status rv = -1;
    rv = robotraconteurlite_messageelement_reader_find_nested_element(element_reader, nested_element_name,
                                                                      nested_element_reader);
    if (rv < 0)
    {
        return rv;
    }

    (void)memset(&header, 0, sizeof(header));
    rv = robotraconteurlite_messageelement_reader_read_header(nested_element_reader, &header);
    if (rv < 0)
    {
        return rv;
    }

    if (header.element_type != ROBOTRACONTEURLITE_DATATYPE_STRING)
    {
        return ROBOTRACONTEURLITE_ERROR_MESSAGEELEMENT_TYPE_MISMATCH;
    }

    if (header.data_count > expected_max_len)
    {
        return ROBOTRACONTEURLITE_ERROR_OUT_OF_RANGE;
    }

    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

/* Message Writer */

robotraconteurlite_status robotraconteurlite_message_writer_init(struct robotraconteurlite_message_writer* writer,
                                                                 struct robotraconteurlite_buffer_vec* buffer,
                                                                 size_t offset, size_t count)
{
    size_t buffer_len = 0;
    robotraconteurlite_status rv = -1;

    assert(writer != NULL);
    assert(buffer != NULL);

    /*if (buffer->buffer_vec_cnt == 1 && (buffer->buffer_vec != NULL) && (buffer->buffer_vec->data == NULL) &&
    (buffer->buffer_vec_cnt == 0))
    {
        writer->dry_run = 1;
    }
    else
    {*/
    rv = robotraconteurlite_buffer_vec_len(buffer, &buffer_len);
    if (rv < 0)
    {
        return rv;
    }

    if (buffer_len < (offset + count))
    {
        return ROBOTRACONTEURLITE_ERROR_OUT_OF_RANGE;
    }
    /*writer->dry_run = 0;
}*/

    writer->buffer = buffer;
    writer->buffer_offset = offset;
    writer->buffer_count = count;

    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

static robotraconteurlite_status robotraconteurlite_message_write_uint32(struct robotraconteurlite_buffer_vec* buffer,

                                                                         size_t* offset, uint32_t val)
{
    robotraconteurlite_status rv = robotraconteurlite_buffer_vec_copy_from_uint32(buffer, *offset, val);
    if (rv < 0)
    {
        return rv;
    }
    *offset += 4U;
    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

static robotraconteurlite_status robotraconteurlite_message_write_uint16(struct robotraconteurlite_buffer_vec* buffer,
                                                                         size_t* offset, uint16_t val)
{
    robotraconteurlite_status rv = robotraconteurlite_buffer_vec_copy_from_uint16(buffer, *offset, val);
    if (rv < 0)
    {
        return rv;
    }
    *offset += 2U;
    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

static robotraconteurlite_status robotraconteurlite_message_write_count(struct robotraconteurlite_buffer_vec* buffer,
                                                                        uint16_t ver, size_t* offset, uint32_t count)
{
    robotraconteurlite_status rv = robotraconteurlite_buffer_vec_copy_from_uint32(buffer, *offset, count);
    if (rv < 0)
    {
        return rv;
    }
    *offset += 4U;
    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

static robotraconteurlite_status robotraconteurlite_message_write_count2(struct robotraconteurlite_buffer_vec* buffer,
                                                                         uint16_t ver, size_t* offset, uint32_t count)
{
    uint16_t count16 = (uint16_t)count;
    robotraconteurlite_status rv = robotraconteurlite_buffer_vec_copy_from_uint16(buffer, *offset, count16);
    if (rv < 0)
    {
        return rv;
    }
    *offset += 2U;
    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

static robotraconteurlite_status robotraconteurlite_message_write_padded_count(
    struct robotraconteurlite_buffer_vec* buffer, uint16_t ver, size_t* offset, uint32_t count, size_t* count_offset)
{
    robotraconteurlite_status rv = robotraconteurlite_buffer_vec_copy_from_uint32(buffer, *offset, count);
    if (rv < 0)
    {
        return rv;
    }
    *count_offset = *offset;
    *offset += 4U;
    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

static robotraconteurlite_status robotraconteurlite_message_write_padded_count_val(
    struct robotraconteurlite_buffer_vec* buffer, uint16_t ver, size_t count_offset, uint32_t count)
{
    robotraconteurlite_status rv = robotraconteurlite_buffer_vec_copy_from_uint32(buffer, count_offset, count);
    return rv;
}

static robotraconteurlite_status robotraconteurlite_message_write_padded_count2(
    struct robotraconteurlite_buffer_vec* buffer, uint16_t ver, size_t* offset, uint32_t count, size_t* count_offset)
{
    uint16_t count16 = (uint16_t)count;
    robotraconteurlite_status rv = robotraconteurlite_buffer_vec_copy_from_uint16(buffer, *offset, count16);
    if (rv < 0)
    {
        return rv;
    }
    *count_offset = *offset;
    *offset += 2U;
    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

static robotraconteurlite_status robotraconteurlite_message_write_padded_count2_val(
    struct robotraconteurlite_buffer_vec* buffer, uint16_t ver, size_t count_offset, uint32_t count)
{
    uint16_t count16 = (uint16_t)count;
    robotraconteurlite_status rv = robotraconteurlite_buffer_vec_copy_from_uint16(buffer, count_offset, count16);
    return rv;
}

static robotraconteurlite_status robotraconteurlite_message_write_header_string_with_len_prefix(
    struct robotraconteurlite_buffer_vec* buffer, uint16_t ver, size_t* offset,
    const struct robotraconteurlite_string* str)
{
    robotraconteurlite_status rv = robotraconteurlite_buffer_vec_copy_from_uint16(buffer, *offset, (uint16_t)str->len);
    if (rv < 0)
    {
        return rv;
    }

    *offset += 2U;

    if (str->len > 0U)
    {
        rv = robotraconteurlite_buffer_vec_copy_from_string(buffer, *offset, str, 0, str->len);
        if (rv < 0)
        {
            return rv;
        }

        *offset += str->len;
    }

    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

robotraconteurlite_status robotraconteurlite_message_writer_begin_message(
    struct robotraconteurlite_message_writer* writer, struct robotraconteurlite_message_header* header,
    struct robotraconteurlite_messageentry_writer* entry_writer)
{
    size_t header_size_offset = 0;
    size_t header_size = 0;
    size_t message_size_offset = 0;
    size_t entry_count_offset = 0;
    robotraconteurlite_status rv = -1;
    struct robotraconteurlite_array_uint8 sender_nodeid_array;
    struct robotraconteurlite_array_uint8 recv_nodeid_array;
    size_t o = writer->buffer_offset;

    assert(header != NULL);
    assert(writer != NULL);
    assert(entry_writer != NULL);

    rv = robotraconteurlite_buffer_vec_copy_from_mem(writer->buffer, o, (const uint8_t*)rrac_magic, 4, 0, 1, 4);
    if (rv < 0)
    {
        return rv;
    }

    o += 4U;

    message_size_offset = o;
    rv = robotraconteurlite_message_write_uint32(writer->buffer, &o, 0);
    if (rv < 0)
    {
        return rv;
    }

    rv = robotraconteurlite_message_write_uint16(writer->buffer, &o, 2);
    if (rv < 0)
    {
        return rv;
    }

    rv = robotraconteurlite_message_write_padded_count2(writer->buffer, 2, &o, 0, &header_size_offset);
    if (rv < 0)
    {
        return rv;
    }

    sender_nodeid_array.data = header->sender_nodeid.data;
    sender_nodeid_array.len = 16;
    recv_nodeid_array.data = header->receiver_nodeid.data;
    recv_nodeid_array.len = 16;

    rv = robotraconteurlite_buffer_vec_copy_from_uint8_array(writer->buffer, o, &sender_nodeid_array, 0, 16);
    if (rv < 0)
    {
        return rv;
    }
    o += 16U;

    rv = robotraconteurlite_buffer_vec_copy_from_uint8_array(writer->buffer, o, &recv_nodeid_array, 0, 16);
    if (rv < 0)
    {
        return rv;
    }
    o += 16U;

    rv = robotraconteurlite_message_write_count(writer->buffer, 2, &o, header->sender_endpoint);
    if (rv < 0)
    {
        return rv;
    }

    rv = robotraconteurlite_message_write_count(writer->buffer, 2, &o, header->receiver_endpoint);
    if (rv < 0)
    {
        return rv;
    }

    rv =
        robotraconteurlite_message_write_header_string_with_len_prefix(writer->buffer, 2, &o, &header->sender_nodename);
    if (rv < 0)
    {
        return rv;
    }

    rv = robotraconteurlite_message_write_header_string_with_len_prefix(writer->buffer, 2, &o,
                                                                        &header->receiver_nodename);
    if (rv < 0)
    {
        return rv;
    }

    rv = robotraconteurlite_message_write_header_string_with_len_prefix(writer->buffer, 2, &o, &header->metadata);
    if (rv < 0)
    {
        return rv;
    }

    rv =
        robotraconteurlite_message_write_padded_count2(writer->buffer, 2, &o, header->entry_count, &entry_count_offset);
    if (rv < 0)
    {
        return rv;
    }

    rv = robotraconteurlite_message_write_uint16(writer->buffer, &o, header->message_id);
    if (rv < 0)
    {
        return rv;
    }

    rv = robotraconteurlite_message_write_uint16(writer->buffer, &o, header->message_res_id);
    if (rv < 0)
    {
        return rv;
    }

    header_size = o - writer->buffer_offset;
    rv = robotraconteurlite_message_write_padded_count2_val(writer->buffer, 2, header_size_offset, header_size);
    if (rv < 0)
    {
        return rv;
    }

    header->header_size = header_size;

    entry_writer->buffer = writer->buffer;
    entry_writer->buffer_offset = writer->buffer_offset + header_size;
    entry_writer->buffer_count = writer->buffer_count - header_size;
    entry_writer->entries_written_count = 0;
    entry_writer->entries_written_size = 0;
    writer->message_size_offset = message_size_offset;
    writer->entry_count_offset = entry_count_offset;

    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

robotraconteurlite_status robotraconteurlite_message_writer_end_message(
    struct robotraconteurlite_message_writer* writer, struct robotraconteurlite_message_header* header,
    struct robotraconteurlite_messageentry_writer* entry_writer)
{
    size_t message_size = 0;
    robotraconteurlite_status rv = -1;

    assert(writer != NULL);
    assert(header != NULL);
    assert(entry_writer != NULL);

    message_size = entry_writer->entries_written_size + header->header_size;

    rv = robotraconteurlite_buffer_vec_copy_from_uint32(writer->buffer, writer->message_size_offset, message_size);
    if (rv < 0)
    {
        return rv;
    }

    rv = robotraconteurlite_message_write_padded_count2_val(writer->buffer, 2, writer->entry_count_offset,
                                                            (uint32_t)entry_writer->entries_written_count);
    if (rv < 0)
    {
        return rv;
    }

    header->message_size = message_size;
    header->entry_count = (uint16_t)entry_writer->entries_written_count;

    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

robotraconteurlite_status robotraconteurlite_messageentry_writer_begin_entry(
    struct robotraconteurlite_messageentry_writer* entry_writer, struct robotraconteurlite_messageentry_header* header,
    struct robotraconteurlite_messageelement_writer* element_writer)
{
    size_t header_size = 0;
    robotraconteurlite_status rv = -1;
    size_t entry_size_offset = 0;
    size_t element_count_offset = 0;
    size_t o = entry_writer->buffer_offset;

    assert(entry_writer != NULL);
    assert(header != NULL);
    assert(element_writer != NULL);

    if (!(entry_writer->entries_written_count < UINT16_MAX))
    {
        return ROBOTRACONTEURLITE_ERROR_PROTOCOL;
    }

    rv = robotraconteurlite_message_write_padded_count(entry_writer->buffer, 2, &o, 0, &entry_size_offset);
    if (rv < 0)
    {
        return rv;
    }

    rv = robotraconteurlite_message_write_uint16(entry_writer->buffer, &o, header->entry_type);
    if (rv < 0)
    {
        return rv;
    }

    /* pad */
    rv = robotraconteurlite_message_write_uint16(entry_writer->buffer, &o, 0);
    if (rv < 0)
    {
        return rv;
    }
    rv = robotraconteurlite_message_write_header_string_with_len_prefix(entry_writer->buffer, 2, &o,
                                                                        &header->service_path);
    if (rv < 0)
    {
        return rv;
    }

    rv = robotraconteurlite_message_write_header_string_with_len_prefix(entry_writer->buffer, 2, &o,
                                                                        &header->member_name);
    if (rv < 0)
    {
        return rv;
    }

    rv = robotraconteurlite_message_write_count(entry_writer->buffer, 2, &o, header->request_id);

    if (rv < 0)
    {
        return rv;
    }

    rv = robotraconteurlite_message_write_uint16(entry_writer->buffer, &o, header->error);
    if (rv < 0)
    {
        return rv;
    }

    rv = robotraconteurlite_message_write_header_string_with_len_prefix(entry_writer->buffer, 2, &o, &header->metadata);
    if (rv < 0)
    {
        return rv;
    }

    rv = robotraconteurlite_message_write_padded_count2(entry_writer->buffer, 2, &o, 0, &element_count_offset);
    if (rv < 0)
    {
        return rv;
    }

    header_size = o - entry_writer->buffer_offset;

    element_writer->buffer = entry_writer->buffer;
    element_writer->buffer_offset = entry_writer->buffer_offset + header_size;
    element_writer->buffer_count = entry_writer->buffer_count - header_size;
    element_writer->elements_written_count = 0;
    element_writer->elements_written_size = 0;
    entry_writer->entry_size_offset = entry_size_offset;
    entry_writer->element_count_offset = element_count_offset;
    entry_writer->entry_header_size = header_size;

    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

robotraconteurlite_status robotraconteurlite_messageentry_writer_end_entry(
    struct robotraconteurlite_messageentry_writer* entry_writer, struct robotraconteurlite_messageentry_header* header,
    struct robotraconteurlite_messageelement_writer* element_writer)
{

    robotraconteurlite_status rv = -1;
    uint32_t entry_size = 0;

    assert(entry_writer != NULL);
    assert(header != NULL);
    assert(element_writer != NULL);

    entry_size = (uint32_t)(entry_writer->entry_header_size + element_writer->elements_written_size);

    rv = robotraconteurlite_message_write_padded_count_val(entry_writer->buffer, 2, entry_writer->entry_size_offset,
                                                           entry_size);
    if (rv < 0)
    {
        return rv;
    }

    rv = robotraconteurlite_message_write_padded_count2_val(entry_writer->buffer, 2, entry_writer->element_count_offset,
                                                            (uint32_t)element_writer->elements_written_count);
    if (rv < 0)
    {
        return rv;
    }

    entry_writer->entries_written_count++;
    entry_writer->entries_written_size += entry_size;
    entry_writer->buffer_offset += entry_size;
    entry_writer->buffer_count -= entry_size;

    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

robotraconteurlite_status robotraconteurlite_messageelement_writer_begin_nested_element(
    struct robotraconteurlite_messageelement_writer* element_writer,
    struct robotraconteurlite_messageelement_header* header,
    struct robotraconteurlite_messageelement_writer* nested_element_writer)
{

    size_t header_size = 0;
    robotraconteurlite_status rv = -1;
    size_t o = element_writer->buffer_offset;
    size_t element_size_offset = 0;
    size_t data_count_offset = 0;

    assert(element_writer != NULL);
    assert(header != NULL);
    assert(nested_element_writer != NULL);

    if (!(element_writer->elements_written_count < UINT32_MAX))
    {
        return ROBOTRACONTEURLITE_ERROR_PROTOCOL;
    }

    rv = robotraconteurlite_message_write_padded_count(element_writer->buffer, 2, &o, 0, &element_size_offset);
    if (rv < 0)
    {
        return rv;
    }

    rv = robotraconteurlite_message_write_header_string_with_len_prefix(element_writer->buffer, 2, &o,
                                                                        &header->element_name);
    if (rv < 0)
    {
        return rv;
    }

    rv = robotraconteurlite_message_write_uint16(element_writer->buffer, &o, header->element_type);
    if (rv < 0)
    {
        return rv;
    }

    rv = robotraconteurlite_message_write_header_string_with_len_prefix(element_writer->buffer, 2, &o,
                                                                        &header->element_type_name);
    if (rv < 0)
    {
        return rv;
    }

    rv = robotraconteurlite_message_write_header_string_with_len_prefix(element_writer->buffer, 2, &o,
                                                                        &header->metadata);

    if (rv < 0)
    {
        return rv;
    }

    rv = robotraconteurlite_message_write_padded_count(element_writer->buffer, 2, &o, 0, &data_count_offset);
    if (rv < 0)
    {
        return rv;
    }

    header_size = o - element_writer->buffer_offset;

    nested_element_writer->buffer = element_writer->buffer;
    nested_element_writer->buffer_offset = element_writer->buffer_offset + header_size;
    nested_element_writer->buffer_count = element_writer->buffer_count - header_size;
    nested_element_writer->elements_written_count = 0;
    nested_element_writer->elements_written_size = 0;
    element_writer->element_size_offset = element_size_offset;
    element_writer->data_count_offset = data_count_offset;
    element_writer->element_header_size = header_size;

    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

robotraconteurlite_status robotraconteurlite_messageelement_writer_end_nested_element(
    struct robotraconteurlite_messageelement_writer* element_writer,
    struct robotraconteurlite_messageelement_header* header,
    struct robotraconteurlite_messageelement_writer* nested_element_writer)
{
    robotraconteurlite_status rv = -1;
    size_t element_size = 0;

    assert(element_writer != NULL);
    assert(header != NULL);
    assert(nested_element_writer != NULL);

    element_size = element_writer->element_header_size + nested_element_writer->elements_written_size;

    rv = robotraconteurlite_message_write_padded_count_val(element_writer->buffer, 2,
                                                           element_writer->element_size_offset, element_size);
    if (rv < 0)
    {
        return rv;
    }

    rv = robotraconteurlite_message_write_padded_count_val(element_writer->buffer, 2, element_writer->data_count_offset,
                                                           (uint32_t)nested_element_writer->elements_written_count);

    if (rv < 0)
    {
        return rv;
    }

    element_writer->elements_written_count++;
    element_writer->elements_written_size += element_size;
    element_writer->buffer_offset += element_size;
    element_writer->buffer_count -= element_size;

    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

robotraconteurlite_status robotraconteurlite_messageelement_writer_write_raw(
    struct robotraconteurlite_messageelement_writer* element_writer,
    const struct robotraconteurlite_string* element_name, const uint8_t* data_buf, size_t data_len, uint16_t data_type,
    size_t data_elem_size)
{

    size_t str1_len = 0;
    size_t data_size = 0;
    size_t elem_size = 0;
    robotraconteurlite_status rv = -1;
    size_t o = element_writer->buffer_offset;

    assert(element_writer != NULL);
    assert(element_name != NULL);
    assert(data_buf != NULL);

    if (element_name->len > UINT16_MAX)
    {
        return ROBOTRACONTEURLITE_ERROR_OUT_OF_RANGE;
    }

    if (!(element_writer->elements_written_count < UINT32_MAX))
    {
        return ROBOTRACONTEURLITE_ERROR_PROTOCOL;
    }

    str1_len = element_name->len;
    data_size = data_len * data_elem_size;
    elem_size = 16U + str1_len + data_size;
    if (elem_size > UINT32_MAX)
    {
        return ROBOTRACONTEURLITE_ERROR_OUT_OF_RANGE;
    }

    rv = robotraconteurlite_message_write_count(element_writer->buffer, 2, &o, (uint32_t)elem_size);
    if (rv < 0)
    {
        return rv;
    }

    rv = robotraconteurlite_message_write_header_string_with_len_prefix(element_writer->buffer, 2, &o, element_name);

    if (rv < 0)
    {
        return rv;
    }

    rv = robotraconteurlite_message_write_uint16(element_writer->buffer, &o, data_type);
    if (rv < 0)
    {
        return rv;
    }

    /* No element type name */
    rv = robotraconteurlite_message_write_uint16(element_writer->buffer, &o, 0);
    if (rv < 0)
    {
        return rv;
    }

    /* No metadata */

    rv = robotraconteurlite_message_write_uint16(element_writer->buffer, &o, 0);
    if (rv < 0)
    {
        return rv;
    }

    rv = robotraconteurlite_message_write_count(element_writer->buffer, 2, &o, data_len);
    if (rv < 0)
    {
        return rv;
    }

    rv = robotraconteurlite_buffer_vec_copy_from_mem(element_writer->buffer, o, data_buf, data_len, 0, data_elem_size,
                                                     data_len);

    if (rv < 0)
    {
        return rv;
    }

    element_writer->elements_written_count++;
    element_writer->elements_written_size += elem_size;
    element_writer->buffer_offset += elem_size;
    element_writer->buffer_count -= elem_size;

    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}
