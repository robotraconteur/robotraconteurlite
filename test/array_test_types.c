

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>

#include "robotraconteurlite/array.h"
#define inline
#include <cmocka.h>

void robotraconteurlite_buffer_uint8_test(void** state)
{
    uint8_t a1[256];
    uint8_t a2[256];
    uint8_t buf_data[256 * sizeof(uint8_t)];
    size_t i = 0;
    struct robotraconteurlite_buffer buf;
    struct robotraconteurlite_buffer_vec buf_vec;
    struct robotraconteurlite_array_uint8 uint8s;
    struct robotraconteurlite_array_uint8 uint8s2;

    uint8_t d1 = 10;
    uint8_t d2 = 0;

    ROBOTRACONTEURLITE_UNUSED(state);

    for (i = 0; i < sizeof(a1) / sizeof(uint8_t); i++)
        a1[i] = (uint8_t)i;
    (void)memset(a2, 0, sizeof(a2));
    (void)memset(buf_data, 0, sizeof(buf_data));

    buf.data = buf_data;
    buf.len = sizeof(buf_data);

    buf_vec.buffer_vec = &buf;
    buf_vec.buffer_vec_cnt = 1;

    uint8s.data = a1;
    uint8s.len = sizeof(a1) / sizeof(uint8_t);

    assert_return_code(robotraconteurlite_buffer_vec_copy_from_uint8_array(&buf_vec, 32, &uint8s, 10, 15), 0);

    assert_true(memcmp((a1 + 10), (buf_data + 32), 15 * sizeof(uint8_t)) == 0);

    uint8s2.data = a2;
    uint8s2.len = sizeof(a2) / sizeof(uint8_t);

    assert_return_code(robotraconteurlite_buffer_vec_copy_to_uint8_array(&buf_vec, 32, &uint8s2, 12, 12), 0);

    assert_true(memcmp(a1 + 10, a2 + 12, 12 * sizeof(uint8_t)) == 0);

    robotraconteurlite_buffer_vec_copy_from_uint8(&buf_vec, 128, d1);
    assert_true(memcmp(&d1, (buf_data + 128), sizeof(uint8_t)) == 0);
    robotraconteurlite_buffer_vec_copy_to_uint8(&buf_vec, 32 + sizeof(uint8_t), &d2);
    assert_true(d2 == 11);
}

void robotraconteurlite_buffer_int8_test(void** state)
{
    int8_t a1[256];
    int8_t a2[256];
    uint8_t buf_data[256 * sizeof(int8_t)];
    size_t i = 0;
    struct robotraconteurlite_buffer buf;
    struct robotraconteurlite_buffer_vec buf_vec;
    struct robotraconteurlite_array_int8 int8s;
    struct robotraconteurlite_array_int8 int8s2;

    int8_t d1 = 10;
    int8_t d2 = 0;

    ROBOTRACONTEURLITE_UNUSED(state);

    for (i = 0; i < sizeof(a1) / sizeof(int8_t); i++)
        a1[i] = (int8_t)i;
    (void)memset(a2, 0, sizeof(a2));
    (void)memset(buf_data, 0, sizeof(buf_data));

    buf.data = buf_data;
    buf.len = sizeof(buf_data);

    buf_vec.buffer_vec = &buf;
    buf_vec.buffer_vec_cnt = 1;

    int8s.data = a1;
    int8s.len = sizeof(a1) / sizeof(int8_t);

    assert_return_code(robotraconteurlite_buffer_vec_copy_from_int8_array(&buf_vec, 32, &int8s, 10, 15), 0);

    assert_true(memcmp((a1 + 10), (buf_data + 32), 15 * sizeof(int8_t)) == 0);

    int8s2.data = a2;
    int8s2.len = sizeof(a2) / sizeof(int8_t);

    assert_return_code(robotraconteurlite_buffer_vec_copy_to_int8_array(&buf_vec, 32, &int8s2, 12, 12), 0);

    assert_true(memcmp(a1 + 10, a2 + 12, 12 * sizeof(int8_t)) == 0);

    robotraconteurlite_buffer_vec_copy_from_int8(&buf_vec, 128, d1);
    assert_true(memcmp(&d1, (buf_data + 128), sizeof(int8_t)) == 0);
    robotraconteurlite_buffer_vec_copy_to_int8(&buf_vec, 32 + sizeof(int8_t), &d2);
    assert_true(d2 == 11);
}

