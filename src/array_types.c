/* cppcheck-suppress-file invalidPointerCast */
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

/* double */
robotraconteurlite_status robotraconteurlite_buffer_vec_copy_to_double_array(
    const struct robotraconteurlite_buffer_vec* source_buf, size_t source_buf_pos,
    struct robotraconteurlite_array_double* dest, size_t dest_pos, size_t dest_count)
{
    return robotraconteurlite_buffer_vec_copy_to_mem(source_buf, source_buf_pos,
                                                     /* cppcheck-suppress invalidPointerCast */
                                                     (uint8_t*)dest->data, dest->len, dest_pos, sizeof(double),
                                                     dest_count);
}

robotraconteurlite_status robotraconteurlite_buffer_vec_copy_from_double_array(
    struct robotraconteurlite_buffer_vec* dest_buf, size_t dest_buf_pos,
    const struct robotraconteurlite_array_double* source, size_t source_pos, size_t source_count)
{
    return robotraconteurlite_buffer_vec_copy_from_mem(dest_buf, dest_buf_pos,
                                                       /* cppcheck-suppress invalidPointerCast */
                                                       (const uint8_t*)source->data, source->len, source_pos,
                                                       sizeof(double), source_count);
}

robotraconteurlite_status robotraconteurlite_buffer_vec_copy_to_double(
    const struct robotraconteurlite_buffer_vec* source_buf, size_t source_buf_pos, double* dest)
{
    return robotraconteurlite_buffer_vec_copy_to_mem(source_buf, source_buf_pos,
                                                     /* cppcheck-suppress invalidPointerCast */
                                                     (uint8_t*)dest, 1, 0, sizeof(double), 1);
}

robotraconteurlite_status robotraconteurlite_buffer_vec_copy_from_double(struct robotraconteurlite_buffer_vec* dest_buf,
                                                                         size_t dest_buf_pos, double source)
{
    return robotraconteurlite_buffer_vec_copy_from_mem(dest_buf, dest_buf_pos,
                                                       /* cppcheck-suppress invalidPointerCast */
                                                       (const uint8_t*)&source, 1, 0, sizeof(double), 1);
}

/* single */
robotraconteurlite_status robotraconteurlite_buffer_vec_copy_to_single_array(
    const struct robotraconteurlite_buffer_vec* source_buf, size_t source_buf_pos,
    struct robotraconteurlite_array_single* dest, size_t dest_pos, size_t dest_count)
{
    return robotraconteurlite_buffer_vec_copy_to_mem(source_buf, source_buf_pos,
                                                     /* cppcheck-suppress invalidPointerCast */
                                                     (uint8_t*)dest->data, dest->len, dest_pos, sizeof(float),
                                                     dest_count);
}

robotraconteurlite_status robotraconteurlite_buffer_vec_copy_from_single_array(
    struct robotraconteurlite_buffer_vec* dest_buf, size_t dest_buf_pos,
    const struct robotraconteurlite_array_single* source, size_t source_pos, size_t source_count)
{
    return robotraconteurlite_buffer_vec_copy_from_mem(dest_buf, dest_buf_pos,
                                                       /* cppcheck-suppress invalidPointerCast */
                                                       (const uint8_t*)source->data, source->len, source_pos,
                                                       sizeof(float), source_count);
}

robotraconteurlite_status robotraconteurlite_buffer_vec_copy_to_single(
    const struct robotraconteurlite_buffer_vec* source_buf, size_t source_buf_pos, float* dest)
{
    return robotraconteurlite_buffer_vec_copy_to_mem(source_buf, source_buf_pos,
                                                     /* cppcheck-suppress invalidPointerCast */
                                                     (uint8_t*)dest, 1, 0, sizeof(float), 1);
}

robotraconteurlite_status robotraconteurlite_buffer_vec_copy_from_single(struct robotraconteurlite_buffer_vec* dest_buf,
                                                                         size_t dest_buf_pos, float source)
{
    return robotraconteurlite_buffer_vec_copy_from_mem(dest_buf, dest_buf_pos,
                                                       /* cppcheck-suppress invalidPointerCast */
                                                       (const uint8_t*)&source, 1, 0, sizeof(float), 1);
}

