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

#ifndef ROBOTRACONTEURLITE_ARRAY_H
#define ROBOTRACONTEURLITE_ARRAY_H

#include <stdint.h>
#include "robotraconteurlite/err.h"
#include "robotraconteurlite/config.h"
#include <stddef.h>
#include <string.h>

struct robotraconteurlite_array_storage
{
    void* data;
    size_t len;
};

struct robotraconteurlite_buffer
{
    uint8_t* data;
    size_t len;
};

struct robotraconteurlite_buffer_vec
{
    struct robotraconteurlite_buffer* buffer_vec;
    size_t buffer_vec_cnt;

    struct robotraconteurlite_array_storage _scalar_storage;
};

struct robotraconteurlite_array_double
{
    double* data;
    size_t len;
};

struct robotraconteurlite_array_vec_double
{
    struct robotraconteurlite_array_double* array_vec;
    size_t array_vec_cnt;

    struct robotraconteurlite_array_storage _scalar_storage;
};

struct robotraconteurlite_array_single
{
    float* data;
    size_t len;
};

struct robotraconteurlite_array_vec_single
{
    struct robotraconteurlite_array_single* array_vec;
    size_t array_vec_cnt;

    struct robotraconteurlite_array_storage _scalar_storage;
};

struct robotraconteurlite_array_int8
{
    int8_t* data;
    size_t len;
};

struct robotraconteurlite_array_vec_int8
{
    struct robotraconteurlite_array_int8* array_vec;
    size_t array_vec_cnt;

    struct robotraconteurlite_array_storage _scalar_storage;
};

struct robotraconteurlite_array_uint8
{
    uint8_t* data;
    size_t len;
};

struct robotraconteurlite_array_vec_uint8
{
    struct robotraconteurlite_array_uint8* array_vec;
    size_t array_vec_cnt;

    struct robotraconteurlite_array_storage _scalar_storage;
};

struct robotraconteurlite_array_int16
{
    int16_t* data;
    size_t len;
};

struct robotraconteurlite_array_vec_int16
{
    struct robotraconteurlite_array_int16* array_vec;
    size_t array_vec_cnt;

    struct robotraconteurlite_array_storage _scalar_storage;
};

struct robotraconteurlite_array_uint16
{
    uint16_t* data;
    size_t len;
};

struct robotraconteurlite_array_vec_uint16
{
    struct robotraconteurlite_array_uint16* array_vec;
    size_t array_vec_cnt;

    struct robotraconteurlite_array_storage _scalar_storage;
};

struct robotraconteurlite_array_int32
{
    int32_t* data;
    size_t len;
};

struct robotraconteurlite_array_vec_int32
{
    struct robotraconteurlite_array_int32* array_vec;
    size_t array_vec_cnt;

    struct robotraconteurlite_array_storage _scalar_storage;
};

struct robotraconteurlite_array_uint32
{
    uint32_t* data;
    size_t len;
};

struct robotraconteurlite_array_vec_uint32
{
    struct robotraconteurlite_array_uint32* array_vec;
    size_t array_vec_cnt;

    struct robotraconteurlite_array_storage _scalar_storage;
};

struct robotraconteurlite_array_int64
{
    int64_t* data;
    size_t len;
};

struct robotraconteurlite_array_vec_int64
{
    struct robotraconteurlite_array_int64* array_vec;
    size_t array_vec_cnt;

    struct robotraconteurlite_array_storage _scalar_storage;
};

struct robotraconteurlite_array_uint64
{
    uint64_t* data;
    size_t len;
};

struct robotraconteurlite_array_vec_uint64
{
    struct robotraconteurlite_array_uint64* array_vec;
    size_t array_vec_cnt;

    struct robotraconteurlite_array_storage _scalar_storage;
};

/* Complex types */
struct robotraconteurlite_cdouble
{
    double real;
    double imag;
};

struct robotraconteurlite_csingle
{
    float real;
    float imag;
};

struct robotraconteurlite_array_cdouble
{
    struct robotraconteurlite_cdouble* data;
    size_t len;
};

struct robotraconteurlite_array_vec_cdouble
{
    struct robotraconteurlite_array_cdouble* array_vec;
    size_t array_vec_cnt;

