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
#include "boot_config.h"
#include "boot_part.h"
#include "boot_port.h"
#include "wind_debug.h"
#include "wind_crc32.h"

#ifdef __cplusplus
extern "C" {
#endif
 
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
    sysparam_part_s sysparam_reg;
        
}share_param_s;



static void update_share_crc(void)
{
    w_part_s *part = boot_part_get(PART_SHARE);
    boot_part_calc_crc(part);
}

static void copy_share_data(char *src,char *dest,w_int32_t len)
{
    w_int32_t i;
    
    for(i = 0;i < len;i ++)
    {
        dest[i] = src[i];
    }
}


static w_int32_t check_share_param(share_param_s *sp)
{
    w_uint32_t *crc = (w_uint32_t*)(sp+1);
    if(sp->magic != SHARE_PARAM_MAGIC)
    {
        wind_notice("share param block is invalid.");
        return -1;
    }
    if(sp->lenth != sizeof(share_param_s))
    {
        wind_warn("share param block lenth is invalid.");
        return -1;
    }

    if(*crc != wind_crc32((w_uint8_t*)sp,sizeof(share_param_s),0xffffffff))
    {
        wind_warn("share param block crc is invalid.");
        return -1;
    }

    return 0;
}

static share_param_s *get_share_data(void)
{
    w_int32_t len;
    share_param_s * sp;
    w_uint8_t *buff = get_common_buffer();
    
    w_part_s *part = boot_part_get(PART_SHARE);
    WIND_ASSERT_RETURN(COMMBUF_SIZE >= part->size,W_NULL);
    boot_part_seek(part,0);
    len = boot_part_read(part,buff,part->size);
    WIND_ASSERT_RETURN(len > 0,W_NULL);
    sp =(share_param_s *)buff;
    if(check_share_param(sp) != 0)
        return W_NULL;
    return sp;
}

static w_err_t flush_share_data(share_param_s *sp)
{
    w_int32_t len;
    w_part_s *part = boot_part_get(PART_SHARE);
    update_share_crc();
    boot_part_seek(part,0);
    len = boot_part_write(part,(w_uint8_t*)sp,COMMBUF_SIZE);
    WIND_ASSERT_RETURN(len > 0,W_ERR_FAIL);
    return W_ERR_OK;
}


w_err_t sp_set_app_rollback(w_uint8_t is_rollback)
{
    share_param_s *sp = get_share_data();
    WIND_ASSERT_RETURN(sp != W_NULL,W_ERR_FAIL);
    sp->rollback_flag = is_rollback;
    return flush_share_data(sp);
}

w_err_t sp_get_app_rollback(w_uint8_t *is_rollback)
{
    share_param_s *sp = get_share_data();
    WIND_ASSERT_RETURN(sp != W_NULL,W_ERR_FAIL);
    *is_rollback = (w_uint8_t)sp->rollback_flag;
    return flush_share_data(sp);
}


w_err_t sp_set_upgrade_param(upgrade_info_s *upreg)
{
    share_param_s *sp = get_share_data();
    WIND_ASSERT_RETURN(sp != W_NULL,W_ERR_FAIL);
    copy_share_data((char*)upreg,(char*)&sp->upgrade_reg,sizeof(upgrade_info_s));
    return flush_share_data(sp);
}

w_err_t sp_get_upgrade_param(upgrade_info_s *upreg)
{
    share_param_s *sp = get_share_data();
    WIND_ASSERT_RETURN(sp != W_NULL,W_ERR_FAIL);
    copy_share_data((char*)&sp->upgrade_reg,(char*)upreg,sizeof(upgrade_info_s));
    return flush_share_data(sp);
}

w_err_t sp_set_sysparam_param(sysparam_part_s *sysparam)
{
    share_param_s *sp = get_share_data();
    WIND_ASSERT_RETURN(sp != W_NULL,W_ERR_FAIL);
    copy_share_data((char*)sysparam,(char*)&sp->sysparam_reg,sizeof(sysparam_part_s));
    return flush_share_data(sp);
}

w_err_t sp_get_sysparam_param(sysparam_part_s *sysparam)
{
    share_param_s *sp = get_share_data();
    WIND_ASSERT_RETURN(sp != W_NULL,W_ERR_FAIL);
    copy_share_data((char*)&sp->sysparam_reg,(char*)sysparam,sizeof(sysparam_part_s));
    return flush_share_data(sp);
}

w_err_t sp_init_share_param(void)
{
    w_int32_t i;
    share_param_s *sp = (share_param_s *)get_common_buffer();

    wind_memset((void *)sp,0,sizeof(share_param_s));
    sp->magic = SHARE_PARAM_MAGIC;
    sp->lenth = sizeof(share_param_s);
    sp->share_version = SHARE_VERSION;
    
    sp->rollback_flag = 0;
    sp->upgrade_reg.flag = 0;
    return flush_share_data(sp);
}

#ifdef __cplusplus
}
#endif

