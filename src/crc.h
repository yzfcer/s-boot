/***************************************************************************************
** 文件名：
** 文件信息：
** 创建时间：2016.09.25
** 创建人：清风海岸
**文件版本 1.0
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