void robotraconteurlite_buffer_uint16_test(void** state)
{
    uint16_t a1[256];
    uint16_t a2[256];
    uint8_t buf_data[256 * sizeof(uint16_t)];
    size_t i = 0;
    struct robotraconteurlite_buffer buf;
    struct robotraconteurlite_buffer_vec buf_vec;
    struct robotraconteurlite_array_uint16 uint16s;
    struct robotraconteurlite_array_uint16 uint16s2;

    uint16_t d1 = 10;
    uint16_t d2 = 0;

    ROBOTRACONTEURLITE_UNUSED(state);

    for (i = 0; i < sizeof(a1) / sizeof(uint16_t); i++)
        a1[i] = (uint16_t)i;
    (void)memset(a2, 0, sizeof(a2));
    (void)memset(buf_data, 0, sizeof(buf_data));

    buf.data = buf_data;
    buf.len = sizeof(buf_data);

    buf_vec.buffer_vec = &buf;
    buf_vec.buffer_vec_cnt = 1;

    uint16s.data = a1;
    uint16s.len = sizeof(a1) / sizeof(uint16_t);

    assert_return_code(robotraconteurlite_buffer_vec_copy_from_uint16_array(&buf_vec, 32, &uint16s, 10, 15), 0);

    assert_true(memcmp((a1 + 10), (buf_data + 32), 15 * sizeof(uint16_t)) == 0);

    uint16s2.data = a2;
    uint16s2.len = sizeof(a2) / sizeof(uint16_t);

    assert_return_code(robotraconteurlite_buffer_vec_copy_to_uint16_array(&buf_vec, 32, &uint16s2, 12, 12), 0);

    assert_true(memcmp(a1 + 10, a2 + 12, 12 * sizeof(uint16_t)) == 0);

    robotraconteurlite_buffer_vec_copy_from_uint16(&buf_vec, 128, d1);
    assert_true(memcmp(&d1, (buf_data + 128), sizeof(uint16_t)) == 0);
    robotraconteurlite_buffer_vec_copy_to_uint16(&buf_vec, 32 + sizeof(uint16_t), &d2);
    assert_true(d2 == 11);
}

void robotraconteurlite_buffer_int16_test(void** state)
{
    int16_t a1[256];
    int16_t a2[256];
    uint8_t buf_data[256 * sizeof(int16_t)];
    size_t i = 0;
    struct robotraconteurlite_buffer buf;
    struct robotraconteurlite_buffer_vec buf_vec;
    struct robotraconteurlite_array_int16 int16s;
    struct robotraconteurlite_array_int16 int16s2;

    int16_t d1 = 10;
    int16_t d2 = 0;

    ROBOTRACONTEURLITE_UNUSED(state);

    for (i = 0; i < sizeof(a1) / sizeof(int16_t); i++)
        a1[i] = (int16_t)i;
    (void)memset(a2, 0, sizeof(a2));
    (void)memset(buf_data, 0, sizeof(buf_data));

    buf.data = buf_data;
    buf.len = sizeof(buf_data);

    buf_vec.buffer_vec = &buf;
    buf_vec.buffer_vec_cnt = 1;

    int16s.data = a1;
    int16s.len = sizeof(a1) / sizeof(int16_t);

    assert_return_code(robotraconteurlite_buffer_vec_copy_from_int16_array(&buf_vec, 32, &int16s, 10, 15), 0);

    assert_true(memcmp((a1 + 10), (buf_data + 32), 15 * sizeof(int16_t)) == 0);

    int16s2.data = a2;
    int16s2.len = sizeof(a2) / sizeof(int16_t);

    assert_return_code(robotraconteurlite_buffer_vec_copy_to_int16_array(&buf_vec, 32, &int16s2, 12, 12), 0);

    assert_true(memcmp(a1 + 10, a2 + 12, 12 * sizeof(int16_t)) == 0);

    robotraconteurlite_buffer_vec_copy_from_int16(&buf_vec, 128, d1);
    assert_true(memcmp(&d1, (buf_data + 128), sizeof(int16_t)) == 0);
    robotraconteurlite_buffer_vec_copy_to_int16(&buf_vec, 32 + sizeof(int16_t), &d2);
    assert_true(d2 == 11);
}

