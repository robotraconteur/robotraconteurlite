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

#include <robotraconteurlite/message.h>
#include <string.h>
#include <assert.h>
#include <math.h>

/* readers */
/* double */
robotraconteurlite_status robotraconteurlite_messageelement_reader_read_data_double_array(
    struct robotraconteurlite_messageelement_reader* element_reader, struct robotraconteurlite_array_double* dest_array)
{
    return robotraconteurlite_messageelement_reader_read_data_ex(element_reader,
                                                                 (struct robotraconteurlite_buffer*)dest_array,
                                                                 ROBOTRACONTEURLITE_DATATYPE_DOUBLE, sizeof(double));
}

robotraconteurlite_status robotraconteurlite_messageelement_reader_read_data_double(
    struct robotraconteurlite_messageelement_reader* element_reader, double* value)
{
    return robotraconteurlite_messageelement_reader_read_data_scalar_ex(
        element_reader, value, ROBOTRACONTEURLITE_DATATYPE_DOUBLE, sizeof(double));
}
/* single */
robotraconteurlite_status robotraconteurlite_messageelement_reader_read_data_single_array(
    struct robotraconteurlite_messageelement_reader* element_reader, struct robotraconteurlite_array_single* dest_array)
{
    return robotraconteurlite_messageelement_reader_read_data_ex(element_reader,
                                                                 (struct robotraconteurlite_buffer*)dest_array,
                                                                 ROBOTRACONTEURLITE_DATATYPE_SINGLE, sizeof(float));
}

robotraconteurlite_status robotraconteurlite_messageelement_reader_read_data_single(
    struct robotraconteurlite_messageelement_reader* element_reader, float* value)
{
    return robotraconteurlite_messageelement_reader_read_data_scalar_ex(
        element_reader, value, ROBOTRACONTEURLITE_DATATYPE_SINGLE, sizeof(float));
}
/* int8 */
robotraconteurlite_status robotraconteurlite_messageelement_reader_read_data_int8_array(
    struct robotraconteurlite_messageelement_reader* element_reader, struct robotraconteurlite_array_int8* dest_array)
{
    return robotraconteurlite_messageelement_reader_read_data_ex(element_reader,
                                                                 (struct robotraconteurlite_buffer*)dest_array,
                                                                 ROBOTRACONTEURLITE_DATATYPE_INT8, sizeof(int8_t));
}

robotraconteurlite_status robotraconteurlite_messageelement_reader_read_data_int8(
    struct robotraconteurlite_messageelement_reader* element_reader, int8_t* value)
{
    return robotraconteurlite_messageelement_reader_read_data_scalar_ex(
        element_reader, value, ROBOTRACONTEURLITE_DATATYPE_INT8, sizeof(int8_t));
}
/* uint8 */
robotraconteurlite_status robotraconteurlite_messageelement_reader_read_data_uint8_array(
    struct robotraconteurlite_messageelement_reader* element_reader, struct robotraconteurlite_array_uint8* dest_array)
{
    return robotraconteurlite_messageelement_reader_read_data_ex(element_reader,
                                                                 (struct robotraconteurlite_buffer*)dest_array,
                                                                 ROBOTRACONTEURLITE_DATATYPE_UINT8, sizeof(uint8_t));
}

robotraconteurlite_status robotraconteurlite_messageelement_reader_read_data_uint8(
    struct robotraconteurlite_messageelement_reader* element_reader, uint8_t* value)
{
    return robotraconteurlite_messageelement_reader_read_data_scalar_ex(
        element_reader, value, ROBOTRACONTEURLITE_DATATYPE_UINT8, sizeof(uint8_t));
}
/* int16 */
robotraconteurlite_status robotraconteurlite_messageelement_reader_read_data_int16_array(
    struct robotraconteurlite_messageelement_reader* element_reader, struct robotraconteurlite_array_int16* dest_array)
{
    return robotraconteurlite_messageelement_reader_read_data_ex(element_reader,
                                                                 (struct robotraconteurlite_buffer*)dest_array,
                                                                 ROBOTRACONTEURLITE_DATATYPE_INT16, sizeof(int16_t));
}