/* int8 */
robotraconteurlite_status robotraconteurlite_buffer_vec_copy_to_int8_array(
    const struct robotraconteurlite_buffer_vec* source_buf, size_t source_buf_pos,
    struct robotraconteurlite_array_int8* dest, size_t dest_pos, size_t dest_count)
{
    return robotraconteurlite_buffer_vec_copy_to_mem(source_buf, source_buf_pos, (uint8_t*)dest->data, dest->len,
                                                     dest_pos, sizeof(int8_t), dest_count);
}

robotraconteurlite_status robotraconteurlite_buffer_vec_copy_from_int8_array(
    struct robotraconteurlite_buffer_vec* dest_buf, size_t dest_buf_pos,
    const struct robotraconteurlite_array_int8* source, size_t source_pos, size_t source_count)
{
    return robotraconteurlite_buffer_vec_copy_from_mem(dest_buf, dest_buf_pos, (const uint8_t*)source->data,
                                                       source->len, source_pos, sizeof(int8_t), source_count);
}

robotraconteurlite_status robotraconteurlite_buffer_vec_copy_to_int8(
    const struct robotraconteurlite_buffer_vec* source_buf, size_t source_buf_pos, int8_t* dest)
{
    return robotraconteurlite_buffer_vec_copy_to_mem(source_buf, source_buf_pos, (uint8_t*)dest, 1, 0, sizeof(int8_t),
                                                     1);
}

robotraconteurlite_status robotraconteurlite_buffer_vec_copy_from_int8(struct robotraconteurlite_buffer_vec* dest_buf,
                                                                       size_t dest_buf_pos, int8_t source)
{
    return robotraconteurlite_buffer_vec_copy_from_mem(dest_buf, dest_buf_pos, (const uint8_t*)&source, 1, 0,
                                                       sizeof(int8_t), 1);
}

/* uint8 */
robotraconteurlite_status robotraconteurlite_buffer_vec_copy_to_uint8_array(
    const struct robotraconteurlite_buffer_vec* source_buf, size_t source_buf_pos,
    struct robotraconteurlite_array_uint8* dest, size_t dest_pos, size_t dest_count)
{
    return robotraconteurlite_buffer_vec_copy_to_mem(source_buf, source_buf_pos, (uint8_t*)dest->data, dest->len,
                                                     dest_pos, sizeof(uint8_t), dest_count);
}

robotraconteurlite_status robotraconteurlite_buffer_vec_copy_from_uint8_array(
    struct robotraconteurlite_buffer_vec* dest_buf, size_t dest_buf_pos,
    const struct robotraconteurlite_array_uint8* source, size_t source_pos, size_t source_count)
{
    return robotraconteurlite_buffer_vec_copy_from_mem(dest_buf, dest_buf_pos, (const uint8_t*)source->data,
                                                       source->len, source_pos, sizeof(uint8_t), source_count);
}

robotraconteurlite_status robotraconteurlite_buffer_vec_copy_to_uint8(
    const struct robotraconteurlite_buffer_vec* source_buf, size_t source_buf_pos, uint8_t* dest)
{
    return robotraconteurlite_buffer_vec_copy_to_mem(source_buf, source_buf_pos, (uint8_t*)dest, 1, 0, sizeof(uint8_t),
                                                     1);
}

robotraconteurlite_status robotraconteurlite_buffer_vec_copy_from_uint8(struct robotraconteurlite_buffer_vec* dest_buf,
                                                                        size_t dest_buf_pos, uint8_t source)
{
    return robotraconteurlite_buffer_vec_copy_from_mem(dest_buf, dest_buf_pos, (const uint8_t*)&source, 1, 0,
                                                       sizeof(uint8_t), 1);
}

/* int16 */
robotraconteurlite_status robotraconteurlite_buffer_vec_copy_to_int16_array(
    const struct robotraconteurlite_buffer_vec* source_buf, size_t source_buf_pos,
    struct robotraconteurlite_array_int16* dest, size_t dest_pos, size_t dest_count)
{
    return robotraconteurlite_buffer_vec_copy_to_mem(source_buf, source_buf_pos, (uint8_t*)dest->data, dest->len,
                                                     dest_pos, sizeof(int16_t), dest_count);
}

