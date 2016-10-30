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
#include "crc.h"
#include "sys_debug.h"
#ifdef __cplusplus
extern "C" {
#endif

uint32_t POLYNOMIAL = 0xEDB88320;
int32_t have_table = 0;
uint32_t table[256];

void make_table()
{
    int32_t i, j ;
    have_table = 1 ;
    for (i = 0;i < 256;i++)
        for (j = 0,table[i] = i;j < 8;j++)
            table[i] = (table[i]>>1)^((table[i]&1)?POLYNOMIAL:0);
}


uint32_t calc_crc32(uint8_t *data, int32_t len,uint32_t crc)
{
    int32_t i;
    if(!have_table) 
        make_table();
    crc = ~crc;
    for (i = 0;i < len;i++)
        crc = (crc >> 8) ^ table[(crc ^ data[i]) & 0xff];
    return ~crc;
}

#ifdef __cplusplus
}
#endif
