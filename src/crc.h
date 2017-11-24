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
#ifndef CRC_H__
#define CRC_H__
#include "wind_type.h"
#ifdef __cplusplus
extern "C" {
#endif
w_uint32_t calc_crc32(w_uint8_t *data, w_int32_t len,w_uint32_t crc);

#ifdef __cplusplus
}
#endif

#endif