void robotraconteurlite_buffer_uint32_test(void** state)
{
    uint32_t a1[256];
    uint32_t a2[256];
    uint8_t buf_data[256 * sizeof(uint32_t)];
    size_t i = 0;
    struct robotraconteurlite_buffer buf;
    struct robotraconteurlite_buffer_vec buf_vec;
    struct robotraconteurlite_array_uint32 uint32s;
    struct robotraconteurlite_array_uint32 uint32s2;

    uint32_t d1 = 10;
    uint32_t d2 = 0;

    ROBOTRACONTEURLITE_UNUSED(state);

    for (i = 0; i < sizeof(a1) / sizeof(uint32_t); i++)
        a1[i] = (uint32_t)i;
    (void)memset(a2, 0, sizeof(a2));
    (void)memset(buf_data, 0, sizeof(buf_data));

    buf.data = buf_data;
    buf.len = sizeof(buf_data);

    buf_vec.buffer_vec = &buf;
    buf_vec.buffer_vec_cnt = 1;

    uint32s.data = a1;
    uint32s.len = sizeof(a1) / sizeof(uint32_t);

    assert_return_code(robotraconteurlite_buffer_vec_copy_from_uint32_array(&buf_vec, 32, &uint32s, 10, 15), 0);

    assert_true(memcmp((a1 + 10), (buf_data + 32), 15 * sizeof(uint32_t)) == 0);

    uint32s2.data = a2;
    uint32s2.len = sizeof(a2) / sizeof(uint32_t);

    assert_return_code(robotraconteurlite_buffer_vec_copy_to_uint32_array(&buf_vec, 32, &uint32s2, 12, 12), 0);

    assert_true(memcmp(a1 + 10, a2 + 12, 12 * sizeof(uint32_t)) == 0);

    robotraconteurlite_buffer_vec_copy_from_uint32(&buf_vec, 128, d1);
    assert_true(memcmp(&d1, (buf_data + 128), sizeof(uint32_t)) == 0);
    robotraconteurlite_buffer_vec_copy_to_uint32(&buf_vec, 32 + sizeof(uint32_t), &d2);
    assert_true(d2 == 11);
}

void robotraconteurlite_buffer_int32_test(void** state)
{
    int32_t a1[256];
    int32_t a2[256];
    uint8_t buf_data[256 * sizeof(int32_t)];
    size_t i = 0;
    struct robotraconteurlite_buffer buf;
    struct robotraconteurlite_buffer_vec buf_vec;
    struct robotraconteurlite_array_int32 int32s;
    struct robotraconteurlite_array_int32 int32s2;

    int32_t d1 = 10;
    int32_t d2 = 0;

    ROBOTRACONTEURLITE_UNUSED(state);

    for (i = 0; i < sizeof(a1) / sizeof(int32_t); i++)
        a1[i] = (int32_t)i;
    (void)memset(a2, 0, sizeof(a2));
    (void)memset(buf_data, 0, sizeof(buf_data));

    buf.data = buf_data;
    buf.len = sizeof(buf_data);

    buf_vec.buffer_vec = &buf;
    buf_vec.buffer_vec_cnt = 1;

    int32s.data = a1;
    int32s.len = sizeof(a1) / sizeof(int32_t);

    assert_return_code(robotraconteurlite_buffer_vec_copy_from_int32_array(&buf_vec, 32, &int32s, 10, 15), 0);

    assert_true(memcmp((a1 + 10), (buf_data + 32), 15 * sizeof(int32_t)) == 0);

    int32s2.data = a2;
    int32s2.len = sizeof(a2) / sizeof(int32_t);

    assert_return_code(robotraconteurlite_buffer_vec_copy_to_int32_array(&buf_vec, 32, &int32s2, 12, 12), 0);

    assert_true(memcmp(a1 + 10, a2 + 12, 12 * sizeof(int32_t)) == 0);

    robotraconteurlite_buffer_vec_copy_from_int32(&buf_vec, 128, d1);
    assert_true(memcmp(&d1, (buf_data + 128), sizeof(int32_t)) == 0);
    robotraconteurlite_buffer_vec_copy_to_int32(&buf_vec, 32 + sizeof(int32_t), &d2);
    assert_true(d2 == 11);
}

