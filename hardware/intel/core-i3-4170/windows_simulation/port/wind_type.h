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
#ifndef WIND_TYPES_H__
#define WIND_TYPES_H__
#ifdef __cplusplus
extern "C" {
#endif

#ifndef w_uint8_t
typedef unsigned char w_uint8_t;
#endif

#ifndef w_uint16_t
typedef unsigned int w_uint16_t;
#endif

#ifndef w_uint32_t
typedef unsigned long w_uint32_t; 
#endif

#ifndef w_uint64_t
typedef unsigned long long w_uint64_t; 
#endif

#ifndef w_int8_t
typedef char w_int8_t; 
#endif

#ifndef w_int16_t
typedef int w_int16_t; 
#endif

#ifndef w_int32_t
typedef long w_int32_t; 
#endif

#ifndef w_int64_t
typedef long long w_int64_t; 
#endif


#ifndef w_fp32_t
typedef float w_fp32_t;
#endif

#ifndef w_fp64_t
typedef float w_fp64_t;
#endif

#ifndef __INLINE__
#define __INLINE__ 
#endif


#ifndef NULL
#define NULL ((void *)0)
#endif

#ifndef w_bool_t
typedef enum __w_bool_t
{
    B_FALSE = 0,
    B_TRUE = 1
} w_bool_t;
#endif


#ifndef w_err_t
typedef w_int16_t w_err_t;
#endif


#ifndef w_stack_t  //堆栈宽度的定义
typedef w_uint32_t w_stack_t;//,*w_pstack_t;
typedef w_uint32_t *w_pstack_t;
#endif



typedef enum __lock_type_e
{
    LOCK_TYPE_NONE = 0,
    LOCK_TYPE_AREA = 1,
    LOCK_TYPE_GLOBAL = 2,
    LOCK_TYPE_BUTT
}lock_type_e;






#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  

