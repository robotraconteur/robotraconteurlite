
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

void robotraconteurlite_arraytest_buffer(void** state)
{
    uint8_t a[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    uint8_t b[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    uint8_t c[] = {0, 0, 5, 6, 7, 8, 0, 0, 0, 0};
    uint8_t d[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    struct robotraconteurlite_buffer bufa;
    struct robotraconteurlite_buffer bufb;
    struct robotraconteurlite_buffer bufc;
    struct robotraconteurlite_buffer bufd;
    size_t bufa_len = 0;

    ROBOTRACONTEURLITE_UNUSED(state);

    bufa.data = a;
    bufa.len = sizeof(a);

    bufb.data = b;
    bufb.len = sizeof(b);

    bufc.data = c;
    bufc.len = sizeof(c);

    bufd.data = d;
    bufd.len = sizeof(d);

    assert_return_code(robotraconteurlite_buffer_len(&bufa, &bufa_len), 0);
    assert_true(bufa_len == 10);

    assert_return_code(robotraconteurlite_buffer_copy(&bufa, 5, &bufb, 2, 4), 0);

    assert_true(memcmp(b, c, sizeof(b)) == 0);

    assert_return_code(robotraconteurlite_buffer_copy_ex(&bufa, 5, 1, 4, &bufd, 1, 2, 2), 0);

    assert_true(memcmp(d, c, sizeof(d)) == 0);
}

void robotraconteurlite_arraytest_buffer_vec_copy_vec(void** state)
{
    uint8_t a1[100];
    uint8_t a2[100];
    uint8_t* a[2];
    uint8_t i = 0;
    struct robotraconteurlite_buffer bufa[8];
    struct robotraconteurlite_buffer bufb[8];
    struct robotraconteurlite_buffer_vec bufa_vec;
    struct robotraconteurlite_buffer_vec bufb_vec;
    struct robotraconteurlite_buffer_vec* buf_vecs[2];
    size_t iter = 0;

    ROBOTRACONTEURLITE_UNUSED(state);

    a[0] = &a1[0];
    a[1] = &a2[0];

    srand((unsigned int)time(0));
    assert_true(sizeof(a1) == sizeof(a2));

    for (i = 0; i < (uint8_t)sizeof(a1); i++)
        a1[i] = i;

    bufa_vec.buffer_vec = bufa;
    bufa_vec.buffer_vec_cnt = 8;

    bufb_vec.buffer_vec = bufb;
    bufb_vec.buffer_vec_cnt = 8;

    buf_vecs[0] = &bufa_vec;
    buf_vecs[1] = &bufb_vec;

    for (iter = 0; iter < 10000; iter++)
    {
        size_t buf_i = 0;
        size_t len_bufa = 0;
        size_t len_bufb = 0;
        size_t bufa_vec_start = 0;
        size_t bufb_vec_start = 0;
        size_t count = 0;

        memset(a2, 0, sizeof(a2));

        for (buf_i = 0; buf_i < 2; buf_i++)
        {
            struct robotraconteurlite_buffer_vec* buf_vec = buf_vecs[buf_i];
            size_t pos = 0;
            size_t i = 0;
            buf_vec->buffer_vec_cnt = 0;

            for (i = 0; i < 8; i++)
            {
                size_t c = 0;
                if (i == 7)
                {
                    c = sizeof(a1) - pos;
                }
                else
                {
                    c = rand() % (sizeof(a1) - pos);
                    if (i < 4)
                    {
                        c = c / 2;
                    }
                }
                buf_vec->buffer_vec[i].data = a[buf_i] + pos;
                buf_vec->buffer_vec[i].len = c;
                buf_vec->buffer_vec_cnt++;
                pos += c;
                if (pos >= sizeof(a1))
                    break;
            }
        }

        assert_return_code(robotraconteurlite_buffer_vec_len(&bufa_vec, &len_bufa), 0);
        assert_return_code(robotraconteurlite_buffer_vec_len(&bufb_vec, &len_bufb), 0);
        assert_true(len_bufa == sizeof(a1));
        assert_true(len_bufb == sizeof(a2));

        bufa_vec_start = rand() % sizeof(a1);
        bufb_vec_start = rand() % sizeof(a2);

        count = rand() % (sizeof(a1) - (bufa_vec_start > bufb_vec_start ? bufa_vec_start : bufb_vec_start));

        robotraconteurlite_buffer_vec_copy_vec(&bufa_vec, bufa_vec_start, &bufb_vec, bufb_vec_start, count);

        assert_true(memcmp(a1 + bufa_vec_start, a2 + bufb_vec_start, count) == 0);
    }
}

void robotraconteurlite_arraytest_buffer_vec_copy_vec_ex(void** state)
{
    uint8_t a1[255];
    uint8_t a2[255];
    uint8_t i = 0;
    uint8_t* a[2];
    struct robotraconteurlite_buffer bufa[8];
    struct robotraconteurlite_buffer bufb[8];
    struct robotraconteurlite_buffer_vec bufa_vec;
    struct robotraconteurlite_buffer_vec bufb_vec;
    struct robotraconteurlite_buffer_vec* buf_vecs[2];
    size_t bufa_vec_el = 0;
    size_t bufb_vec_el = 0;
    size_t* buf_vec_els[2];
    size_t iter = 0;

    ROBOTRACONTEURLITE_UNUSED(state);

    assert_true(sizeof(a1) == sizeof(a2));

    srand((unsigned int)time(0));

    for (i = 0; i < (uint8_t)sizeof(a1); i++)
        a1[i] = i;

    a[0] = &a1[0];
    a[1] = &a2[0];

    bufa_vec.buffer_vec = bufa;
    bufa_vec.buffer_vec_cnt = 8;

    bufb_vec.buffer_vec = bufb;
    bufb_vec.buffer_vec_cnt = 8;

    buf_vecs[0] = &bufa_vec;
    buf_vecs[1] = &bufb_vec;

    bufa_vec_el = 1 << ((rand() % 3) + 1);
    bufb_vec_el = 1 << ((rand() % 3) + 1);
    buf_vec_els[0] = &bufa_vec_el;
    buf_vec_els[1] = &bufb_vec_el;

    for (iter = 0; iter < 100000; iter++)
    {
        size_t buf_i = 0;
        size_t len_bufa = 0;
        size_t len_bufb = 0;
        size_t bufa_vec_start = 0;
        size_t bufb_vec_start = 0;
        size_t bufa_vec_count = 0;
        size_t bufb_vec_count = 0;
        size_t bufa_vec_count_b = 0;
        size_t bufb_vec_count_b = 0;

        size_t count_bytes = 0;
        size_t big_el = 0;

        memset(a2, 0, sizeof(a2));

        for (buf_i = 0; buf_i < 2; buf_i++)
        {
            struct robotraconteurlite_buffer_vec* buf_vec = buf_vecs[buf_i];
            size_t pos = 0;
            size_t i = 0;
            buf_vec->buffer_vec_cnt = 0;

            for (i = 0; i < 8; i++)
            {
                size_t c = 0;
                if (i == 7)
                {
                    c = (sizeof(a1) / *buf_vec_els[buf_i]) - pos;
                }
                else
                {
                    c = rand() % ((sizeof(a1) / *buf_vec_els[buf_i]) - pos);
                    if (i < 4)
                    {
                        c = c / 2;
                    }
                }
                buf_vec->buffer_vec[i].data = a[buf_i] + pos * (*buf_vec_els[buf_i]);
                buf_vec->buffer_vec[i].len = c;
                buf_vec->buffer_vec_cnt++;
                pos += c;
                if (pos >= (sizeof(a1) / *buf_vec_els[buf_i]))
                    break;
            }
        }

        assert_return_code(robotraconteurlite_buffer_vec_len(&bufa_vec, &len_bufa), 0);
        assert_return_code(robotraconteurlite_buffer_vec_len(&bufb_vec, &len_bufb), 0);
        assert_true(len_bufa == (sizeof(a1) / bufa_vec_el));
        assert_true(len_bufb == (sizeof(a2) / bufb_vec_el));

        bufa_vec_start = rand() % (sizeof(a1) / bufa_vec_el);
        bufb_vec_start = rand() % (sizeof(a2) / bufb_vec_el);
        bufa_vec_count = ((sizeof(a1) / bufa_vec_el) - bufa_vec_start);
        bufb_vec_count = ((sizeof(a2) / bufb_vec_el) - bufb_vec_start);
        bufa_vec_count_b = bufa_vec_count * bufa_vec_el;
        bufb_vec_count_b = bufb_vec_count * bufb_vec_el;

        count_bytes = bufa_vec_count_b < bufb_vec_count_b ? bufa_vec_count_b : bufb_vec_count_b;
        big_el = bufa_vec_el > bufb_vec_el ? bufa_vec_el : bufb_vec_el;

        count_bytes = (rand() % count_bytes);
        count_bytes = count_bytes - (count_bytes % big_el);

        assert_return_code(robotraconteurlite_buffer_vec_copy_vec_ex(
                               &bufa_vec, bufa_vec_start, bufa_vec_el, count_bytes / bufa_vec_el, &bufb_vec,
                               bufb_vec_start, bufb_vec_el, count_bytes / bufb_vec_el),
                           0);

        assert_true(memcmp(a1 + (bufa_vec_start * bufa_vec_el), a2 + (bufb_vec_start * bufb_vec_el), count_bytes) == 0);
    }
}

void robotraconteurlite_buffer_copy_to_double_test(void** state)
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
    memset(a2, 0, sizeof(a2));
    memset(buf_data, 0, sizeof(buf_data));

    buf.data = buf_data;
    buf.len = sizeof(buf_data);

    buf_vec.buffer_vec = &buf;
    buf_vec.buffer_vec_cnt = 1;

    doubles.data = a1;
    doubles.len = sizeof(a1) / sizeof(double);

    assert_return_code(robotraconteurlite_buffer_vec_copy_from_double_array(&buf_vec, 32, &doubles, 10, 15), 0);

    assert_true(memcmp(a1 + 10, buf_data + 32, 15 * sizeof(double)) == 0);

    doubles2.data = a2;
    doubles2.len = sizeof(a2) / sizeof(double);

    assert_return_code(robotraconteurlite_buffer_vec_copy_to_double_array(&buf_vec, 32, &doubles2, 12, 12), 0);

    assert_true(memcmp(a1 + 10, a2 + 12, 12 * sizeof(double)) == 0);

    robotraconteurlite_buffer_vec_copy_from_double(&buf_vec, 128, d1);
    assert_true(memcmp(&d1, buf_data + 128, sizeof(double)) == 0);
    robotraconteurlite_buffer_vec_copy_to_double(&buf_vec, 40, &d2);
    assert_true(d2 == 11.0);
}

void robotraconteurlite_string_test(void** state)
{
    char str1_data[] = "01234";
    struct robotraconteurlite_string str1;
    char str2_data[] = "012345";
    struct robotraconteurlite_string str2;
    char str3_data[] = "01334";
    struct robotraconteurlite_string str3;
    char str4_data[] = "01234";
    struct robotraconteurlite_string str4;

    ROBOTRACONTEURLITE_UNUSED(state);

    str1.data = str1_data;
    str1.len = strlen(str1_data);

    str2.data = str2_data;
    str2.len = strlen(str2_data);

    str3.data = str3_data;
    str3.len = strlen(str3_data);

    str4.data = str4_data;
    str4.len = strlen(str4_data);

    assert_true(robotraconteurlite_string_cmp(&str1, &str4) == 0);
    assert_true(robotraconteurlite_string_cmp(&str1, &str2) == -1);
    assert_true(robotraconteurlite_string_cmp(&str2, &str1) == 1);
    assert_true(robotraconteurlite_string_cmp(&str1, &str3) == -1);

    assert_true(robotraconteurlite_string_hash(&str1) == 0x30070005);
    assert_true(robotraconteurlite_string_hash(&str2) == 0x30010006);
}

int main(void)
{
    const struct CMUnitTest tests[] = {cmocka_unit_test(robotraconteurlite_arraytest_buffer),
                                       cmocka_unit_test(robotraconteurlite_arraytest_buffer_vec_copy_vec),
                                       cmocka_unit_test(robotraconteurlite_arraytest_buffer_vec_copy_vec_ex),
                                       cmocka_unit_test(robotraconteurlite_buffer_copy_to_double_test),
                                       cmocka_unit_test(robotraconteurlite_string_test)};
    return cmocka_run_group_tests(tests, NULL, NULL);
}