void robotraconteurlite_buffer_uint64_test(void** state)
{
    uint64_t a1[256];
    uint64_t a2[256];
    uint8_t buf_data[256 * sizeof(uint64_t)];
    size_t i = 0;
    struct robotraconteurlite_buffer buf;
    struct robotraconteurlite_buffer_vec buf_vec;
    struct robotraconteurlite_array_uint64 uint64s;
    struct robotraconteurlite_array_uint64 uint64s2;

    uint64_t d1 = 10;
    uint64_t d2 = 0;

    ROBOTRACONTEURLITE_UNUSED(state);

    for (i = 0; i < sizeof(a1) / sizeof(uint64_t); i++)
        a1[i] = (uint64_t)i;
    (void)memset(a2, 0, sizeof(a2));
    (void)memset(buf_data, 0, sizeof(buf_data));

    buf.data = buf_data;
    buf.len = sizeof(buf_data);

    buf_vec.buffer_vec = &buf;
    buf_vec.buffer_vec_cnt = 1;

    uint64s.data = a1;
    uint64s.len = sizeof(a1) / sizeof(uint64_t);

    assert_return_code(robotraconteurlite_buffer_vec_copy_from_uint64_array(&buf_vec, 32, &uint64s, 10, 15), 0);

    assert_true(memcmp((a1 + 10), (buf_data + 32), 15 * sizeof(uint64_t)) == 0);

    uint64s2.data = a2;
    uint64s2.len = sizeof(a2) / sizeof(uint64_t);

    assert_return_code(robotraconteurlite_buffer_vec_copy_to_uint64_array(&buf_vec, 32, &uint64s2, 12, 12), 0);

    assert_true(memcmp(a1 + 10, a2 + 12, 12 * sizeof(uint64_t)) == 0);

    robotraconteurlite_buffer_vec_copy_from_uint64(&buf_vec, 128, d1);
    assert_true(memcmp(&d1, (buf_data + 128), sizeof(uint64_t)) == 0);
    robotraconteurlite_buffer_vec_copy_to_uint64(&buf_vec, 32 + sizeof(uint64_t), &d2);
    assert_true(d2 == 11);
}

void robotraconteurlite_buffer_int64_test(void** state)
{
    int64_t a1[256];
    int64_t a2[256];
    uint8_t buf_data[256 * sizeof(int64_t)];
    size_t i = 0;
    struct robotraconteurlite_buffer buf;
    struct robotraconteurlite_buffer_vec buf_vec;
    struct robotraconteurlite_array_int64 int64s;
    struct robotraconteurlite_array_int64 int64s2;

    int64_t d1 = 10;
    int64_t d2 = 0;

    ROBOTRACONTEURLITE_UNUSED(state);

    for (i = 0; i < sizeof(a1) / sizeof(int64_t); i++)
        a1[i] = (int64_t)i;
    (void)memset(a2, 0, sizeof(a2));
    (void)memset(buf_data, 0, sizeof(buf_data));

    buf.data = buf_data;
    buf.len = sizeof(buf_data);

    buf_vec.buffer_vec = &buf;
    buf_vec.buffer_vec_cnt = 1;

    int64s.data = a1;
    int64s.len = sizeof(a1) / sizeof(int64_t);

    assert_return_code(robotraconteurlite_buffer_vec_copy_from_int64_array(&buf_vec, 32, &int64s, 10, 15), 0);

    assert_true(memcmp((a1 + 10), (buf_data + 32), 15 * sizeof(int64_t)) == 0);

    int64s2.data = a2;
    int64s2.len = sizeof(a2) / sizeof(int64_t);

    assert_return_code(robotraconteurlite_buffer_vec_copy_to_int64_array(&buf_vec, 32, &int64s2, 12, 12), 0);

    assert_true(memcmp(a1 + 10, a2 + 12, 12 * sizeof(int64_t)) == 0);

    robotraconteurlite_buffer_vec_copy_from_int64(&buf_vec, 128, d1);
    assert_true(memcmp(&d1, (buf_data + 128), sizeof(int64_t)) == 0);
    robotraconteurlite_buffer_vec_copy_to_int64(&buf_vec, 32 + sizeof(int64_t), &d2);
    assert_true(d2 == 11);
}