    struct robotraconteurlite_array_storage _scalar_storage;
};

struct robotraconteurlite_array_csingle
{
    struct robotraconteurlite_csingle* data;
    size_t len;
};

struct robotraconteurlite_array_vec_csingle
{
    struct robotraconteurlite_array_csingle* array_vec;
    size_t array_vec_cnt;

    struct robotraconteurlite_array_storage _scalar_storage;
};

struct robotraconteurlite_bool
{
    uint8_t logical;
};

struct robotraconteurlite_array_bool
{
    struct robotraconteurlite_bool* data;
    size_t len;
};

struct robotraconteurlite_array_vec_bool
{
    struct robotraconteurlite_array_bool* array_vec;
    size_t array_vec_cnt;

    struct robotraconteurlite_array_storage _scalar_storage;
};

/* All strings are utf8! */
struct robotraconteurlite_string
{
    char* data;
    size_t len;
};

struct robotraconteurlite_string_vec
{
    struct robotraconteurlite_string* string_vec;
    size_t string_vec_cnt;

    struct robotraconteurlite_array_storage _scalar_storage;
};

ROBOTRACONTEURLITE_API robotraconteurlite_status
robotraconteurlite_buffer_init_scalar(struct robotraconteurlite_buffer* buffer, void* data, size_t len);

ROBOTRACONTEURLITE_API robotraconteurlite_status robotraconteurlite_buffer_vec_init_scalar(
    struct robotraconteurlite_buffer_vec* buffer_vec, struct robotraconteurlite_buffer* buffer);

ROBOTRACONTEURLITE_API robotraconteurlite_status
robotraconteurlite_buffer_len(const struct robotraconteurlite_buffer* source, size_t* len);

ROBOTRACONTEURLITE_API robotraconteurlite_status
robotraconteurlite_buffer_vec_len(const struct robotraconteurlite_buffer_vec* source, size_t* len);

ROBOTRACONTEURLITE_API robotraconteurlite_status
robotraconteurlite_buffer_copy(const struct robotraconteurlite_buffer* source, size_t source_pos,
                               const struct robotraconteurlite_buffer* dest, size_t dest_pos, size_t count);

ROBOTRACONTEURLITE_API robotraconteurlite_status
robotraconteurlite_buffer_vec_copy_vec(const struct robotraconteurlite_buffer_vec* source, size_t source_pos,
                                       const struct robotraconteurlite_buffer_vec* dest, size_t dest_pos, size_t count);

ROBOTRACONTEURLITE_API robotraconteurlite_status robotraconteurlite_buffer_copy_ex(
    const struct robotraconteurlite_buffer* source, size_t source_pos, size_t source_elem_size, size_t source_count,
    const struct robotraconteurlite_buffer* dest, size_t dest_pos, size_t dest_elem_size, size_t dest_count);

ROBOTRACONTEURLITE_API robotraconteurlite_status robotraconteurlite_buffer_vec_copy_vec_ex(
    const struct robotraconteurlite_buffer_vec* source, size_t source_pos, size_t source_elem_size, size_t source_count,
    const struct robotraconteurlite_buffer_vec* dest, size_t dest_pos, size_t dest_elem_size, size_t dest_count);

/* double */
ROBOTRACONTEURLITE_API robotraconteurlite_status robotraconteurlite_buffer_vec_copy_to_double_array(
    const struct robotraconteurlite_buffer_vec* source_buf, size_t source_buf_pos,
    const struct robotraconteurlite_array_double* dest, size_t dest_pos, size_t dest_count);

ROBOTRACONTEURLITE_API robotraconteurlite_status robotraconteurlite_buffer_vec_copy_from_double_array(
    const struct robotraconteurlite_buffer_vec* dest_buf, size_t dest_buf_pos,
    const struct robotraconteurlite_array_double* source, size_t source_pos, size_t source_count);

ROBOTRACONTEURLITE_API robotraconteurlite_status robotraconteurlite_buffer_vec_copy_to_double(
    const struct robotraconteurlite_buffer_vec* source_buf, size_t source_buf_pos, double* dest);

