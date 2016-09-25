/***************************************************************************************
** 文件名：
** 文件信息：
** 创建时间：2015.10.13
** 创建人：周江村
**文件版本1.0
** 修改记录：
***************************************************************************************/
#ifndef BOOT_PARAM_H__
#define BOOT_PARAM_H__
#include "mem_map.h"
#ifdef __cplusplus
extern "C" {
#endif

#define BOOT_PARAM_MAGIC 0X2561C5A0
#define BOOT_VERSION 1
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
    
    uint8_t version;//bootloader的版本号
    uint8_t debug_mode;
    uint8_t pad1[1];

    uint32_t flush_num;//参数区擦写次数
    mem_map_s mem_map;//系统的空间映射表

    uint32_t crc;//CRC校验
}boot_param_s;

void *sys_boot_params(void);
void init_map_info(mem_map_s *map);

void *get_boot_params_from_ROM(void);
void upate_bootparam_crc(boot_param_s *btprm);
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

