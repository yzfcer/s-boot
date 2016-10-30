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
#ifndef BOOT_PARAM_H__
#define BOOT_PARAM_H__
#include "boot_type.h"
#include "mem_map.h"
#ifdef __cplusplus
extern "C" {
#endif

#define BOOT_PARAM_MAGIC 0X2561C5A0
#define BOOT_VERSION 0x000001

//存储于flash参数区的数据的整体结构，
//这部分数据在内存中没有备份，在需要时从flash读出
typedef struct 
{
    uint32_t magic;//魔术字，用于参数头部校验
    uint32_t lenth;//本参数结构体的长度
    
    uint8_t version;    //bootloader`
    uint8_t debug_mode; //调试模式
    uint8_t wait_sec;   //等待键盘事件秒数
    uint8_t run_type;   //运行在RAM或Flash
    uint8_t encrypt_type;//程序加密使能
    bool_t lock_en;     //芯片锁定使能
    
    mem_map_s mem_map;//系统的空间映射表
}boot_param_s;

void *get_boot_params(void);
void *get_boot_params_from_ROM(void);

void param_init(const mem_map_s *mmap);
int32_t param_check_valid(uint8_t *prmbuf);
int32_t param_check_debug_mode(void);
void    param_clear_buffer(void);
int32_t param_read(void);
int32_t param_flush(void);

#ifdef __cplusplus
}
#endif

#endif

