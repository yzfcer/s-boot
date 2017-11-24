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
#ifndef BOOT_DEBUG_H__
#define BOOT_DEBUG_H__
#include "wind_type.h"
#include "mem_map.h"
#include "boot_hw_if.h"
#include "boot_config.h"

#ifdef __cplusplus
extern "C" {
#endif


#if BOOT_DEBUG_ENABLE
#define sys_debug(fmt,...) wind_printf("%s"fmt"  [%s,%d]\r\n","DEBUG: ",##__VA_ARGS__,__FUNCTION__,__LINE__)
#define sys_notice(fmt,...) wind_printf("%s"fmt"\r\n","NOTICE: ",##__VA_ARGS__)
#define sys_warn(fmt,...) wind_printf("\r\n%s"fmt" [%s,%d]\r\n\r\n","------WARNING------\r\n",##__VA_ARGS__,__FUNCTION__,__LINE__)
#define sys_error(fmt,...) wind_printf("\r\n%s"fmt" [%s,%d]\r\n\r\n","------ERROR------\r\n",##__VA_ARGS__,__FUNCTION__,__LINE__)
#else
#define sys_debug(fmt,...) //wind_printf("%s"fmt"\r\n","DEBUG:",__VA_ARGS__)
#define sys_notice(fmt,...) wind_printf("%s"fmt"\r\n","NOTICE: ",##__VA_ARGS__)
#define sys_warn(fmt,...) wind_printf("\r\n%s"fmt" \r\n\r\n","------WARNING------\r\n",##__VA_ARGS__)
#define sys_error(fmt,...) wind_printf("\r\n%s"fmt" \r\n\r\n","------ERROR------\r\n",##__VA_ARGS__)
#endif
#ifndef NULL 
#define NULL (void*)0
#endif


#ifdef __cplusplus
}
#endif

#endif
