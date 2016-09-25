/***************************************************************************************
** 文件名：
** 文件信息：
** 创建时间：2016.09.25
** 创建人：清风海岸
**文件版本 1.0
** 修改记录：
***************************************************************************************/
#ifndef BOOT_DEBUG_H__
#define BOOT_DEBUG_H__
#include "boot_type.h"
#include "mem_map.h"
#include "boot_hw_if.h"
//#include "hw_device.h"
//#include "port.h"

#ifdef __cplusplus
extern "C" {
#endif


#define BOOT_DEBUG_EN 0
#if BOOT_DEBUG_EN
#define boot_todo(fmt,...) boot_printf("%s"fmt" [%s,%d]\r\n","TODO:***--------  ",##__VA_ARGS__,__FUNCTION__,__LINE__)
#define boot_debug(fmt,...) boot_printf("%s"fmt"  [%s,%d]\r\n","DEBUG: ",##__VA_ARGS__,__FUNCTION__,__LINE__)
#define boot_notice(fmt,...) boot_printf("%s"fmt"\r\n","NOTICE: ",##__VA_ARGS__)
#define boot_warn(fmt,...) boot_printf("\r\n%s"fmt" [%s,%d]\r\n\r\n","------WARNING------\r\n",##__VA_ARGS__,__FUNCTION__,__LINE__)
#define boot_error(fmt,...) boot_printf("\r\n%s"fmt" [%s,%d]\r\n\r\n","------ERROR------\r\n",##__VA_ARGS__,__FUNCTION__,__LINE__)
#else
#define boot_todo(fmt,...) //boot_printf("%s"fmt" [%s,%d]\r\n","TODO:***--------  ",__VA_ARGS__,__FUNCTION__,__LINE__)
#define boot_debug(fmt,...) //boot_printf("%s"fmt"\r\n","DEBUG:",__VA_ARGS__)
#define boot_notice(fmt,...) boot_printf("%s"fmt"\r\n","NOTICE: ",##__VA_ARGS__)
#define boot_warn(fmt,...) boot_printf("\r\n%s"fmt" \r\n\r\n","------WARNING------\r\n",##__VA_ARGS__)
#define boot_error(fmt,...) boot_printf("\r\n%s"fmt" \r\n\r\n","------ERROR------\r\n",##__VA_ARGS__)
#endif
#ifndef NULL 
#define NULL (void*)0
#endif


#ifdef __cplusplus
}
#endif

#endif