ROBOTRACONTEURLITE_API robotraconteurlite_status robotraconteurlite_buffer_vec_copy_from_double(
    const struct robotraconteurlite_buffer_vec* dest_buf, size_t dest_buf_pos, double source);

/* single */
ROBOTRACONTEURLITE_API robotraconteurlite_status robotraconteurlite_buffer_vec_copy_to_single_array(
    const struct robotraconteurlite_buffer_vec* source_buf, size_t source_buf_pos,
    const struct robotraconteurlite_array_single* dest, size_t dest_pos, size_t dest_count);

ROBOTRACONTEURLITE_API robotraconteurlite_status robotraconteurlite_buffer_vec_copy_from_single_array(
    const struct robotraconteurlite_buffer_vec* dest_buf, size_t dest_buf_pos,
    const struct robotraconteurlite_array_single* source, size_t source_pos, size_t source_count);

ROBOTRACONTEURLITE_API robotraconteurlite_status robotraconteurlite_buffer_vec_copy_to_single(
    const struct robotraconteurlite_buffer_vec* source_buf, size_t source_buf_pos, float* dest);

ROBOTRACONTEURLITE_API robotraconteurlite_status robotraconteurlite_buffer_vec_copy_from_single(
    const struct robotraconteurlite_buffer_vec* dest_buf, size_t dest_buf_pos, float source);

/* int8 */
ROBOTRACONTEURLITE_API robotraconteurlite_status robotraconteurlite_buffer_vec_copy_to_int8_array(
    const struct robotraconteurlite_buffer_vec* source_buf, size_t source_buf_pos,
    const struct robotraconteurlite_array_int8* dest, size_t dest_pos, size_t dest_count);

ROBOTRACONTEURLITE_API robotraconteurlite_status robotraconteurlite_buffer_vec_copy_from_int8_array(
    const struct robotraconteurlite_buffer_vec* dest_buf, size_t dest_buf_pos,
    const struct robotraconteurlite_array_int8* source, size_t source_pos, size_t source_count);

ROBOTRACONTEURLITE_API robotraconteurlite_status robotraconteurlite_buffer_vec_copy_to_int8(
    const struct robotraconteurlite_buffer_vec* source_buf, size_t source_buf_pos, int8_t* dest);

ROBOTRACONTEURLITE_API robotraconteurlite_status robotraconteurlite_buffer_vec_copy_from_int8(
    const struct robotraconteurlite_buffer_vec* dest_buf, size_t dest_buf_pos, int8_t source);

/* uint8 */
ROBOTRACONTEURLITE_API robotraconteurlite_status robotraconteurlite_buffer_vec_copy_to_uint8_array(
    const struct robotraconteurlite_buffer_vec* source_buf, size_t source_buf_pos,
    const struct robotraconteurlite_array_uint8* dest, size_t dest_pos, size_t dest_count);

ROBOTRACONTEURLITE_API robotraconteurlite_status robotraconteurlite_buffer_vec_copy_from_uint8_array(
    const struct robotraconteurlite_buffer_vec* dest_buf, size_t dest_buf_pos,
    const struct robotraconteurlite_array_uint8* source, size_t source_pos, size_t source_count);

ROBOTRACONTEURLITE_API robotraconteurlite_status robotraconteurlite_buffer_vec_copy_to_uint8(
    const struct robotraconteurlite_buffer_vec* source_buf, size_t source_buf_pos, uint8_t* dest);

ROBOTRACONTEURLITE_API robotraconteurlite_status robotraconteurlite_buffer_vec_copy_from_uint8(
    const struct robotraconteurlite_buffer_vec* dest_buf, size_t dest_buf_pos, uint8_t source);

/* int16 */
ROBOTRACONTEURLITE_API robotraconteurlite_status robotraconteurlite_buffer_vec_copy_to_int16_array(
    const struct robotraconteurlite_buffer_vec* source_buf, size_t source_buf_pos,
    const struct robotraconteurlite_array_int16* dest, size_t dest_pos, size_t dest_count);

ROBOTRACONTEURLITE_API robotraconteurlite_status robotraconteurlite_buffer_vec_copy_from_int16_array(
    const struct robotraconteurlite_buffer_vec* dest_buf, size_t dest_buf_pos,
    const struct robotraconteurlite_array_int16* source, size_t source_pos, size_t source_count);

