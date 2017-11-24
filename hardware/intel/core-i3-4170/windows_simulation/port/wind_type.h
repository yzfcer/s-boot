/*********************************************************************************
  *Copyright(C),2016-2018,yzfcer@163.com
  *FileName:  
  *Author:      Zhou Jiangcun
  *Version:     1.0
  *Date:        2016/10/08
  *Description:  
  *Others:  
  *History:  
     1.Date:
       Author:
       Modification:
**********************************************************************************/
#ifndef BOOT_TYPE_H__
#define BOOT_TYPE_H__
#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned long long w_uint64_t;
typedef unsigned int w_uint32_t;
typedef unsigned short w_uint16_t;
typedef unsigned char w_uint8_t;

typedef long long w_int64_t;
typedef int w_int32_t;
typedef short w_int16_t;
typedef char w_int8_t;

typedef float w_fp32_t;
typedef double w_fp64_t;
typedef enum 
{
    B_FALSE=0,
    B_TRUE,
}bool_t;
#ifndef NULL
#define NULL (void*)0
#endif

#ifdef __cplusplus
}
#endif

#endif