robotraconteurlite_status robotraconteurlite_messageelement_reader_read_data_int16(
    struct robotraconteurlite_messageelement_reader* element_reader, int16_t* value)
{
    return robotraconteurlite_messageelement_reader_read_data_scalar_ex(
        element_reader, value, ROBOTRACONTEURLITE_DATATYPE_INT16, sizeof(int16_t));
}
/* uint16 */
robotraconteurlite_status robotraconteurlite_messageelement_reader_read_data_uint16_array(
    struct robotraconteurlite_messageelement_reader* element_reader, struct robotraconteurlite_array_uint16* dest_array)
{
    return robotraconteurlite_messageelement_reader_read_data_ex(element_reader,
                                                                 (struct robotraconteurlite_buffer*)dest_array,
                                                                 ROBOTRACONTEURLITE_DATATYPE_UINT16, sizeof(uint16_t));
}

robotraconteurlite_status robotraconteurlite_messageelement_reader_read_data_uint16(
    struct robotraconteurlite_messageelement_reader* element_reader, uint16_t* value)
{
    return robotraconteurlite_messageelement_reader_read_data_scalar_ex(
        element_reader, value, ROBOTRACONTEURLITE_DATATYPE_UINT16, sizeof(uint16_t));
}
/* int32 */
robotraconteurlite_status robotraconteurlite_messageelement_reader_read_data_int32_array(
    struct robotraconteurlite_messageelement_reader* element_reader, struct robotraconteurlite_array_int32* dest_array)
{
    return robotraconteurlite_messageelement_reader_read_data_ex(element_reader,
                                                                 (struct robotraconteurlite_buffer*)dest_array,
                                                                 ROBOTRACONTEURLITE_DATATYPE_INT32, sizeof(int32_t));
}

robotraconteurlite_status robotraconteurlite_messageelement_reader_read_data_int32(
    struct robotraconteurlite_messageelement_reader* element_reader, int32_t* value)
{
    return robotraconteurlite_messageelement_reader_read_data_scalar_ex(
        element_reader, value, ROBOTRACONTEURLITE_DATATYPE_INT32, sizeof(int32_t));
}
/* uint32 */
robotraconteurlite_status robotraconteurlite_messageelement_reader_read_data_uint32_array(
    struct robotraconteurlite_messageelement_reader* element_reader, struct robotraconteurlite_array_uint32* dest_array)
{
    return robotraconteurlite_messageelement_reader_read_data_ex(element_reader,
                                                                 (struct robotraconteurlite_buffer*)dest_array,
                                                                 ROBOTRACONTEURLITE_DATATYPE_UINT32, sizeof(uint32_t));
}

robotraconteurlite_status robotraconteurlite_messageelement_reader_read_data_uint32(
    struct robotraconteurlite_messageelement_reader* element_reader, uint32_t* value)
{
    return robotraconteurlite_messageelement_reader_read_data_scalar_ex(
        element_reader, value, ROBOTRACONTEURLITE_DATATYPE_UINT32, sizeof(uint32_t));
}
/* int64 */
robotraconteurlite_status robotraconteurlite_messageelement_reader_read_data_int64_array(
    struct robotraconteurlite_messageelement_reader* element_reader, struct robotraconteurlite_array_int64* dest_array)
{
    return robotraconteurlite_messageelement_reader_read_data_ex(element_reader,
                                                                 (struct robotraconteurlite_buffer*)dest_array,
                                                                 ROBOTRACONTEURLITE_DATATYPE_INT64, sizeof(int64_t));
}