ROBOTRACONTEURLITE_API robotraconteurlite_status robotraconteurlite_buffer_vec_copy_to_int16(
    const struct robotraconteurlite_buffer_vec* source_buf, size_t source_buf_pos, int16_t* dest);

ROBOTRACONTEURLITE_API robotraconteurlite_status robotraconteurlite_buffer_vec_copy_from_int16(
    const struct robotraconteurlite_buffer_vec* dest_buf, size_t dest_buf_pos, int16_t source);

/* uint16 */
ROBOTRACONTEURLITE_API robotraconteurlite_status robotraconteurlite_buffer_vec_copy_to_uint16_array(
    const struct robotraconteurlite_buffer_vec* source_buf, size_t source_buf_pos,
    const struct robotraconteurlite_array_uint16* dest, size_t dest_pos, size_t dest_count);

ROBOTRACONTEURLITE_API robotraconteurlite_status robotraconteurlite_buffer_vec_copy_from_uint16_array(
    const struct robotraconteurlite_buffer_vec* dest_buf, size_t dest_buf_pos,
    const struct robotraconteurlite_array_uint16* source, size_t source_pos, size_t source_count);

ROBOTRACONTEURLITE_API robotraconteurlite_status robotraconteurlite_buffer_vec_copy_to_uint16(
    const struct robotraconteurlite_buffer_vec* source_buf, size_t source_buf_pos, uint16_t* dest);

ROBOTRACONTEURLITE_API robotraconteurlite_status robotraconteurlite_buffer_vec_copy_from_uint16(
    const struct robotraconteurlite_buffer_vec* dest_buf, size_t dest_buf_pos, uint16_t source);

/* int32 */
ROBOTRACONTEURLITE_API robotraconteurlite_status robotraconteurlite_buffer_vec_copy_to_int32_array(
    const struct robotraconteurlite_buffer_vec* source_buf, size_t source_buf_pos,
    const struct robotraconteurlite_array_int32* dest, size_t dest_pos, size_t dest_count);

ROBOTRACONTEURLITE_API robotraconteurlite_status robotraconteurlite_buffer_vec_copy_from_int32_array(
    const struct robotraconteurlite_buffer_vec* dest_buf, size_t dest_buf_pos,
    const struct robotraconteurlite_array_int32* source, size_t source_pos, size_t source_count);

ROBOTRACONTEURLITE_API robotraconteurlite_status robotraconteurlite_buffer_vec_copy_to_int32(
    const struct robotraconteurlite_buffer_vec* source_buf, size_t source_buf_pos, int32_t* dest);

ROBOTRACONTEURLITE_API robotraconteurlite_status robotraconteurlite_buffer_vec_copy_from_int32(
    const struct robotraconteurlite_buffer_vec* dest_buf, size_t dest_buf_pos, int32_t source);

/* uint32 */
ROBOTRACONTEURLITE_API robotraconteurlite_status robotraconteurlite_buffer_vec_copy_to_uint32_array(
    const struct robotraconteurlite_buffer_vec* source_buf, size_t source_buf_pos,
    const struct robotraconteurlite_array_uint32* dest, size_t dest_pos, size_t dest_count);

ROBOTRACONTEURLITE_API robotraconteurlite_status robotraconteurlite_buffer_vec_copy_from_uint32_array(
    const struct robotraconteurlite_buffer_vec* dest_buf, size_t dest_buf_pos,
    const struct robotraconteurlite_array_uint32* source, size_t source_pos, size_t source_count);

ROBOTRACONTEURLITE_API robotraconteurlite_status robotraconteurlite_buffer_vec_copy_to_uint32(
    const struct robotraconteurlite_buffer_vec* source_buf, size_t source_buf_pos, uint32_t* dest);

ROBOTRACONTEURLITE_API robotraconteurlite_status robotraconteurlite_buffer_vec_copy_from_uint32(
    const struct robotraconteurlite_buffer_vec* dest_buf, size_t dest_buf_pos, uint32_t source);