robotraconteurlite_status robotraconteurlite_buffer_vec_copy_from_int16_array(
    struct robotraconteurlite_buffer_vec* dest_buf, size_t dest_buf_pos,
    const struct robotraconteurlite_array_int16* source, size_t source_pos, size_t source_count)
{
    return robotraconteurlite_buffer_vec_copy_from_mem(dest_buf, dest_buf_pos, (const uint8_t*)source->data,
                                                       source->len, source_pos, sizeof(int16_t), source_count);
}

robotraconteurlite_status robotraconteurlite_buffer_vec_copy_to_int16(
    const struct robotraconteurlite_buffer_vec* source_buf, size_t source_buf_pos, int16_t* dest)
{
    return robotraconteurlite_buffer_vec_copy_to_mem(source_buf, source_buf_pos, (uint8_t*)dest, 1, 0, sizeof(int16_t),
                                                     1);
}

robotraconteurlite_status robotraconteurlite_buffer_vec_copy_from_int16(struct robotraconteurlite_buffer_vec* dest_buf,
                                                                        size_t dest_buf_pos, int16_t source)
{
    return robotraconteurlite_buffer_vec_copy_from_mem(dest_buf, dest_buf_pos, (const uint8_t*)&source, 1, 0,
                                                       sizeof(int16_t), 1);
}

/* uint16 */
robotraconteurlite_status robotraconteurlite_buffer_vec_copy_to_uint16_array(
    const struct robotraconteurlite_buffer_vec* source_buf, size_t source_buf_pos,
    struct robotraconteurlite_array_uint16* dest, size_t dest_pos, size_t dest_count)
{
    return robotraconteurlite_buffer_vec_copy_to_mem(source_buf, source_buf_pos, (uint8_t*)dest->data, dest->len,
                                                     dest_pos, sizeof(uint16_t), dest_count);
}

robotraconteurlite_status robotraconteurlite_buffer_vec_copy_from_uint16_array(
    struct robotraconteurlite_buffer_vec* dest_buf, size_t dest_buf_pos,
    const struct robotraconteurlite_array_uint16* source, size_t source_pos, size_t source_count)
{
    return robotraconteurlite_buffer_vec_copy_from_mem(dest_buf, dest_buf_pos, (const uint8_t*)source->data,
                                                       source->len, source_pos, sizeof(uint16_t), source_count);
}

robotraconteurlite_status robotraconteurlite_buffer_vec_copy_to_uint16(
    const struct robotraconteurlite_buffer_vec* source_buf, size_t source_buf_pos, uint16_t* dest)
{
    return robotraconteurlite_buffer_vec_copy_to_mem(source_buf, source_buf_pos, (uint8_t*)dest, 1, 0, sizeof(uint16_t),
                                                     1);
}

robotraconteurlite_status robotraconteurlite_buffer_vec_copy_from_uint16(struct robotraconteurlite_buffer_vec* dest_buf,
                                                                         size_t dest_buf_pos, uint16_t source)
{
    return robotraconteurlite_buffer_vec_copy_from_mem(dest_buf, dest_buf_pos, (const uint8_t*)&source, 1, 0,
                                                       sizeof(uint16_t), 1);
}

/* int32 */
robotraconteurlite_status robotraconteurlite_buffer_vec_copy_to_int32_array(
    const struct robotraconteurlite_buffer_vec* source_buf, size_t source_buf_pos,
    struct robotraconteurlite_array_int32* dest, size_t dest_pos, size_t dest_count)
{
    return robotraconteurlite_buffer_vec_copy_to_mem(source_buf, source_buf_pos, (uint8_t*)dest->data, dest->len,
                                                     dest_pos, sizeof(int32_t), dest_count);
}

robotraconteurlite_status robotraconteurlite_buffer_vec_copy_from_int32_array(
    struct robotraconteurlite_buffer_vec* dest_buf, size_t dest_buf_pos,
    const struct robotraconteurlite_array_int32* source, size_t source_pos, size_t source_count)
{
    return robotraconteurlite_buffer_vec_copy_from_mem(dest_buf, dest_buf_pos, (const uint8_t*)source->data,
                                                       source->len, source_pos, sizeof(int32_t), source_count);
}

