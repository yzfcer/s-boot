/***************************************************************************************
** 文件名：
** 文件信息：
** 创建时间：2016.09.25
** 创建人：清风海岸
**文件版本1.0
** 修改记录：
***************************************************************************************/
#include "share_param.h"
#include "port.h"
////#include "hw_device.h"
#include "boot_debug.h"
#include "crc.h"

#ifdef __cplusplus
extern "C" {
#endif
//共享地址
//#define get_share_addr get_share_addr()//0x2003F000
extern uint32_t get_share_addr (void);

#define SHARE_VERSION 1// share版本号，考虑到以后结构扩充，需要兼容处理, 兼容处理的细节，以后再考虑
typedef struct
{
    uint32_t magic;//魔术字，用于参数头部校验
    uint32_t lenth;//本参数结构体的长度
    uint32_t share_version;//共享区版本

    //回滚标记
    uint32_t rollback_flag;//应用程序在异常时设置位1，正常为0

    //升级参数区
    upgrade_region_s upgrade_reg;

    //保留空间参数区
    reserve_region_s reserve_reg;
        
    //空间状态参数区
    mem_status_s mem_status;

    //升级文件相关的信息区
    img_head_s img_head;
    uint32_t pad;
    uint32_t crc;//CRC校验
}share_param_s;

static void update_share_crc(void)
{
    share_param_s *sp = (share_param_s *)(void*)get_share_addr();
    sp->crc = calc_crc32((char*)sp,sizeof(share_param_s) - sizeof(sp->crc),0);
}

static void copy_share_data(char *src,char *dest,int32_t len)
{
    int32_t i;
    //boot_notice("copy data from 0x%x to 0x%x,lenth %d",src,dest,len);
    for(i = 0;i < len;i ++)
    {
        dest[i] = src[i];
    }
}

//检查参数是否有效，有效返回1，无效返回0
static int32_t check_share_param(void)
{
    
    share_param_s *sp = (share_param_s *)(void*)get_share_addr();

    if(sp->magic != SHARE_PARAM_MAGIC)
    {
        boot_notice("share param block is invalid.");
        return -1;
    }
    if(sp->lenth != sizeof(share_param_s))
    {
        boot_warn("share param block lenth is invalid.");
        return -1;
    }

    if(sp->crc != calc_crc32((char*)sp,sizeof(share_param_s) - sizeof(sp->crc),0))
    {
        boot_warn("share param block crc is invalid.");
        return -1;
    }

    return 0;
}

void sp_set_app_rollback(uint8_t is_rollback)
{
    share_param_s * sp =(share_param_s *)(void*)get_share_addr();
    sp->rollback_flag = is_rollback;
    update_share_crc();
}

int32_t sp_get_app_rollback(uint8_t *is_rollback)
{
    share_param_s *sp = (share_param_s *)(void*)get_share_addr();
    if(check_share_param())
        return -1;
    *is_rollback = sp->rollback_flag;
    return 0;
}


void sp_set_upgrade_param(upgrade_region_s *upreg)
{
    share_param_s *sp = (share_param_s *)(void*)get_share_addr();
    copy_share_data((char*)upreg,(char*)&sp->upgrade_reg,sizeof(upgrade_region_s));
    update_share_crc();
}

int32_t sp_get_upgrade_param(upgrade_region_s *upreg)
{
    share_param_s *sp = (share_param_s *)(void*)get_share_addr();
    if(check_share_param())
        return -1;
    copy_share_data((char*)&sp->upgrade_reg,(char*)upreg,sizeof(upgrade_region_s));
    return 0;
}

void sp_set_reserve_param(reserve_region_s *reserve)
{
    share_param_s *sp = (share_param_s *)(void*)get_share_addr();
    copy_share_data((char*)reserve,(char*)&sp->reserve_reg,sizeof(reserve_region_s));
    update_share_crc();
}

int32_t sp_get_reserve_param(reserve_region_s *reserve)
{
    share_param_s *sp = (share_param_s *)(void*)get_share_addr();
    if(check_share_param())
        return -1;
    copy_share_data((char*)&sp->reserve_reg,(char*)reserve,sizeof(reserve_region_s));
    return 0;
}


void sp_set_mem_status(mem_status_s * mem_status)
{
    share_param_s *sp = (share_param_s *)(void*)get_share_addr();
    copy_share_data((char*)mem_status,(char*)&sp->mem_status,sizeof(mem_status_s));
    update_share_crc();
}


int32_t sp_get_mem_status(mem_status_s * mem_status)
{
    share_param_s *sp = (share_param_s *)(void*)get_share_addr();
    if(check_share_param())
        return -1;
    copy_share_data((char*)mem_status,(char*)&sp->mem_status,sizeof(mem_status_s));
    return 0;
}

void sp_set_upgrade_file_param(img_head_s *img_file)
{
    share_param_s *sp = (share_param_s *)(void*)get_share_addr();
    copy_share_data((char*)img_file,(char*)&sp->img_head,sizeof(img_head_s));
    update_share_crc();
}

int32_t sp_get_upgrade_file_param(img_head_s *img_file)
{
    share_param_s *sp = (share_param_s *)(void*)get_share_addr();
    if(check_share_param())
        return -1;
    copy_share_data((char*)&sp->img_head,(char*)img_file,sizeof(img_head_s));
    return 0;
}

void sp_init_share_param(void)
{
    int32_t i;
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

