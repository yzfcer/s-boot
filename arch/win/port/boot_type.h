#ifndef BOOT_TYPE_H__
#define BOOT_TYPE_H__
#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned long long uint64_t;
typedef unsigned int uint32_t;
typedef unsigned short uint32_t16_t;
typedef unsigned char uint8_t;

typedef long long int64_t;
typedef int int32_t;
typedef short int16_t;
typedef char int8_t;

typedef float fp32_t;
typedef double fp64_t;
typedef enum 
{
    FALSE,
    TRUE,
}bool_t;
#ifdef __cplusplus
}
#endif

#endif
