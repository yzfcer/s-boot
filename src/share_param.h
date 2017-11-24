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
#ifndef SHARE_PARAM_H__
#define SHARE_PARAM_H__
#include "wind_type.h"
#ifdef __cplusplus
extern "C" {
#endif

#define SHARE_PARAM_MAGIC 0x52d360a5
#define SHARE_BASE 0x2002F000
#define SHARE_LENTH 0x1000
typedef struct 
{
    w_uint32_t exist;
    w_uint32_t isOK;
}hw_status_s;

//升级参数区
typedef struct
{
    w_uint32_t flag;//升级标志
    w_uint32_t mem_type;//升级程序的介质类型
    w_uint32_t addr;//升级程序的地址
    w_uint32_t size;//升级程序的空间大小
    w_uint32_t datalen;//升级程序的数据长度
}upgrade_info_s;

//保留空间参数区
typedef struct
{
    w_uint32_t addr;//保留空间的地址
    w_uint32_t size;//保留空间的长度
    w_uint32_t mem_type;//保留空间的类型
    w_uint32_t pad;
}sysparam_region_s;


void sp_set_app_rollback(w_uint8_t is_rollback);

w_int32_t sp_get_app_rollback(w_uint8_t *is_rollback);

void sp_set_upgrade_param(upgrade_info_s *upreg);

w_int32_t sp_get_upgrade_param(upgrade_info_s *upreg);

void sp_set_sysparam_param(sysparam_region_s *sysparam);

w_int32_t sp_get_sysparam_param(sysparam_region_s *sysparam);

void sp_init_share_param(void);

#ifdef __cplusplus
}
#endif

#endif

