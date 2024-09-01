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

#include <robotraconteurlite/robotraconteurlite.h>
#include <string.h>
#include <assert.h>

robotraconteurlite_status robotraconteurlite_buffer_init_scalar(struct robotraconteurlite_buffer* buffer, void* data,
                                                                size_t len)
{
    assert(buffer != NULL);
    assert(data != NULL);

    /* TODO: Fix cppcheck warning */
    /* cppcheck-suppress misra-c2012-11.5 */
    buffer->data = (uint8_t*)data;
    buffer->len = len;
    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

robotraconteurlite_status robotraconteurlite_buffer_vec_init_scalar(struct robotraconteurlite_buffer_vec* buffer_vec,
                                                                    struct robotraconteurlite_buffer* buffer)
{
    assert(buffer_vec != NULL);
    assert(buffer != NULL);

    buffer_vec->buffer_vec = (struct robotraconteurlite_buffer*)buffer;
    buffer_vec->buffer_vec_cnt = 1;
    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

robotraconteurlite_status robotraconteurlite_buffer_len(const struct robotraconteurlite_buffer* source, size_t* len)
{
    assert(source != NULL);
    assert(len != NULL);

    *len = source->len;
    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

robotraconteurlite_status robotraconteurlite_buffer_vec_len(const struct robotraconteurlite_buffer_vec* source,
                                                            size_t* len)
{
    size_t i = 0;

    assert(source != NULL);
    assert(len != NULL);

    *len = 0;
    for (i = 0; i < source->buffer_vec_cnt; i++)
    {
        *len += source->buffer_vec[i].len;
    }
    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

robotraconteurlite_status robotraconteurlite_buffer_copy(const struct robotraconteurlite_buffer* source,
                                                         size_t source_pos,
                                                         const struct robotraconteurlite_buffer* dest, size_t dest_pos,
                                                         size_t count)
{
    assert(source != NULL);
    assert(source->data != NULL);
    assert(dest != NULL);
    assert(dest->data != NULL);

    if (count == 0U)
    {
        return ROBOTRACONTEURLITE_ERROR_SUCCESS;
    }

    if (!(source_pos < source->len) || ((source_pos + count) > source->len) || !(dest_pos < dest->len) ||
        ((dest_pos + count) > dest->len))
    {
        return ROBOTRACONTEURLITE_ERROR_OUT_OF_RANGE;
    }

    (void)memcpy(dest->data + dest_pos, source->data + source_pos, count);
    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

robotraconteurlite_status robotraconteurlite_buffer_vec_copy_vec(const struct robotraconteurlite_buffer_vec* source,
                                                                 size_t source_pos,
                                                                 const struct robotraconteurlite_buffer_vec* dest,
                                                                 size_t dest_pos, size_t count)
{
    size_t source_len = 0;
    size_t dest_len = 0;
    size_t source_current_pos = 0;
    size_t dest_current_pos = 0;
    size_t i = 0;
    size_t source_i = 0;
    size_t dest_i = 0;
    size_t source_i_pos = 0;
    size_t dest_i_pos = 0;
    size_t count_remaining = 0;

    assert(source != NULL);
    assert(dest != NULL);
    assert(source->buffer_vec != NULL);
    assert(dest->buffer_vec != NULL);

    if ((source->buffer_vec_cnt == 1U) && (dest->buffer_vec_cnt == 1U))
    {
        return robotraconteurlite_buffer_copy(&source->buffer_vec[0], source_pos, &dest->buffer_vec[0], dest_pos,
                                              count);
    }

    for (i = 0; i < source->buffer_vec_cnt; i++)
    {
        source_len += source->buffer_vec[i].len;
    }

    for (i = 0; i < dest->buffer_vec_cnt; i++)
    {
        dest_len += dest->buffer_vec[i].len;
    }

    if (!(source_pos < source_len) || ((source_pos + count) > source_len) || !(dest_pos < dest_len) ||
        ((dest_pos + count) > dest_len))
    {
        return ROBOTRACONTEURLITE_ERROR_OUT_OF_RANGE;
    }

    source_i = 0;
    dest_i = 0;
    source_i_pos = 0;
    dest_i_pos = 0;
    while ((source_current_pos + source->buffer_vec[source_i].len) < source_pos)
    {
        source_current_pos += source->buffer_vec[source_i].len;
        source_i++;
    }

    while ((dest_current_pos + dest->buffer_vec[dest_i].len) < dest_pos)
    {
        dest_current_pos += dest->buffer_vec[dest_i].len;
        dest_i++;
    }

    if (source_current_pos < source_pos)
    {
        source_i_pos = source_pos - source_current_pos;
    }

    if (dest_current_pos < dest_pos)
    {
        dest_i_pos = dest_pos - dest_current_pos;
    }

    count_remaining = count;
    while (count_remaining > 0U)
    {
        size_t source_i_delta = source->buffer_vec[source_i].len - source_i_pos;
        size_t dest_i_delta = dest->buffer_vec[dest_i].len - dest_i_pos;
        size_t delta = 0;

        if (source_i_delta <= 0U)
        {
            source_i++;
            source_i_pos = 0;
            continue;
        }
        if (dest_i_delta <= 0U)
        {
            dest_i++;
            dest_i_pos = 0;
            continue;
        }

        delta = (source_i_delta < dest_i_delta) ? source_i_delta : dest_i_delta;
        delta = (count_remaining < delta) ? count_remaining : delta;

        assert(source->buffer_vec[source_i].data != NULL);
        assert(dest->buffer_vec[dest_i].data != NULL);

        (void)memcpy(dest->buffer_vec[dest_i].data + dest_i_pos, source->buffer_vec[source_i].data + source_i_pos,
                     delta);

        count_remaining -= delta;
        source_i_pos += delta;
        dest_i_pos += delta;
    }

    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

robotraconteurlite_status robotraconteurlite_buffer_copy_ex(const struct robotraconteurlite_buffer* source,
                                                            size_t source_pos, size_t source_elem_size,
                                                            size_t source_count,
                                                            const struct robotraconteurlite_buffer* dest,
                                                            size_t dest_pos, size_t dest_elem_size, size_t dest_count)
{
    assert(source != NULL);
    assert(source->data != NULL);
    assert(dest != NULL);
    assert(dest->data != NULL);

    if ((source_count == 0U) && (dest_count == 0U))
    {
        return ROBOTRACONTEURLITE_ERROR_SUCCESS;
    }

    if ((source_count * source_elem_size) != (dest_count * dest_elem_size))
    {
        return ROBOTRACONTEURLITE_ERROR_OUT_OF_RANGE;
    }

    if (!(source_pos < source->len) || ((source_pos + source_count) > source->len) || !(dest_pos < dest->len) ||
        ((dest_pos + dest_count) > dest->len))
    {
        return ROBOTRACONTEURLITE_ERROR_OUT_OF_RANGE;
    }

    (void)memcpy(dest->data + (dest_pos * dest_elem_size), source->data + (source_pos * source_elem_size),
                 source_count * source_elem_size);
    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

robotraconteurlite_status robotraconteurlite_buffer_vec_copy_vec_ex(
    const struct robotraconteurlite_buffer_vec* source, size_t source_pos, size_t source_elem_size, size_t source_count,
    const struct robotraconteurlite_buffer_vec* dest, size_t dest_pos, size_t dest_elem_size, size_t dest_count)
{
    size_t source_len = 0;
    size_t dest_len = 0;
    size_t source_current_byte_pos = 0;
    size_t dest_current_byte_pos = 0;
    size_t i = 0;
    size_t source_i = 0;
    size_t dest_i = 0;
    size_t source_i_byte_pos = 0;
    size_t dest_i_byte_pos = 0;
    size_t bytes_remaining = 0;
    size_t source_byte_pos = 0;
    size_t dest_byte_pos = 0;

    assert(source != NULL);
    assert(dest != NULL);
    assert(source->buffer_vec != NULL);
    assert(dest->buffer_vec != NULL);

    if ((source->buffer_vec_cnt == 1U) && (dest->buffer_vec_cnt == 1U))
    {
        return robotraconteurlite_buffer_copy_ex(&source->buffer_vec[0], source_pos, source_elem_size, source_count,
                                                 &dest->buffer_vec[0], dest_pos, dest_elem_size, dest_count);
    }

    if ((source_count * source_elem_size) != (dest_count * dest_elem_size))
    {
        return ROBOTRACONTEURLITE_ERROR_OUT_OF_RANGE;
    }

    for (i = 0; i < source->buffer_vec_cnt; i++)
    {
        source_len += source->buffer_vec[i].len;
    }

    for (i = 0; i < dest->buffer_vec_cnt; i++)
    {
        dest_len += dest->buffer_vec[i].len;
    }

    if (!(source_pos < source_len) || ((source_pos + source_count) > source_len) || !(dest_pos < dest_len) ||
        ((dest_pos + dest_count) > dest_len))
    {
        return ROBOTRACONTEURLITE_ERROR_OUT_OF_RANGE;
    }

    source_byte_pos = source_pos * source_elem_size;
    dest_byte_pos = dest_pos * dest_elem_size;

    source_i = 0;
    dest_i = 0;
    source_i_byte_pos = 0;
    dest_i_byte_pos = 0;
    while (source_current_byte_pos + (source->buffer_vec[source_i].len * source_elem_size) < source_byte_pos)
    {
        source_current_byte_pos += source->buffer_vec[source_i].len * source_elem_size;
        source_i++;
    }

    while (dest_current_byte_pos + (dest->buffer_vec[dest_i].len * dest_elem_size) < dest_byte_pos)
    {
        dest_current_byte_pos += dest->buffer_vec[dest_i].len * dest_elem_size;
        dest_i++;
    }

    if (source_current_byte_pos < source_byte_pos)
    {
        source_i_byte_pos = source_byte_pos - source_current_byte_pos;
    }

    if (dest_current_byte_pos < dest_byte_pos)
    {
        dest_i_byte_pos = dest_byte_pos - dest_current_byte_pos;
    }

    bytes_remaining = source_count * source_elem_size;
    while (bytes_remaining > 0U)
    {
        size_t source_i_delta = (source->buffer_vec[source_i].len * source_elem_size) - source_i_byte_pos;
        size_t dest_i_delta = (dest->buffer_vec[dest_i].len * dest_elem_size) - dest_i_byte_pos;
        size_t delta = 0;

        if (source_i_delta <= 0U)
        {
            source_i++;
            source_i_byte_pos = 0;
            continue;
        }
        if (dest_i_delta <= 0U)
        {
            dest_i++;
            dest_i_byte_pos = 0;
            continue;
        }

        delta = (source_i_delta < dest_i_delta) ? source_i_delta : dest_i_delta;
        delta = (bytes_remaining < delta) ? bytes_remaining : delta;

        assert(source->buffer_vec[source_i].data != NULL);
        assert(dest->buffer_vec[dest_i].data != NULL);

        (void)memcpy(dest->buffer_vec[dest_i].data + dest_i_byte_pos,
                     source->buffer_vec[source_i].data + source_i_byte_pos, delta);

        bytes_remaining -= delta;
        source_i_byte_pos += delta;
        dest_i_byte_pos += delta;
    }

    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

robotraconteurlite_status robotraconteurlite_buffer_vec_copy_to_mem(const struct robotraconteurlite_buffer_vec* source,
                                                                    size_t source_pos, uint8_t* dest, size_t dest_len,
                                                                    size_t dest_pos, size_t dest_elem_size,
                                                                    size_t dest_count)
{
    struct robotraconteurlite_buffer dest_buf;
    struct robotraconteurlite_buffer_vec dest_vec;
    (void)memset(&dest_buf, 0, sizeof(dest_buf));
    (void)memset(&dest_vec, 0, sizeof(dest_vec));
    dest_buf.data = dest;
    dest_buf.len = dest_len;
    dest_vec.buffer_vec = &dest_buf;
    dest_vec.buffer_vec_cnt = 1;

    /* TODO: More efficient implementation of this operation */
    return robotraconteurlite_buffer_vec_copy_vec_ex(source, source_pos, 1, dest_count * dest_elem_size, &dest_vec,
                                                     dest_pos, dest_elem_size, dest_count);
}

robotraconteurlite_status robotraconteurlite_buffer_vec_copy_from_mem(const struct robotraconteurlite_buffer_vec* dest,
                                                                      size_t dest_pos, const uint8_t* source,
                                                                      size_t source_len, size_t source_pos,
                                                                      size_t source_elem_size, size_t source_count)
{
    struct robotraconteurlite_buffer source_buf;
    struct robotraconteurlite_buffer_vec source_vec;
    (void)memset(&source_buf, 0, sizeof(source_buf));
    (void)memset(&source_vec, 0, sizeof(source_vec));
    source_buf.data = (uint8_t*)source;
    source_buf.len = source_len;
    source_vec.buffer_vec = &source_buf;
    source_vec.buffer_vec_cnt = 1;

    return robotraconteurlite_buffer_vec_copy_vec_ex(&source_vec, source_pos, source_elem_size, source_count, dest,
                                                     dest_pos, 1, source_count * source_elem_size);
}

/* string */
robotraconteurlite_status robotraconteurlite_buffer_vec_copy_to_string(
    const struct robotraconteurlite_buffer_vec* source_buf, size_t source_buf_pos,
    struct robotraconteurlite_string* dest, size_t dest_pos, size_t dest_count)
{
    struct robotraconteurlite_buffer_vec dest_vec;
    (void)memset(&dest_vec, 0, sizeof(dest_vec));
    dest_vec.buffer_vec = (struct robotraconteurlite_buffer*)dest;
    dest_vec.buffer_vec_cnt = 1;
    return robotraconteurlite_buffer_vec_copy_vec_ex(source_buf, source_buf_pos, 1, dest_count * sizeof(char),
                                                     &dest_vec, dest_pos, sizeof(char), dest_count);
}

robotraconteurlite_status robotraconteurlite_buffer_vec_copy_from_string(struct robotraconteurlite_buffer_vec* dest_buf,
                                                                         size_t dest_buf_pos,
                                                                         const struct robotraconteurlite_string* source,
                                                                         size_t source_pos, size_t source_count)
{
    struct robotraconteurlite_buffer_vec source_vec;
    (void)memset(&source_vec, 0, sizeof(source_vec));
    source_vec.buffer_vec = (struct robotraconteurlite_buffer*)source;
    source_vec.buffer_vec_cnt = 1;
    return robotraconteurlite_buffer_vec_copy_vec_ex(&source_vec, source_pos, sizeof(char), source_count, dest_buf,
                                                     dest_buf_pos, 1, source_count * sizeof(char));
}

robotraconteurlite_status robotraconteurlite_string_cmp(const struct robotraconteurlite_string* str1,
                                                        const struct robotraconteurlite_string* str2)
{
    assert(str1 != NULL);
    assert(str2 != NULL);
    assert(str1->data != NULL);
    assert(str2->data != NULL);

    if ((str1->len == 0U) && (str2->len == 0U))
    {
        return 0;
    }

    if (str1->len < str2->len)
    {
        return -1;
    }

    if (str1->len > str2->len)
    {
        return 1;
    }

    return memcmp(str1->data, str2->data, str1->len);
}

uint32_t robotraconteurlite_string_hash(const struct robotraconteurlite_string* str)
{

    uint32_t str_len = 0;

    assert(str != NULL);
    assert(str->data != NULL);

    str_len = (uint32_t)str->len;

    switch (str_len)
    {
    case 0:
        return 0;
    case 1:
        return (str_len & 0xFFFFU) | ((uint32_t)str->data[0] << 24);
    case 2:
        return (str_len & 0xFFFFU) | ((uint32_t)str->data[0] << 24) | ((uint32_t)str->data[str_len - 1U] << 16);
    default:
        return (str_len & 0xFFFFU) | ((uint32_t)str->data[0] << 24) |
               (((uint32_t)str->data[str_len - 1U] ^ (uint32_t)str->data[str_len - 2U]) << 16);
    }
}

robotraconteurlite_status robotraconteurlite_string_copy_to(const struct robotraconteurlite_string* source,
                                                            struct robotraconteurlite_string* dest)
{

    assert(source != NULL);
    assert(dest != NULL);
    if (source->len == 0U)
    {
        dest->len = 0;
        return ROBOTRACONTEURLITE_ERROR_SUCCESS;
    }

    if (!source->data)
    {
        return ROBOTRACONTEURLITE_ERROR_INVALID_ARGUMENT;
    }
    assert(source->data != NULL);
    assert(dest->data != NULL);

    if (source->len > dest->len)
    {
        return ROBOTRACONTEURLITE_ERROR_INVALID_ARGUMENT;
    }

    (void)memcpy(dest->data, source->data, source->len);
    dest->len = source->len;
    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}

robotraconteurlite_status robotraconteurlite_string_shallow_copy_to(const struct robotraconteurlite_string* source,
                                                                    struct robotraconteurlite_string* dest)
{

    assert(source != NULL);
    assert(dest != NULL);

    dest->data = source->data;
    dest->len = source->len;
    return ROBOTRACONTEURLITE_ERROR_SUCCESS;
}
