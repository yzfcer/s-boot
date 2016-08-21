/***************************************************************************************
** 文件名：
** 文件信息：
** 创建时间：2015.10.15
** 创建人：周江村
**文件版本1.0
** 修改记录：
***************************************************************************************/
#ifndef CRC_H__
#define CRC_H__
#include "boot_type.h"
#ifdef __cplusplus
extern "C" {
#endif
uint32_t calc_crc32(char *buff, int32_t len,uint32_t crc);

#ifdef __cplusplus
}
#endif

#endif

