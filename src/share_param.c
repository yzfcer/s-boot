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
#include "share_param.h"
#include "boot_port.h"
#include "sys_debug.h"
#include "crc.h"

#ifdef __cplusplus
extern "C" {
#endif
//共享地址
//#define get_share_addr get_share_addr()//0x2003F000
extern w_uint32_t get_share_addr (void);

#define SHARE_VERSION 0x0001
typedef struct
{
    w_uint32_t magic;
    w_uint32_t lenth;
    w_uint32_t share_version;

    //回滚标记
    w_uint32_t rollback_flag;//应用程序在异常时设置位1，正常为0

    //升级参数区
    upgrade_info_s upgrade_reg;

    //系统参数区，用于保留应用程序参数
    sysparam_region_s sysparam_reg;
        
}share_param_s;

static void update_share_crc(void)
{
    share_param_s *sp = (share_param_s *)(void*)get_share_addr();
    w_uint32_t *crc = (w_uint32_t*)(sp+1);
    *crc = calc_crc32((char*)sp,sizeof(share_param_s),0xffffffff);
}

static void copy_share_data(char *src,char *dest,w_int32_t len)
{
    w_int32_t i;
    //sys_notice("copy data from 0x%x to 0x%x,lenth %d",src,dest,len);
    for(i = 0;i < len;i ++)
    {
        dest[i] = src[i];
    }
}


static w_int32_t check_share_param(void)
{
    
    share_param_s *sp = (share_param_s *)(void*)get_share_addr();
    w_uint32_t *crc = (w_uint32_t*)(sp+1);
    if(sp->magic != SHARE_PARAM_MAGIC)
    {
        sys_notice("share param block is invalid.");
        return -1;
    }
    if(sp->lenth != sizeof(share_param_s))
    {
        sys_warn("share param block lenth is invalid.");
        return -1;
    }

    if(*crc != calc_crc32((char*)sp,sizeof(share_param_s),0xffffffff))
    {
        sys_warn("share param block crc is invalid.");
        return -1;
    }

    return 0;
}

void sp_set_app_rollback(w_uint8_t is_rollback)
{
    share_param_s * sp =(share_param_s *)(void*)get_share_addr();
    sp->rollback_flag = is_rollback;
    update_share_crc();
}

w_int32_t sp_get_app_rollback(w_uint8_t *is_rollback)
{
    share_param_s *sp = (share_param_s *)(void*)get_share_addr();
    if(check_share_param())
        return -1;
    *is_rollback = sp->rollback_flag;
    return 0;
}


void sp_set_upgrade_param(upgrade_info_s *upreg)
{
    share_param_s *sp = (share_param_s *)(void*)get_share_addr();
    copy_share_data((char*)upreg,(char*)&sp->upgrade_reg,sizeof(upgrade_info_s));
    update_share_crc();
}

w_int32_t sp_get_upgrade_param(upgrade_info_s *upreg)
{
    share_param_s *sp = (share_param_s *)(void*)get_share_addr();
    if(check_share_param())
        return -1;
    copy_share_data((char*)&sp->upgrade_reg,(char*)upreg,sizeof(upgrade_info_s));
    return 0;
}

void sp_set_sysparam_param(sysparam_region_s *sysparam)
{
    share_param_s *sp = (share_param_s *)(void*)get_share_addr();
    copy_share_data((char*)sysparam,(char*)&sp->sysparam_reg,sizeof(sysparam_region_s));
    update_share_crc();
}

w_int32_t sp_get_sysparam_param(sysparam_region_s *sysparam)
{
    share_param_s *sp = (share_param_s *)(void*)get_share_addr();
    if(check_share_param())
        return -1;
    copy_share_data((char*)&sp->sysparam_reg,(char*)sysparam,sizeof(sysparam_region_s));
    return 0;
}

void sp_init_share_param(void)
{
    w_int32_t i;
    char *mem;
    share_param_s *sp = (share_param_s *)(void*)get_share_addr();
    mem = (char*)sp;
    for(i = 0;i < sizeof(share_param_s);i ++)
    {
        mem[i] = 0;
    }
    sp->magic = SHARE_PARAM_MAGIC;
    sp->lenth = sizeof(share_param_s);
    sp->share_version = SHARE_VERSION;
    
    sp->rollback_flag = 0;
    sp->upgrade_reg.flag = 0;
    update_share_crc();
}

#ifdef __cplusplus
}
#endif