#define robotraconteurlite_array_float robotraconteurlite_array_single
#define robotraconteurlite_buffer_vec_copy_from_float_array robotraconteurlite_buffer_vec_copy_from_single_array
#define robotraconteurlite_buffer_vec_copy_to_float_array robotraconteurlite_buffer_vec_copy_to_single_array
#define robotraconteurlite_buffer_vec_copy_from_float robotraconteurlite_buffer_vec_copy_from_single
#define robotraconteurlite_buffer_vec_copy_to_float robotraconteurlite_buffer_vec_copy_to_single

static int cmp_float(const float* a, const float* b, size_t len)
{
    size_t i = 0;
    for (i = 0; i < len; i++)
    {
        if (a[i] != b[i])
        {
            return 1;
        }
    }
    return 0;
}

void robotraconteurlite_buffer_float_test(void** state)
{
    float a1[256];
    float a2[256];
    uint8_t buf_data[256 * sizeof(float)];
    size_t i = 0;
    struct robotraconteurlite_buffer buf;
    struct robotraconteurlite_buffer_vec buf_vec;
    struct robotraconteurlite_array_float floats;
    struct robotraconteurlite_array_float floats2;

    float d1 = 10.23;
    float d2 = 0.0;

    ROBOTRACONTEURLITE_UNUSED(state);

    for (i = 0; i < sizeof(a1) / sizeof(float); i++)
        a1[i] = (float)i;
    (void)memset(a2, 0, sizeof(a2));
    (void)memset(buf_data, 0, sizeof(buf_data));

    buf.data = buf_data;
    buf.len = sizeof(buf_data);

    buf_vec.buffer_vec = &buf;
    buf_vec.buffer_vec_cnt = 1;

    floats.data = a1;
    floats.len = sizeof(a1) / sizeof(float);

    assert_return_code(robotraconteurlite_buffer_vec_copy_from_float_array(&buf_vec, 32, &floats, 10, 15), 0);

    assert_true(cmp_float(a1 + 10, (float*)(buf_data + 32), 15) == 0);

    floats2.data = a2;
    floats2.len = sizeof(a2) / sizeof(float);

    assert_return_code(robotraconteurlite_buffer_vec_copy_to_float_array(&buf_vec, 32, &floats2, 12, 12), 0);

    assert_true(cmp_float(a1 + 10, a2 + 12, 12) == 0);

    robotraconteurlite_buffer_vec_copy_from_float(&buf_vec, 128, d1);
    assert_true(cmp_float(&d1, (float*)(buf_data + 128), 1) == 0);
    robotraconteurlite_buffer_vec_copy_to_float(&buf_vec, 32 + sizeof(float), &d2);
    assert_true(d2 == 11.0);
}

static int cmp_double(const double* a, const double* b, size_t len)
{
    size_t i = 0;
    for (i = 0; i < len; i++)
    {
        if (a[i] != b[i])
        {
            return 1;
        }
    }
    return 0;
}

