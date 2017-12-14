/*********************************************************************************
  *Copyright(C),2016-2018,yzfcer@163.com
  *FileName:  
  *Author:      Jason Zhou
  *Version:     1.0
  *Date:        2016/10/08
  *Description:  
  *Others:  
  *History:  
     1.Date:
       Author:
       Modification:
**********************************************************************************/
#ifndef BOOT_PARAM_H__
#define BOOT_PARAM_H__
#include "wind_type.h"
#include "boot_part.h"
#ifdef __cplusplus
extern "C" {
#endif

#define BOOT_PARAM_MAGIC 0X2561C5A0
#define BOOT_VERSION 0x010000

//存储于flash参数区的数据的整体结构，
//这部分数据在内存中没有备份，在需要时从flash读出
typedef struct 
{
    w_uint32_t magic;//魔术字，用于参数头部校验
    w_uint32_t lenth;//本参数结构体的长度
    
    w_uint32_t version;    //bootloader`
    w_uint8_t debug_mode; //调试模式
    w_uint8_t wait_sec;   //等待键盘事件秒数
    w_uint8_t run_type;   //运行在RAM或Flash
    w_uint8_t encrypt_type;//程序加密使能
    w_uint8_t lock_en;     //芯片锁定使能

    w_int8_t phymem_max;
    w_int8_t phymem_cnt;
    
    w_int8_t part_max;
    w_int8_t part_cnt;
    
    w_int32_t reg_count;
    w_int32_t map_size;
    char runreg[PART_NAME_LEN];

}boot_param_s;

boot_param_s *boot_param_instance(void);
boot_param_s *boot_param_from_rom(void);

void      boot_param_reset(void);
w_int32_t boot_param_check_valid(w_uint8_t *prmbuf);
w_int32_t boot_param_check_debug_mode(void);
void      boot_param_clear_buffer(void);
w_int32_t boot_param_read(void);
w_int32_t boot_param_flush(void);
void *boot_param_mem_map(void);
#ifdef __cplusplus
}
#endif

#endif