robotraconteurlite_status robotraconteurlite_messageelement_reader_read_data_int64(
    struct robotraconteurlite_messageelement_reader* element_reader, int64_t* value)
{
    return robotraconteurlite_messageelement_reader_read_data_scalar_ex(
        element_reader, value, ROBOTRACONTEURLITE_DATATYPE_INT64, sizeof(int64_t));
}
/* uint64 */
robotraconteurlite_status robotraconteurlite_messageelement_reader_read_data_uint64_array(
    struct robotraconteurlite_messageelement_reader* element_reader, struct robotraconteurlite_array_uint64* dest_array)
{
    return robotraconteurlite_messageelement_reader_read_data_ex(element_reader,
                                                                 (struct robotraconteurlite_buffer*)dest_array,
                                                                 ROBOTRACONTEURLITE_DATATYPE_UINT64, sizeof(uint64_t));
}

robotraconteurlite_status robotraconteurlite_messageelement_reader_read_data_uint64(
    struct robotraconteurlite_messageelement_reader* element_reader, uint64_t* value)
{
    return robotraconteurlite_messageelement_reader_read_data_scalar_ex(
        element_reader, value, ROBOTRACONTEURLITE_DATATYPE_UINT64, sizeof(uint64_t));
}
/* cdouble */
robotraconteurlite_status robotraconteurlite_messageelement_reader_read_data_cdouble_array(
    struct robotraconteurlite_messageelement_reader* element_reader,
    struct robotraconteurlite_array_cdouble* dest_array)
{
    return robotraconteurlite_messageelement_reader_read_data_ex(
        element_reader, (struct robotraconteurlite_buffer*)dest_array, ROBOTRACONTEURLITE_DATATYPE_CDOUBLE,
        sizeof(struct robotraconteurlite_cdouble));
}

robotraconteurlite_status robotraconteurlite_messageelement_reader_read_data_cdouble(
    struct robotraconteurlite_messageelement_reader* element_reader, struct robotraconteurlite_cdouble* value)
{
    return robotraconteurlite_messageelement_reader_read_data_scalar_ex(
        element_reader, value, ROBOTRACONTEURLITE_DATATYPE_CDOUBLE, sizeof(struct robotraconteurlite_cdouble));
}
/* csingle */
robotraconteurlite_status robotraconteurlite_messageelement_reader_read_data_csingle_array(
    struct robotraconteurlite_messageelement_reader* element_reader,
    struct robotraconteurlite_array_csingle* dest_array)
{
    return robotraconteurlite_messageelement_reader_read_data_ex(
        element_reader, (struct robotraconteurlite_buffer*)dest_array, ROBOTRACONTEURLITE_DATATYPE_CSINGLE,
        sizeof(struct robotraconteurlite_csingle));
}

robotraconteurlite_status robotraconteurlite_messageelement_reader_read_data_csingle(
    struct robotraconteurlite_messageelement_reader* element_reader, struct robotraconteurlite_csingle* value)
{
    return robotraconteurlite_messageelement_reader_read_data_scalar_ex(
        element_reader, value, ROBOTRACONTEURLITE_DATATYPE_CSINGLE, sizeof(struct robotraconteurlite_csingle));
}
/* bool */
robotraconteurlite_status robotraconteurlite_messageelement_reader_read_data_bool_array(
    struct robotraconteurlite_messageelement_reader* element_reader, struct robotraconteurlite_array_bool* dest_array)
{
    return robotraconteurlite_messageelement_reader_read_data_ex(
        element_reader, (struct robotraconteurlite_buffer*)dest_array, ROBOTRACONTEURLITE_DATATYPE_BOOL,
        sizeof(struct robotraconteurlite_bool));
}

robotraconteurlite_status robotraconteurlite_messageelement_reader_read_data_bool(
    struct robotraconteurlite_messageelement_reader* element_reader, struct robotraconteurlite_bool* value)
{
    return robotraconteurlite_messageelement_reader_read_data_scalar_ex(
        element_reader, value, ROBOTRACONTEURLITE_DATATYPE_BOOL, sizeof(struct robotraconteurlite_bool));
}

