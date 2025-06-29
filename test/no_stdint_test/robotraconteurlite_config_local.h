#ifndef ROBOTRACONTEURLITE_CONFIG_LOCAL_H
#define ROBOTRACONTEURLITE_CONFIG_LOCAL_H

#include <limits.h>
#include <stddef.h>

typedef char robotraconteurlite_i8;
typedef unsigned char robotraconteurlite_u8;
typedef short robotraconteurlite_i16;
typedef unsigned short robotraconteurlite_u16;
#ifdef _WIN32
typedef long robotraconteurlite_i32;
typedef unsigned long robotraconteurlite_u32;
typedef long long robotraconteurlite_i64;
typedef unsigned long long robotraconteurlite_u64;
#else
typedef int robotraconteurlite_i32;
typedef unsigned int robotraconteurlite_u32;
typedef long robotraconteurlite_i64;
typedef unsigned long robotraconteurlite_u64;
#endif
typedef float robotraconteurlite_single;
typedef double robotraconteurlite_double;
typedef size_t robotraconteurlite_size_t;
typedef unsigned char robotraconteurlite_byte;
#define ROBOTRACONTEURLITE_INT8_MIN (-128)
#define ROBOTRACONTEURLITE_INT8_MAX 127
#define ROBOTRACONTEURLITE_UINT8_MIN 0
#define ROBOTRACONTEURLITE_UINT8_MAX 255
#define ROBOTRACONTEURLITE_INT16_MIN (-32768)
#define ROBOTRACONTEURLITE_INT16_MAX 32767
#define ROBOTRACONTEURLITE_UINT16_MIN 0
#define ROBOTRACONTEURLITE_UINT16_MAX 65535
#define ROBOTRACONTEURLITE_INT32_MIN (-2147483648L)
#define ROBOTRACONTEURLITE_INT32_MAX 2147483647L
#define ROBOTRACONTEURLITE_UINT32_MIN 0
#define ROBOTRACONTEURLITE_UINT32_MAX 4294967295UL
#define ROBOTRACONTEURLITE_INT64_MIN (-9223372036854775807LL - 1)
#define ROBOTRACONTEURLITE_INT64_MAX 9223372036854775807LL
#define ROBOTRACONTEURLITE_UINT64_MIN 0
#define ROBOTRACONTEURLITE_UINT64_MAX 18446744073709551615ULL
#define ROBOTRACONTEURLITE_SINGLE_MIN FLT_MIN
#define ROBOTRACONTEURLITE_SINGLE_MAX FLT_MAX
#define ROBOTRACONTEURLITE_DOUBLE_MIN DBL_MIN
#define ROBOTRACONTEURLITE_DOUBLE_MAX DBL_MAX
#define ROBOTRACONTEURLITE_SIZE_T_MAX 4294967295UL
#define ROBOTRACONTEURLITE_SIZE_T_MIN 0

#endif /* ROBOTRACONTEURLITE_CONFIG_LOCAL_H */