/* int64 */
ROBOTRACONTEURLITE_API robotraconteurlite_status robotraconteurlite_buffer_vec_copy_to_int64_array(
    const struct robotraconteurlite_buffer_vec* source_buf, size_t source_buf_pos,
    const struct robotraconteurlite_array_int64* dest, size_t dest_pos, size_t dest_count);

ROBOTRACONTEURLITE_API robotraconteurlite_status robotraconteurlite_buffer_vec_copy_from_int64_array(
    const struct robotraconteurlite_buffer_vec* dest_buf, size_t dest_buf_pos,
    const struct robotraconteurlite_array_int64* source, size_t source_pos, size_t source_count);

ROBOTRACONTEURLITE_API robotraconteurlite_status robotraconteurlite_buffer_vec_copy_to_int64(
    const struct robotraconteurlite_buffer_vec* source_buf, size_t source_buf_pos, int64_t* dest);

ROBOTRACONTEURLITE_API robotraconteurlite_status robotraconteurlite_buffer_vec_copy_from_int64(
    const struct robotraconteurlite_buffer_vec* dest_buf, size_t dest_buf_pos, int64_t source);

/* uint64 */
ROBOTRACONTEURLITE_API robotraconteurlite_status robotraconteurlite_buffer_vec_copy_to_uint64_array(
    const struct robotraconteurlite_buffer_vec* source_buf, size_t source_buf_pos,
    const struct robotraconteurlite_array_uint64* dest, size_t dest_pos, size_t dest_count);

ROBOTRACONTEURLITE_API robotraconteurlite_status robotraconteurlite_buffer_vec_copy_from_uint64_array(
    const struct robotraconteurlite_buffer_vec* dest_buf, size_t dest_buf_pos,
    const struct robotraconteurlite_array_uint64* source, size_t source_pos, size_t source_count);

ROBOTRACONTEURLITE_API robotraconteurlite_status robotraconteurlite_buffer_vec_copy_to_uint64(
    const struct robotraconteurlite_buffer_vec* source_buf, size_t source_buf_pos, uint64_t* dest);

ROBOTRACONTEURLITE_API robotraconteurlite_status robotraconteurlite_buffer_vec_copy_from_uint64(
    const struct robotraconteurlite_buffer_vec* dest_buf, size_t dest_buf_pos, uint64_t source);

/* cdouble */
ROBOTRACONTEURLITE_API robotraconteurlite_status robotraconteurlite_buffer_vec_copy_to_cdouble_array(
    const struct robotraconteurlite_buffer_vec* source_buf, size_t source_buf_pos,
    const struct robotraconteurlite_array_cdouble* dest, size_t dest_pos, size_t dest_count);

ROBOTRACONTEURLITE_API robotraconteurlite_status robotraconteurlite_buffer_vec_copy_from_cdouble_array(
    const struct robotraconteurlite_buffer_vec* dest_buf, size_t dest_buf_pos,
    const struct robotraconteurlite_array_cdouble* source, size_t source_pos, size_t source_count);

ROBOTRACONTEURLITE_API robotraconteurlite_status
robotraconteurlite_buffer_vec_copy_to_cdouble(const struct robotraconteurlite_buffer_vec* source_buf,
                                              size_t source_buf_pos, struct robotraconteurlite_cdouble* dest);

ROBOTRACONTEURLITE_API robotraconteurlite_status
robotraconteurlite_buffer_vec_copy_from_cdouble(const struct robotraconteurlite_buffer_vec* dest_buf,
                                                size_t dest_buf_pos, struct robotraconteurlite_cdouble source);

/* csingle */
ROBOTRACONTEURLITE_API robotraconteurlite_status robotraconteurlite_buffer_vec_copy_to_csingle_array(
    const struct robotraconteurlite_buffer_vec* source_buf, size_t source_buf_pos,
    const struct robotraconteurlite_array_csingle* dest, size_t dest_pos, size_t dest_count);

ROBOTRACONTEURLITE_API robotraconteurlite_status robotraconteurlite_buffer_vec_copy_from_csingle_array(
    const struct robotraconteurlite_buffer_vec* dest_buf, size_t dest_buf_pos,
    const struct robotraconteurlite_array_csingle* source, size_t source_pos, size_t source_count);