/* writers */
/* double */
robotraconteurlite_status robotraconteurlite_messageelement_writer_write_double(
    struct robotraconteurlite_messageelement_writer* element_writer,
    const struct robotraconteurlite_string* element_name, double value)
{
    struct robotraconteurlite_array_double a;
    a.data = &value;
    a.len = 1;
    return robotraconteurlite_messageelement_writer_write_raw(element_writer, element_name,
                                                              (struct robotraconteurlite_buffer*)&a,
                                                              ROBOTRACONTEURLITE_DATATYPE_DOUBLE, sizeof(double));
}

robotraconteurlite_status robotraconteurlite_messageelement_writer_write_double_array(
    struct robotraconteurlite_messageelement_writer* element_writer,
    const struct robotraconteurlite_string* element_name, const struct robotraconteurlite_array_double* value)
{
    return robotraconteurlite_messageelement_writer_write_raw(element_writer, element_name,
                                                              (struct robotraconteurlite_buffer*)value,
                                                              ROBOTRACONTEURLITE_DATATYPE_DOUBLE, sizeof(double));
}
/* single */
robotraconteurlite_status robotraconteurlite_messageelement_writer_write_single(
    struct robotraconteurlite_messageelement_writer* element_writer,
    const struct robotraconteurlite_string* element_name, float value)
{
    struct robotraconteurlite_array_single a;
    a.data = &value;
    a.len = 1;
    return robotraconteurlite_messageelement_writer_write_raw(element_writer, element_name,
                                                              (struct robotraconteurlite_buffer*)&a,
                                                              ROBOTRACONTEURLITE_DATATYPE_SINGLE, sizeof(float));
}

robotraconteurlite_status robotraconteurlite_messageelement_writer_write_single_array(
    struct robotraconteurlite_messageelement_writer* element_writer,
    const struct robotraconteurlite_string* element_name, const struct robotraconteurlite_array_single* value)
{
    return robotraconteurlite_messageelement_writer_write_raw(element_writer, element_name,
                                                              (struct robotraconteurlite_buffer*)value,
                                                              ROBOTRACONTEURLITE_DATATYPE_SINGLE, sizeof(float));
}
/* int8 */
robotraconteurlite_status robotraconteurlite_messageelement_writer_write_int8(
    struct robotraconteurlite_messageelement_writer* element_writer,
    const struct robotraconteurlite_string* element_name, int8_t value)
{
    struct robotraconteurlite_array_int8 a;
    a.data = &value;
    a.len = 1;
    return robotraconteurlite_messageelement_writer_write_raw(element_writer, element_name,
                                                              (struct robotraconteurlite_buffer*)&a,
                                                              ROBOTRACONTEURLITE_DATATYPE_INT8, sizeof(int8_t));
}

robotraconteurlite_status robotraconteurlite_messageelement_writer_write_int8_array(
    struct robotraconteurlite_messageelement_writer* element_writer,
    const struct robotraconteurlite_string* element_name, const struct robotraconteurlite_array_int8* value)
{
    return robotraconteurlite_messageelement_writer_write_raw(element_writer, element_name,
                                                              (struct robotraconteurlite_buffer*)value,
                                                              ROBOTRACONTEURLITE_DATATYPE_INT8, sizeof(int8_t));
}
/* uint8 */
robotraconteurlite_status robotraconteurlite_messageelement_writer_write_uint8(
    struct robotraconteurlite_messageelement_writer* element_writer,
    const struct robotraconteurlite_string* element_name, uint8_t value)
{
    struct robotraconteurlite_array_uint8 a;
    a.data = &value;
    a.len = 1;
    return robotraconteurlite_messageelement_writer_write_raw(element_writer, element_name,
                                                              (struct robotraconteurlite_buffer*)&a,
                                                              ROBOTRACONTEURLITE_DATATYPE_UINT8, sizeof(uint8_t));
}

