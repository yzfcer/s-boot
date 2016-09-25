/***************************************************************************************
** 文件名：
** 文件信息：
** 创建时间：2016.09.25
** 创建人：清风海岸
**文件版本 1.0
** 修改记录：
***************************************************************************************/
#include "crc.h"
#include "boot_debug.h"
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


uint32_t calc_crc32(char *buff, int32_t len,uint32_t crc)
{
    int32_t i;
    if(!have_table) 
        make_table();
    //boot_debug("calculate CRC base 0x%x,len %d",buff,len);
    crc = ~crc;
    for (i = 0;i < len;i++)
        crc = (crc >> 8) ^ table[(crc ^ buff[i]) & 0xff];
    return ~crc;
}

#ifdef __cplusplus
}
#endif
