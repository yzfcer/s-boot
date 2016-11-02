/*********************************************************************************
  *Copyright(C),2016-2018,yzfcer@163.com
  *FileName:  
  *Author:      Zhou Jiangcun
  *Version:     1.0
  *Date:        2016/11/2
  *Description:  
  *Others:  
  *History:  
     1.Date:
       Author:
       Modification:
**********************************************************************************/
#ifndef BOOT_CHECK_H__
#define BOOT_CHECK_H__
#include "mem_map.h"
int32_t check_rom_program(region_s *code);
int32_t check_rom_programs(void);

#endif