robotraconteurlite_status robotraconteurlite_messageelement_writer_write_uint8_array(
    struct robotraconteurlite_messageelement_writer* element_writer,
    const struct robotraconteurlite_string* element_name, const struct robotraconteurlite_array_uint8* value)
{
    return robotraconteurlite_messageelement_writer_write_raw(element_writer, element_name,
                                                              (struct robotraconteurlite_buffer*)value,
                                                              ROBOTRACONTEURLITE_DATATYPE_UINT8, sizeof(uint8_t));
}
/* int16 */
robotraconteurlite_status robotraconteurlite_messageelement_writer_write_int16(
    struct robotraconteurlite_messageelement_writer* element_writer,
    const struct robotraconteurlite_string* element_name, int16_t value)
{
    struct robotraconteurlite_array_int16 a;
    a.data = &value;
    a.len = 1;
    return robotraconteurlite_messageelement_writer_write_raw(element_writer, element_name,
                                                              (struct robotraconteurlite_buffer*)&a,
                                                              ROBOTRACONTEURLITE_DATATYPE_INT16, sizeof(int16_t));
}

robotraconteurlite_status robotraconteurlite_messageelement_writer_write_int16_array(
    struct robotraconteurlite_messageelement_writer* element_writer,
    const struct robotraconteurlite_string* element_name, const struct robotraconteurlite_array_int16* value)
{
    return robotraconteurlite_messageelement_writer_write_raw(element_writer, element_name,
                                                              (struct robotraconteurlite_buffer*)value,
                                                              ROBOTRACONTEURLITE_DATATYPE_INT16, sizeof(int16_t));
}
/* uint16 */
robotraconteurlite_status robotraconteurlite_messageelement_writer_write_uint16(
    struct robotraconteurlite_messageelement_writer* element_writer,
    const struct robotraconteurlite_string* element_name, uint16_t value)
{
    struct robotraconteurlite_array_uint16 a;
    a.data = &value;
    a.len = 1;
    return robotraconteurlite_messageelement_writer_write_raw(element_writer, element_name,
                                                              (struct robotraconteurlite_buffer*)&a,
                                                              ROBOTRACONTEURLITE_DATATYPE_UINT16, sizeof(uint16_t));
}

robotraconteurlite_status robotraconteurlite_messageelement_writer_write_uint16_array(
    struct robotraconteurlite_messageelement_writer* element_writer,
    const struct robotraconteurlite_string* element_name, const struct robotraconteurlite_array_uint16* value)
{
    return robotraconteurlite_messageelement_writer_write_raw(element_writer, element_name,
                                                              (struct robotraconteurlite_buffer*)value,
                                                              ROBOTRACONTEURLITE_DATATYPE_UINT16, sizeof(uint16_t));
}
/* int32 */
robotraconteurlite_status robotraconteurlite_messageelement_writer_write_int32(
    struct robotraconteurlite_messageelement_writer* element_writer,
    const struct robotraconteurlite_string* element_name, int32_t value)
{
    struct robotraconteurlite_array_int32 a;
    a.data = &value;
    a.len = 1;
    return robotraconteurlite_messageelement_writer_write_raw(element_writer, element_name,
                                                              (struct robotraconteurlite_buffer*)&a,
                                                              ROBOTRACONTEURLITE_DATATYPE_INT32, sizeof(int32_t));
}

