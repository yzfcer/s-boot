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
#ifndef BOOT_INPUT_IF_H__
#define BOOT_INPUT_IF_H__
#include "boot_type.h"
#ifdef __cplusplus
extern "C" {
#endif
void boot_delay(uint32_t ms);

int boot_printf(const char *fmt,...);

int32_t read_char_blocking(char *ch);
int32_t read_line_blockig(char *buff,int32_t len);
int32_t wait_for_key_input(int32_t to_sec,char *ch,int32_t print32_t_flag);

uint8_t *get_block_buffer(void);
int32_t read_block(memtype_e memtype,uint32_t memidx,uint32_t addr,uint8_t *buf,int32_t blkcount);
int32_t write_block(memtype_e memtype,uint32_t memidx,uint32_t addr,uint8_t *buf,int32_t blkcount);
int32_t erase_block(memtype_e memtype,uint32_t memidx,uint32_t addr,int32_t blkcount);

#ifdef __cplusplus
}
#endif

#endif