robotraconteurlite_status robotraconteurlite_buffer_vec_copy_to_int32(
    const struct robotraconteurlite_buffer_vec* source_buf, size_t source_buf_pos, int32_t* dest)
{
    return robotraconteurlite_buffer_vec_copy_to_mem(source_buf, source_buf_pos, (uint8_t*)dest, 1, 0, sizeof(int32_t),
                                                     1);
}

robotraconteurlite_status robotraconteurlite_buffer_vec_copy_from_int32(struct robotraconteurlite_buffer_vec* dest_buf,
                                                                        size_t dest_buf_pos, int32_t source)
{
    return robotraconteurlite_buffer_vec_copy_from_mem(dest_buf, dest_buf_pos, (const uint8_t*)&source, 1, 0,
                                                       sizeof(int32_t), 1);
}

/* uint32 */
robotraconteurlite_status robotraconteurlite_buffer_vec_copy_to_uint32_array(
    const struct robotraconteurlite_buffer_vec* source_buf, size_t source_buf_pos,
    struct robotraconteurlite_array_uint32* dest, size_t dest_pos, size_t dest_count)
{
    return robotraconteurlite_buffer_vec_copy_to_mem(source_buf, source_buf_pos, (uint8_t*)dest->data, dest->len,
                                                     dest_pos, sizeof(uint32_t), dest_count);
}

robotraconteurlite_status robotraconteurlite_buffer_vec_copy_from_uint32_array(
    struct robotraconteurlite_buffer_vec* dest_buf, size_t dest_buf_pos,
    const struct robotraconteurlite_array_uint32* source, size_t source_pos, size_t source_count)
{
    return robotraconteurlite_buffer_vec_copy_from_mem(dest_buf, dest_buf_pos, (const uint8_t*)source->data,
                                                       source->len, source_pos, sizeof(uint32_t), source_count);
}

robotraconteurlite_status robotraconteurlite_buffer_vec_copy_to_uint32(
    const struct robotraconteurlite_buffer_vec* source_buf, size_t source_buf_pos, uint32_t* dest)
{
    return robotraconteurlite_buffer_vec_copy_to_mem(source_buf, source_buf_pos, (uint8_t*)dest, 1, 0, sizeof(uint32_t),
                                                     1);
}

robotraconteurlite_status robotraconteurlite_buffer_vec_copy_from_uint32(struct robotraconteurlite_buffer_vec* dest_buf,
                                                                         size_t dest_buf_pos, uint32_t source)
{
    return robotraconteurlite_buffer_vec_copy_from_mem(dest_buf, dest_buf_pos, (const uint8_t*)&source, 1, 0,
                                                       sizeof(uint32_t), 1);
}

/* int64 */
robotraconteurlite_status robotraconteurlite_buffer_vec_copy_to_int64_array(
    const struct robotraconteurlite_buffer_vec* source_buf, size_t source_buf_pos,
    struct robotraconteurlite_array_int64* dest, size_t dest_pos, size_t dest_count)
{
    return robotraconteurlite_buffer_vec_copy_to_mem(source_buf, source_buf_pos, (uint8_t*)dest->data, dest->len,
                                                     dest_pos, sizeof(int64_t), dest_count);
}

robotraconteurlite_status robotraconteurlite_buffer_vec_copy_from_int64_array(
    struct robotraconteurlite_buffer_vec* dest_buf, size_t dest_buf_pos,
    const struct robotraconteurlite_array_int64* source, size_t source_pos, size_t source_count)
{
    return robotraconteurlite_buffer_vec_copy_from_mem(dest_buf, dest_buf_pos, (const uint8_t*)source->data,
                                                       source->len, source_pos, sizeof(int64_t), source_count);
}

robotraconteurlite_status robotraconteurlite_buffer_vec_copy_to_int64(
    const struct robotraconteurlite_buffer_vec* source_buf, size_t source_buf_pos, int64_t* dest)
{
    return robotraconteurlite_buffer_vec_copy_to_mem(source_buf, source_buf_pos, (uint8_t*)dest, 1, 0, sizeof(int64_t),
                                                     1);
}

