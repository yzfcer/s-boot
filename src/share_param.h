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

//����������
typedef struct
{
    uint32_t flag;//������־
    uint32_t mem_type;//��������Ľ�������
    uint32_t addr;//��������ĵ�ַ
    uint32_t datalen;// ��������ĳ���
}upgrade_region_s;

//�����ռ������
typedef struct
{
    uint32_t addr;//�����ռ�ĵ�ַ
    uint32_t size;//�����ռ�ĳ���
    uint32_t mem_type;//�����ռ������
    uint32_t pad;
}reserve_region_s;

typedef struct 
{
    hw_status_s iflash;
    hw_status_s iram;
    hw_status_s sflash;
    hw_status_s xram;
}mem_status_s;


void sp_set_app_rollback(uint8_t is_rollback);

int32_t sp_get_app_rollback(uint8_t *is_rollback);

void sp_set_upgrade_param(upgrade_region_s *upreg);

int32_t sp_get_upgrade_param(upgrade_region_s *upreg);

void sp_set_reserve_param(reserve_region_s *reserve);

int32_t sp_get_reserve_param(reserve_region_s *reserve);

void sp_set_mem_status(mem_status_s * mem_status);

int32_t sp_get_mem_status(mem_status_s * mem_status);

void sp_init_share_param(void);

#ifdef __cplusplus
}
#endif

#endif