robotraconteurlite_status robotraconteurlite_messageelement_writer_write_int32_array(
    struct robotraconteurlite_messageelement_writer* element_writer,
    const struct robotraconteurlite_string* element_name, const struct robotraconteurlite_array_int32* value)
{
    return robotraconteurlite_messageelement_writer_write_raw(element_writer, element_name,
                                                              (struct robotraconteurlite_buffer*)value,
                                                              ROBOTRACONTEURLITE_DATATYPE_INT32, sizeof(int32_t));
}
/* uint32 */
robotraconteurlite_status robotraconteurlite_messageelement_writer_write_uint32(
    struct robotraconteurlite_messageelement_writer* element_writer,
    const struct robotraconteurlite_string* element_name, uint32_t value)
{
    struct robotraconteurlite_array_uint32 a;
    a.data = &value;
    a.len = 1;
    return robotraconteurlite_messageelement_writer_write_raw(element_writer, element_name,
                                                              (struct robotraconteurlite_buffer*)&a,
                                                              ROBOTRACONTEURLITE_DATATYPE_UINT32, sizeof(uint32_t));
}

robotraconteurlite_status robotraconteurlite_messageelement_writer_write_uint32_array(
    struct robotraconteurlite_messageelement_writer* element_writer,
    const struct robotraconteurlite_string* element_name, const struct robotraconteurlite_array_uint32* value)
{
    return robotraconteurlite_messageelement_writer_write_raw(element_writer, element_name,
                                                              (struct robotraconteurlite_buffer*)value,
                                                              ROBOTRACONTEURLITE_DATATYPE_UINT32, sizeof(uint32_t));
}
/* int64 */
robotraconteurlite_status robotraconteurlite_messageelement_writer_write_int64(
    struct robotraconteurlite_messageelement_writer* element_writer,
    const struct robotraconteurlite_string* element_name, int64_t value)
{
    struct robotraconteurlite_array_int64 a;
    a.data = &value;
    a.len = 1;
    return robotraconteurlite_messageelement_writer_write_raw(element_writer, element_name,
                                                              (struct robotraconteurlite_buffer*)&a,
                                                              ROBOTRACONTEURLITE_DATATYPE_INT64, sizeof(int64_t));
}

robotraconteurlite_status robotraconteurlite_messageelement_writer_write_int64_array(
    struct robotraconteurlite_messageelement_writer* element_writer,
    const struct robotraconteurlite_string* element_name, const struct robotraconteurlite_array_int64* value)
{
    return robotraconteurlite_messageelement_writer_write_raw(element_writer, element_name,
                                                              (struct robotraconteurlite_buffer*)value,
                                                              ROBOTRACONTEURLITE_DATATYPE_INT64, sizeof(int64_t));
}
/* uint64 */
robotraconteurlite_status robotraconteurlite_messageelement_writer_write_uint64(
    struct robotraconteurlite_messageelement_writer* element_writer,
    const struct robotraconteurlite_string* element_name, uint64_t value)
{
    struct robotraconteurlite_array_uint64 a;
    a.data = &value;
    a.len = 1;
    return robotraconteurlite_messageelement_writer_write_raw(element_writer, element_name,
                                                              (struct robotraconteurlite_buffer*)&a,
                                                              ROBOTRACONTEURLITE_DATATYPE_UINT64, sizeof(uint64_t));
}

robotraconteurlite_status robotraconteurlite_messageelement_writer_write_uint64_array(
    struct robotraconteurlite_messageelement_writer* element_writer,
    const struct robotraconteurlite_string* element_name, const struct robotraconteurlite_array_uint64* value)
{
    return robotraconteurlite_messageelement_writer_write_raw(element_writer, element_name,
                                                              (struct robotraconteurlite_buffer*)value,
                                                              ROBOTRACONTEURLITE_DATATYPE_UINT64, sizeof(uint64_t));
}
/* cdouble */
robotraconteurlite_status robotraconteurlite_messageelement_writer_write_cdouble(
    struct robotraconteurlite_messageelement_writer* element_writer,
    const struct robotraconteurlite_string* element_name, struct robotraconteurlite_cdouble value)
{
    struct robotraconteurlite_array_cdouble a;
    a.data = &value;
    a.len = 1;
    return robotraconteurlite_messageelement_writer_write_raw(
        element_writer, element_name, (struct robotraconteurlite_buffer*)&a, ROBOTRACONTEURLITE_DATATYPE_CDOUBLE,
        sizeof(struct robotraconteurlite_cdouble));
}