robotraconteurlite_status robotraconteurlite_buffer_vec_copy_from_int64(struct robotraconteurlite_buffer_vec* dest_buf,
                                                                        size_t dest_buf_pos, int64_t source)
{
    return robotraconteurlite_buffer_vec_copy_from_mem(dest_buf, dest_buf_pos, (const uint8_t*)&source, 1, 0,
                                                       sizeof(int64_t), 1);
}

/* uint64 */
robotraconteurlite_status robotraconteurlite_buffer_vec_copy_to_uint64_array(
    const struct robotraconteurlite_buffer_vec* source_buf, size_t source_buf_pos,
    struct robotraconteurlite_array_uint64* dest, size_t dest_pos, size_t dest_count)
{
    return robotraconteurlite_buffer_vec_copy_to_mem(source_buf, source_buf_pos, (uint8_t*)dest->data, dest->len,
                                                     dest_pos, sizeof(uint64_t), dest_count);
}

robotraconteurlite_status robotraconteurlite_buffer_vec_copy_from_uint64_array(
    struct robotraconteurlite_buffer_vec* dest_buf, size_t dest_buf_pos,
    const struct robotraconteurlite_array_uint64* source, size_t source_pos, size_t source_count)
{
    return robotraconteurlite_buffer_vec_copy_from_mem(dest_buf, dest_buf_pos, (const uint8_t*)source->data,
                                                       source->len, source_pos, sizeof(uint64_t), source_count);
}

robotraconteurlite_status robotraconteurlite_buffer_vec_copy_to_uint64(
    const struct robotraconteurlite_buffer_vec* source_buf, size_t source_buf_pos, uint64_t* dest)
{
    return robotraconteurlite_buffer_vec_copy_to_mem(source_buf, source_buf_pos, (uint8_t*)dest, 1, 0, sizeof(uint64_t),
                                                     1);
}

robotraconteurlite_status robotraconteurlite_buffer_vec_copy_from_uint64(struct robotraconteurlite_buffer_vec* dest_buf,
                                                                         size_t dest_buf_pos, uint64_t source)
{
    return robotraconteurlite_buffer_vec_copy_from_mem(dest_buf, dest_buf_pos, (const uint8_t*)&source, 1, 0,
                                                       sizeof(uint64_t), 1);
}

/* cdouble */
robotraconteurlite_status robotraconteurlite_buffer_vec_copy_to_cdouble_array(
    const struct robotraconteurlite_buffer_vec* source_buf, size_t source_buf_pos,
    struct robotraconteurlite_array_cdouble* dest, size_t dest_pos, size_t dest_count)
{
    return robotraconteurlite_buffer_vec_copy_to_mem(source_buf, source_buf_pos, (uint8_t*)dest->data, dest->len,
                                                     dest_pos, sizeof(struct robotraconteurlite_cdouble), dest_count);
}

robotraconteurlite_status robotraconteurlite_buffer_vec_copy_from_cdouble_array(
    struct robotraconteurlite_buffer_vec* dest_buf, size_t dest_buf_pos,
    const struct robotraconteurlite_array_cdouble* source, size_t source_pos, size_t source_count)
{
    return robotraconteurlite_buffer_vec_copy_from_mem(dest_buf, dest_buf_pos, (const uint8_t*)source->data,
                                                       source->len, source_pos,
                                                       sizeof(struct robotraconteurlite_cdouble), source_count);
}

robotraconteurlite_status robotraconteurlite_buffer_vec_copy_to_cdouble(
    const struct robotraconteurlite_buffer_vec* source_buf, size_t source_buf_pos,
    struct robotraconteurlite_cdouble* dest)
{
    return robotraconteurlite_buffer_vec_copy_to_mem(source_buf, source_buf_pos, (uint8_t*)dest, 1, 0,
                                                     sizeof(struct robotraconteurlite_cdouble), 1);
}

robotraconteurlite_status robotraconteurlite_buffer_vec_copy_from_cdouble(
    struct robotraconteurlite_buffer_vec* dest_buf, size_t dest_buf_pos, struct robotraconteurlite_cdouble source)
{
    return robotraconteurlite_buffer_vec_copy_from_mem(dest_buf, dest_buf_pos, (const uint8_t*)&source, 1, 0,
                                                       sizeof(struct robotraconteurlite_cdouble), 1);
}