void robotraconteurlite_buffer_double_test(void** state)
{
    double a1[256];
    double a2[256];
    uint8_t buf_data[256 * sizeof(double)];
    size_t i = 0;
    struct robotraconteurlite_buffer buf;
    struct robotraconteurlite_buffer_vec buf_vec;
    struct robotraconteurlite_array_double doubles;
    struct robotraconteurlite_array_double doubles2;

    double d1 = 10.23;
    double d2 = 0.0;

    ROBOTRACONTEURLITE_UNUSED(state);

    for (i = 0; i < sizeof(a1) / sizeof(double); i++)
        a1[i] = (double)i;
    (void)memset(a2, 0, sizeof(a2));
    (void)memset(buf_data, 0, sizeof(buf_data));

    buf.data = buf_data;
    buf.len = sizeof(buf_data);

    buf_vec.buffer_vec = &buf;
    buf_vec.buffer_vec_cnt = 1;

    doubles.data = a1;
    doubles.len = sizeof(a1) / sizeof(double);

    assert_return_code(robotraconteurlite_buffer_vec_copy_from_double_array(&buf_vec, 32, &doubles, 10, 15), 0);

    assert_true(cmp_double(a1 + 10, (double*)(buf_data + 32), 15) == 0);

    doubles2.data = a2;
    doubles2.len = sizeof(a2) / sizeof(double);

    assert_return_code(robotraconteurlite_buffer_vec_copy_to_double_array(&buf_vec, 32, &doubles2, 12, 12), 0);

    assert_true(cmp_double(a1 + 10, a2 + 12, 12) == 0);

    robotraconteurlite_buffer_vec_copy_from_double(&buf_vec, 128, d1);
    assert_true(cmp_double(&d1, (double*)(buf_data + 128), 1) == 0);
    robotraconteurlite_buffer_vec_copy_to_double(&buf_vec, 32 + sizeof(double), &d2);
    assert_true(d2 == 11.0);
}

static int cmp_csingle(const struct robotraconteurlite_csingle* a, const struct robotraconteurlite_csingle* b,
                       size_t len)
{
    size_t i = 0;
    for (i = 0; i < len; i++)
    {
        if (fabs((double)(a[i].real - b[i].real)) > 1e-4 || fabs((double)(a[i].imag - b[i].imag)) > 1e-4)
        {
            return 1;
        }
    }
    return 0;
}

void robotraconteurlite_buffer_csingle_test(void** state)
{
    struct robotraconteurlite_csingle a1[256];
    struct robotraconteurlite_csingle a2[256];
    uint8_t buf_data[256 * sizeof(struct robotraconteurlite_csingle)];
    size_t i = 0;
    struct robotraconteurlite_buffer buf;
    struct robotraconteurlite_buffer_vec buf_vec;
    struct robotraconteurlite_array_csingle csingles;
    struct robotraconteurlite_array_csingle csingles2;

    struct robotraconteurlite_csingle d1;
    struct robotraconteurlite_csingle d2;
    memset(&d1, 0, sizeof(d1));
    memset(&d2, 0, sizeof(d2));

    d1.real = 10.23;
    d1.imag = 11.34;

    ROBOTRACONTEURLITE_UNUSED(state);

    for (i = 0; i < sizeof(a1) / sizeof(struct robotraconteurlite_csingle); i++)
    {
        a1[i].real = (float)i;
        a1[i].imag = (float)(i + 1);
    }
    (void)memset(a2, 0, sizeof(a2));
    (void)memset(buf_data, 0, sizeof(buf_data));

    buf.data = buf_data;
    buf.len = sizeof(buf_data);

    buf_vec.buffer_vec = &buf;
    buf_vec.buffer_vec_cnt = 1;

    csingles.data = a1;
    csingles.len = sizeof(a1) / sizeof(struct robotraconteurlite_csingle);

    assert_return_code(robotraconteurlite_buffer_vec_copy_from_csingle_array(&buf_vec, 32, &csingles, 10, 15), 0);

    assert_true(cmp_csingle(a1 + 10, (struct robotraconteurlite_csingle*)(buf_data + 32), 15) == 0);

    csingles2.data = a2;
    csingles2.len = sizeof(a2) / sizeof(struct robotraconteurlite_csingle);

    assert_return_code(robotraconteurlite_buffer_vec_copy_to_csingle_array(&buf_vec, 32, &csingles2, 12, 12), 0);

    assert_true(cmp_csingle(a1 + 10, a2 + 12, 12) == 0);

    robotraconteurlite_buffer_vec_copy_from_csingle(&buf_vec, 128, d1);
    assert_true(cmp_csingle(&d1, (struct robotraconteurlite_csingle*)(buf_data + 128), 1) == 0);
    robotraconteurlite_buffer_vec_copy_to_csingle(&buf_vec, 32 + sizeof(struct robotraconteurlite_csingle), &d2);
    assert_true(d2.real == 11.0);
    assert_true(d2.imag == 12.0);
}

