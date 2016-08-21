/***************************************************************************************
** 文件名：
** 文件信息：
** 创建时间：2015.10.13
** 创建人：周江村
**文件版本1.0
** 修改记录：
***************************************************************************************/
#ifndef SHARE_PARAM_H__
#define SHARE_PARAM_H__
#include "boot_type.h"
#ifdef __cplusplus
extern "C" {
#endif

#define SHARE_PARAM_MAGIC 0x52d360a5
#define SHARE_BASE 0x2002F000
#define SHARE_LENTH 0x1000
typedef struct 
{
    uint32_t exist;
    uint32_t isOK;
}hw_status_s;

//升级参数区
typedef struct
{
    uint32_t flag;//升级标志
    uint32_t mem_type;//升级程序的介质类型
    uint32_t addr;//升级程序的地址
    uint32_t lenth;// 升级程序的长度
    uint32_t crc;//升级程序的校验值
    uint32_t pad;
}upgrade_region_s;

//保留空间参数区
typedef struct
{
    uint32_t addr;//保留空间的地址
    uint32_t lenth;//保留空间的长度
    uint32_t mem_type;//保留空间的类型
    uint32_t pad;
}reserve_region_s;

typedef struct 
{
    hw_status_s iflash;
    hw_status_s iram;
    hw_status_s sflash;
    hw_status_s xram;
}mem_status_s;

typedef struct 
{
    char head[8];
    uint32_t ver;
    uint32_t head_len;//文件头部的长度
    char file_name[64];//加密前的文件名
    uint32_t file_version;//应用程序的版本号
    uint32_t file_len;//加密前文件长度
    uint32_t file_crc;//加密前的CRC校验
    uint32_t pad;//八字节对齐
}img_head_s;



void sp_set_app_rollback(uint8_t is_rollback);

int32_t sp_get_app_rollback(uint8_t *is_rollback);

void sp_set_upgrade_param(upgrade_region_s *upreg);

int32_t sp_get_upgrade_param(upgrade_region_s *upreg);

void sp_set_reserve_param(reserve_region_s *reserve);

int32_t sp_get_reserve_param(reserve_region_s *reserve);

void sp_set_mem_status(mem_status_s * mem_status);

int32_t sp_get_mem_status(mem_status_s * mem_status);

void sp_set_upgrade_file_param(img_head_s *img_file);

void sp_set_upgrade_file_param(img_head_s *img_file);

void sp_init_share_param(void);

#ifdef __cplusplus
}
#endif

#endif