/* csingle */
robotraconteurlite_status robotraconteurlite_buffer_vec_copy_to_csingle_array(
    const struct robotraconteurlite_buffer_vec* source_buf, size_t source_buf_pos,
    struct robotraconteurlite_array_csingle* dest, size_t dest_pos, size_t dest_count)
{
    return robotraconteurlite_buffer_vec_copy_to_mem(source_buf, source_buf_pos, (uint8_t*)dest->data, dest->len,
                                                     dest_pos, sizeof(struct robotraconteurlite_csingle), dest_count);
}

robotraconteurlite_status robotraconteurlite_buffer_vec_copy_from_csingle_array(
    struct robotraconteurlite_buffer_vec* dest_buf, size_t dest_buf_pos,
    const struct robotraconteurlite_array_csingle* source, size_t source_pos, size_t source_count)
{
    return robotraconteurlite_buffer_vec_copy_from_mem(dest_buf, dest_buf_pos, (const uint8_t*)source->data,
                                                       source->len, source_pos,
                                                       sizeof(struct robotraconteurlite_csingle), source_count);
}

robotraconteurlite_status robotraconteurlite_buffer_vec_copy_to_csingle(
    const struct robotraconteurlite_buffer_vec* source_buf, size_t source_buf_pos,
    struct robotraconteurlite_csingle* dest)
{
    return robotraconteurlite_buffer_vec_copy_to_mem(source_buf, source_buf_pos, (uint8_t*)dest, 1, 0,
                                                     sizeof(struct robotraconteurlite_csingle), 1);
}

robotraconteurlite_status robotraconteurlite_buffer_vec_copy_from_csingle(
    struct robotraconteurlite_buffer_vec* dest_buf, size_t dest_buf_pos, struct robotraconteurlite_csingle source)
{
    return robotraconteurlite_buffer_vec_copy_from_mem(dest_buf, dest_buf_pos, (const uint8_t*)&source, 1, 0,
                                                       sizeof(struct robotraconteurlite_csingle), 1);
}

/* bool */
robotraconteurlite_status robotraconteurlite_buffer_vec_copy_to_bool_array(
    const struct robotraconteurlite_buffer_vec* source_buf, size_t source_buf_pos,
    struct robotraconteurlite_array_bool* dest, size_t dest_pos, size_t dest_count)
{
    return robotraconteurlite_buffer_vec_copy_to_mem(source_buf, source_buf_pos, (uint8_t*)dest->data, dest->len,
                                                     dest_pos, sizeof(struct robotraconteurlite_bool), dest_count);
}

robotraconteurlite_status robotraconteurlite_buffer_vec_copy_from_bool_array(
    struct robotraconteurlite_buffer_vec* dest_buf, size_t dest_buf_pos,
    const struct robotraconteurlite_array_bool* source, size_t source_pos, size_t source_count)
{
    return robotraconteurlite_buffer_vec_copy_from_mem(dest_buf, dest_buf_pos, (const uint8_t*)source->data,
                                                       source->len, source_pos, sizeof(struct robotraconteurlite_bool),
                                                       source_count);
}

robotraconteurlite_status robotraconteurlite_buffer_vec_copy_to_bool(
    const struct robotraconteurlite_buffer_vec* source_buf, size_t source_buf_pos, struct robotraconteurlite_bool* dest)
{
    return robotraconteurlite_buffer_vec_copy_to_mem(source_buf, source_buf_pos, (uint8_t*)dest, 1, 0,
                                                     sizeof(struct robotraconteurlite_bool), 1);
}

robotraconteurlite_status robotraconteurlite_buffer_vec_copy_from_bool(struct robotraconteurlite_buffer_vec* dest_buf,
                                                                       size_t dest_buf_pos,
                                                                       struct robotraconteurlite_bool source)
{
    return robotraconteurlite_buffer_vec_copy_from_mem(dest_buf, dest_buf_pos, (const uint8_t*)&source, 1, 0,
                                                       sizeof(struct robotraconteurlite_bool), 1);
}
