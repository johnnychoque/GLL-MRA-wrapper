#ifndef _config_h_
#define _config_h_

#ifdef __FreeBSD__
#include <sys/endian.h>
#else
#include <endian.h>
#endif

#if __BYTE_ORDER == __LITTLE_ENDIAN
#define LITTLEENDIAN
#elif __BYTE_ORDER == __BIG_ENDIAN
#define BIGENDIAN
#else
#error "No byte order defined!!! Check"
#endif

typedef unsigned char u_int8_t;
typedef unsigned short u_int16_t;
typedef unsigned int u_int32_t;
typedef unsigned long long int u_int64_t;

typedef char s_int8_t;
typedef short s_int16_t;
typedef int s_int32_t;
typedef long long int s_int64_t;

#define MAX_UINT_8 255
#define MAX_UINT_16 65535
#define MAX_UINT_32 4294967295


#endif // _config_h_
