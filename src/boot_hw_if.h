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
#include "wind_type.h"
#ifdef __cplusplus
extern "C" {
#endif
void boot_delay(w_uint32_t ms);

w_int32_t wind_printf(const char *fmt,...);

w_int32_t read_char_blocking(char *ch);
w_int32_t read_line_blockig(char *buff,w_int32_t len);
w_int32_t wait_for_key_input(w_int32_t to_sec,char *ch,w_int32_t print32_t_flag);

w_uint8_t *get_block_buffer(void);
w_int32_t read_block(memtype_e memtype,w_uint32_t memidx,w_uint32_t addr,w_uint8_t *buf,w_int32_t blkcount);
w_int32_t write_block(memtype_e memtype,w_uint32_t memidx,w_uint32_t addr,w_uint8_t *buf,w_int32_t blkcount);
w_int32_t erase_block(memtype_e memtype,w_uint32_t memidx,w_uint32_t addr,w_int32_t blkcount);

#ifdef __cplusplus
}
#endif

#endif