static int cmp_cdouble(const struct robotraconteurlite_cdouble* a, const struct robotraconteurlite_cdouble* b,
                       size_t len)
{
    size_t i = 0;
    for (i = 0; i < len; i++)
    {
        if (fabs((double)(a[i].real - b[i].real)) > 1e-4 || fabs((double)(a[i].imag - b[i].imag)) > 1e-4)
        {
            return 1;
        }
    }
    return 0;
}

void robotraconteurlite_buffer_cdouble_test(void** state)
{
    struct robotraconteurlite_cdouble a1[256];
    struct robotraconteurlite_cdouble a2[256];
    uint8_t buf_data[256 * sizeof(struct robotraconteurlite_cdouble)];
    size_t i = 0;
    struct robotraconteurlite_buffer buf;
    struct robotraconteurlite_buffer_vec buf_vec;
    struct robotraconteurlite_array_cdouble cdoubles;
    struct robotraconteurlite_array_cdouble cdoubles2;

    struct robotraconteurlite_cdouble d1;
    struct robotraconteurlite_cdouble d2;
    memset(&d1, 0, sizeof(d1));
    memset(&d2, 0, sizeof(d2));

    d1.real = 10.23;
    d1.imag = 11.34;

    ROBOTRACONTEURLITE_UNUSED(state);

    for (i = 0; i < sizeof(a1) / sizeof(struct robotraconteurlite_cdouble); i++)
    {
        a1[i].real = (float)i;
        a1[i].imag = (float)(i + 1);
    }
    (void)memset(a2, 0, sizeof(a2));
    (void)memset(buf_data, 0, sizeof(buf_data));

    buf.data = buf_data;
    buf.len = sizeof(buf_data);

    buf_vec.buffer_vec = &buf;
    buf_vec.buffer_vec_cnt = 1;

    cdoubles.data = a1;
    cdoubles.len = sizeof(a1) / sizeof(struct robotraconteurlite_cdouble);

    assert_return_code(robotraconteurlite_buffer_vec_copy_from_cdouble_array(&buf_vec, 32, &cdoubles, 10, 15), 0);

    assert_true(cmp_cdouble(a1 + 10, (struct robotraconteurlite_cdouble*)(buf_data + 32), 15) == 0);

    cdoubles2.data = a2;
    cdoubles2.len = sizeof(a2) / sizeof(struct robotraconteurlite_cdouble);

    assert_return_code(robotraconteurlite_buffer_vec_copy_to_cdouble_array(&buf_vec, 32, &cdoubles2, 12, 12), 0);

    assert_true(cmp_cdouble(a1 + 10, a2 + 12, 12) == 0);

    robotraconteurlite_buffer_vec_copy_from_cdouble(&buf_vec, 128, d1);
    assert_true(cmp_cdouble(&d1, (struct robotraconteurlite_cdouble*)(buf_data + 128), 1) == 0);
    robotraconteurlite_buffer_vec_copy_to_cdouble(&buf_vec, 32 + sizeof(struct robotraconteurlite_cdouble), &d2);
    assert_true(d2.real == 11.0);
    assert_true(d2.imag == 12.0);
}

int main(void)
{
    const struct CMUnitTest tests[] = {cmocka_unit_test(robotraconteurlite_buffer_uint8_test),
                                       cmocka_unit_test(robotraconteurlite_buffer_int8_test),
                                       cmocka_unit_test(robotraconteurlite_buffer_uint16_test),
                                       cmocka_unit_test(robotraconteurlite_buffer_int16_test),
                                       cmocka_unit_test(robotraconteurlite_buffer_uint32_test),
                                       cmocka_unit_test(robotraconteurlite_buffer_int32_test),
                                       cmocka_unit_test(robotraconteurlite_buffer_uint64_test),
                                       cmocka_unit_test(robotraconteurlite_buffer_int64_test),
                                       cmocka_unit_test(robotraconteurlite_buffer_float_test),
                                       cmocka_unit_test(robotraconteurlite_buffer_double_test),
                                       cmocka_unit_test(robotraconteurlite_buffer_csingle_test),
                                       cmocka_unit_test(robotraconteurlite_buffer_cdouble_test)};
    return cmocka_run_group_tests(tests, NULL, NULL);
}
