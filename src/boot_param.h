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
#define BLOCK_SIZE 4096

typedef struct 
{
    uint32_t lenth;
    uint32_t crc;
}pro_verify_s;

//存储于flash参数区的数据的整体结构，
//这部分数据在内存中没有备份，在需要时从flash读出
typedef struct 
{
    uint32_t magic;//魔术字，用于参数头部校验
    uint32_t lenth;//本参数结构体的长度
    uint32_t flush_num;//参数区擦写次数
    
    uint8_t version;//bootloader`
    uint8_t debug_mode;//调试模式
    uint8_t wait_sec;//等待键盘事件秒数
    bool_t encrypt_en;//程序加密使能
    bool_t lock_en;//芯片锁定使能
    mem_map_s mem_map;//系统的空间映射表
    uint32_t crc;//CRC校验
}boot_param_s;

void *get_boot_params(void);
void *get_boot_params_from_ROM(void);

void init_boot_param(const mem_map_s *mmap);
int32_t check_boot_param(boot_param_s *btprm);
int32_t check_app_debug_mode(void);
void clear_boot_param_buffer(void);
int32_t read_param(void);
int32_t write_param(void);

#ifdef __cplusplus
}
#endif

#endif