robotraconteurlite_status robotraconteurlite_messageelement_writer_write_cdouble_array(
    struct robotraconteurlite_messageelement_writer* element_writer,
    const struct robotraconteurlite_string* element_name, const struct robotraconteurlite_array_cdouble* value)
{
    return robotraconteurlite_messageelement_writer_write_raw(
        element_writer, element_name, (struct robotraconteurlite_buffer*)value, ROBOTRACONTEURLITE_DATATYPE_CDOUBLE,
        sizeof(struct robotraconteurlite_cdouble));
}
/* csingle */
robotraconteurlite_status robotraconteurlite_messageelement_writer_write_csingle(
    struct robotraconteurlite_messageelement_writer* element_writer,
    const struct robotraconteurlite_string* element_name, struct robotraconteurlite_csingle value)
{
    struct robotraconteurlite_array_csingle a;
    a.data = &value;
    a.len = 1;
    return robotraconteurlite_messageelement_writer_write_raw(
        element_writer, element_name, (struct robotraconteurlite_buffer*)&a, ROBOTRACONTEURLITE_DATATYPE_CSINGLE,
        sizeof(struct robotraconteurlite_csingle));
}

robotraconteurlite_status robotraconteurlite_messageelement_writer_write_csingle_array(
    struct robotraconteurlite_messageelement_writer* element_writer,
    const struct robotraconteurlite_string* element_name, const struct robotraconteurlite_array_csingle* value)
{
    return robotraconteurlite_messageelement_writer_write_raw(
        element_writer, element_name, (struct robotraconteurlite_buffer*)value, ROBOTRACONTEURLITE_DATATYPE_CSINGLE,
        sizeof(struct robotraconteurlite_csingle));
}
/* bool */
robotraconteurlite_status robotraconteurlite_messageelement_writer_write_bool(
    struct robotraconteurlite_messageelement_writer* element_writer,
    const struct robotraconteurlite_string* element_name, struct robotraconteurlite_bool value)
{
    struct robotraconteurlite_array_bool a;
    a.data = &value;
    a.len = 1;
    return robotraconteurlite_messageelement_writer_write_raw(
        element_writer, element_name, (struct robotraconteurlite_buffer*)&a, ROBOTRACONTEURLITE_DATATYPE_BOOL,
        sizeof(struct robotraconteurlite_bool));
}

robotraconteurlite_status robotraconteurlite_messageelement_writer_write_bool_array(
    struct robotraconteurlite_messageelement_writer* element_writer,
    const struct robotraconteurlite_string* element_name, const struct robotraconteurlite_array_bool* value)
{
    return robotraconteurlite_messageelement_writer_write_raw(
        element_writer, element_name, (struct robotraconteurlite_buffer*)value, ROBOTRACONTEURLITE_DATATYPE_BOOL,
        sizeof(struct robotraconteurlite_bool));
}

/* string */

robotraconteurlite_status robotraconteurlite_messageelement_reader_read_data_string(
    struct robotraconteurlite_messageelement_reader* element_reader, struct robotraconteurlite_string* dest_str)
{
    return robotraconteurlite_messageelement_reader_read_data_ex(
        element_reader, (struct robotraconteurlite_buffer*)dest_str, ROBOTRACONTEURLITE_DATATYPE_STRING, sizeof(char));
}

robotraconteurlite_status robotraconteurlite_messageelement_writer_write_data_string(
    struct robotraconteurlite_messageelement_writer* element_writer,
    const struct robotraconteurlite_string* element_name, const struct robotraconteurlite_string* value)
{
    return robotraconteurlite_messageelement_writer_write_raw(element_writer, element_name,
                                                              (struct robotraconteurlite_buffer*)value,
                                                              ROBOTRACONTEURLITE_DATATYPE_STRING, sizeof(char));
}