ROBOTRACONTEURLITE_API robotraconteurlite_status
robotraconteurlite_buffer_vec_copy_to_csingle(const struct robotraconteurlite_buffer_vec* source_buf,
                                              size_t source_buf_pos, struct robotraconteurlite_csingle* dest);

ROBOTRACONTEURLITE_API robotraconteurlite_status
robotraconteurlite_buffer_vec_copy_from_csingle(const struct robotraconteurlite_buffer_vec* dest_buf,
                                                size_t dest_buf_pos, struct robotraconteurlite_csingle source);

/* struct robotraconteurlite_bool */
ROBOTRACONTEURLITE_API robotraconteurlite_status robotraconteurlite_buffer_vec_copy_to_bool_array(
    const struct robotraconteurlite_buffer_vec* source_buf, size_t source_buf_pos,
    const struct robotraconteurlite_array_bool* dest, size_t dest_pos, size_t dest_count);

ROBOTRACONTEURLITE_API robotraconteurlite_status robotraconteurlite_buffer_vec_copy_from_bool_array(
    const struct robotraconteurlite_buffer_vec* dest_buf, size_t dest_buf_pos,
    const struct robotraconteurlite_array_bool* source, size_t source_pos, size_t source_count);

ROBOTRACONTEURLITE_API robotraconteurlite_status
robotraconteurlite_buffer_vec_copy_to_bool(const struct robotraconteurlite_buffer_vec* source_buf,
                                           size_t source_buf_pos, struct robotraconteurlite_bool* dest);

ROBOTRACONTEURLITE_API robotraconteurlite_status robotraconteurlite_buffer_vec_copy_from_bool(
    const struct robotraconteurlite_buffer_vec* dest_buf, size_t dest_buf_pos, struct robotraconteurlite_bool source);

/* string */
ROBOTRACONTEURLITE_API robotraconteurlite_status robotraconteurlite_buffer_vec_copy_to_string(
    const struct robotraconteurlite_buffer_vec* source_buf, size_t source_buf_pos,
    const struct robotraconteurlite_string* dest, size_t dest_pos, size_t dest_count);

ROBOTRACONTEURLITE_API robotraconteurlite_status robotraconteurlite_buffer_vec_copy_from_string(
    const struct robotraconteurlite_buffer_vec* dest_buf, size_t dest_buf_pos,
    const struct robotraconteurlite_string* source, size_t source_pos, size_t source_count);

ROBOTRACONTEURLITE_API robotraconteurlite_status robotraconteurlite_string_cmp(
    const struct robotraconteurlite_string* str1, const struct robotraconteurlite_string* str2);

static int robotraconteurlite_string_cmp_c_str(const struct robotraconteurlite_string* str1, const char* str2)
{
    size_t strlen_str2 = strlen(str2);
    if ((strlen_str2 == 0U) && (str1->len == 0U))
    {
        return 0;
    }

    if (str1->len == 0U)
    {
        return -1;
    }

    if (strlen_str2 == 0U)
    {
        return 1;
    }

    if (str1->len < strlen_str2)
    {
        return -1;
    }

    if (str1->len > strlen_str2)
    {
        return 1;
    }

    return memcmp(str1->data, str2, str1->len);
}

ROBOTRACONTEURLITE_API uint32_t robotraconteurlite_string_hash(const struct robotraconteurlite_string* str);

ROBOTRACONTEURLITE_API robotraconteurlite_status robotraconteurlite_string_copy_to(
    const struct robotraconteurlite_string* source, struct robotraconteurlite_string* dest);

ROBOTRACONTEURLITE_API robotraconteurlite_status robotraconteurlite_string_shallow_copy_to(
    const struct robotraconteurlite_string* source, struct robotraconteurlite_string* dest);

static void robotraconteurlite_string_from_c_str(const char* source, struct robotraconteurlite_string* dest)
{
    if (source == NULL)
    {
        dest->data = NULL;
        dest->len = 0;
        return;
    }
    dest->data = (char*)source;
    dest->len = strlen(source);
}
#endif /* ROBOTRACONTEURLITE_ARRAY_H */
