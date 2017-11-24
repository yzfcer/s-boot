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
#include "wind_type.h"
#include "mem_map.h"
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
    
    mem_map_s mem_map;//系统的空间映射表
}boot_param_s;

void *get_boot_params(void);
void *get_boot_params_from_ROM(void);

void param_init(void);
w_int32_t param_check_valid(w_uint8_t *prmbuf);
w_int32_t param_check_debug_mode(void);
void    param_clear_buffer(void);
w_int32_t param_read(void);
w_int32_t param_flush(void);

#ifdef __cplusplus
}
#endif

#endif

