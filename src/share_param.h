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
    uint32_t size;//��������Ŀռ��С
    uint32_t datalen;//������������ݳ���
}upgrade_info_s;

//�����ռ������
typedef struct
{
    uint32_t addr;//�����ռ�ĵ�ַ
    uint32_t size;//�����ռ�ĳ���
    uint32_t mem_type;//�����ռ������
    uint32_t pad;
}sysparam_region_s;


void sp_set_app_rollback(uint8_t is_rollback);

int32_t sp_get_app_rollback(uint8_t *is_rollback);

void sp_set_upgrade_param(upgrade_info_s *upreg);

int32_t sp_get_upgrade_param(upgrade_info_s *upreg);

void sp_set_sysparam_param(sysparam_region_s *sysparam);

int32_t sp_get_sysparam_param(sysparam_region_s *sysparam);

void sp_init_share_param(void);

#ifdef __cplusplus
}
#endif

#endif

