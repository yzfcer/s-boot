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
#include "wind_config.h" 
#include "boot_param.h"
#include "boot_port.h"
#include "wind_debug.h"
#include "wind_crc32.h"
#include "mem_driver.h"
#include "boot_hw_if.h"
#include "phy_mem.h"
#include "boot_part.h"
#include "wind_string.h"
#ifdef __cplusplus
extern "C" {
#endif
#define BT_BUF_SIZE 1024

boot_param_s *g_pbp = W_NULL;
static w_uint8_t g_bootparam[BT_BUF_SIZE];


static void upate_bootparam_crc(w_uint8_t *prmbuf)
{
    w_uint32_t *crc = (w_uint32_t*)&prmbuf[sizeof(boot_param_s)];
    *crc = wind_crc32(prmbuf,sizeof(boot_param_s),0xffffffff);
}

boot_param_s *boot_param_instance(void)
{
    if(W_NULL == g_pbp)
    {
        g_pbp = (boot_param_s *)&g_bootparam;
        g_pbp = (boot_param_s*)boot_param_from_rom();
    }
    return (boot_param_s *)g_pbp;
}

boot_param_s *boot_param_from_rom(void)
{
    w_int32_t ret;
    ret = boot_param_read();
    if(0 != ret)
    {
        wind_warn("get boot params failed.");
        g_pbp = W_NULL;
        return W_NULL;
    }
    g_pbp = (boot_param_s *)&g_bootparam;
    return (boot_param_s*)g_pbp;
}


void boot_param_reset(void)
{   
    boot_param_s *bp = (boot_param_s*)g_bootparam;
    
    wind_memset(g_bootparam,0,BT_BUF_SIZE);
    bp->magic = BOOT_PARAM_MAGIC;
    bp->lenth = sizeof(boot_param_s);

    bp->version = BOOT_VERSION;
    bp->debug_mode = 0;
    bp->wait_sec = WAIT_KEY_SEC;
    bp->run_type = RUN_SPACE_TYPE;
    bp->encrypt_type = ENCRYPT_TYPE;
    bp->lock_en = MCU_LOCK_ENABLE;
    
    if(phymem_get_count() == 0)
    {
        phymems_register();
        bp->phymem_cnt = phymem_get_count();
    }
    if(part_get_count() == 0)
    {
        parts_create();
        bp->part_cnt = part_get_count();
    }
    wind_memcpy(bp->phmem,phymem_get_list(),PHYMEM_COUNT*sizeof(phymem_s));
    wind_memcpy(bp->part,part_get_list(),PART_COUNT*sizeof(part_s));
    wind_notice("init boot param OK.");
}


//检查参数是否有效，有效返回1，无效返回0
w_int32_t boot_param_check_valid(w_uint8_t *prmbuf)
{
    boot_param_s *bp = (boot_param_s *)prmbuf;
    w_uint32_t *crc = (w_uint32_t*)&prmbuf[sizeof(boot_param_s)];
    if(bp->magic != BOOT_PARAM_MAGIC)
    {
        wind_warn("param block is invalid.");
        return -1;
    }
    if(bp->lenth != sizeof(boot_param_s))
    {
        wind_warn("param block lenth is invalid.");
        return -1;
    }
    if(*crc != wind_crc32((w_uint8_t*)bp,sizeof(boot_param_s),0xffffffff))
    {
        wind_warn("param block crc is invalid.");
        return -1;
    }
    if(bp->version > BOOT_VERSION)
    {
        wind_warn("param block version is not matched.");
        return -1;
    }
    return 0;
}

void boot_param_clear_buffer(void)
{
    w_int32_t i;
    for(i = 0;i < sizeof(g_bootparam);i ++)
    {
        g_bootparam[i] = 0;
    }
    g_pbp = W_NULL;
}

w_int32_t boot_param_read(void)
{
    w_uint32_t err = 0;
    w_int32_t i,j,len,ret;
    part_s *part[2];
    
    part[0] = part_get_inst_name("btpara1");
    part[1] = part_get_inst_name("btpara2");
    
    for(i = 0;i < 2;i ++)
    {
        for(j = 0;j < 3;j ++)
        {
            len = read_block(MEM_TYPE_ROM,part[i]->memidx,part[i]->addr,g_bootparam,sizeof(g_bootparam) / BLOCK_SIZE);
            if(len >= sizeof(g_bootparam) / BLOCK_SIZE)
            {
                break;
            }
        }
        ret = boot_param_check_valid(g_bootparam);
        if(0 == ret)
        {
            break;
        }
        else
        {
            wind_warn("read param %d fail.",i + 1);
            err ++;
        }
    }
    if(err >= 2)
    {
        wind_warn("read both params failed.");
        return -1;
    }
    return 0;
    
}

w_int32_t boot_param_flush(void)
{
    w_int32_t i,j,len,err = 0;
    part_s *part[2];
    boot_param_s *bp = (boot_param_s *)g_bootparam;
    part[0] = part_get_inst_name("btpara1");
    part[1] = part_get_inst_name("btpara2");
    
    //part[0]->datalen = sizeof(boot_param_s) + bp->map_size;
    //part[1]->datalen = sizeof(boot_param_s) + bp->map_size;
    upate_bootparam_crc((w_uint8_t*)bp);
    for(i = 0;i < 2;i ++)
    {
        for(j = 0;j < 3;j ++)
        {
            len = write_block(MEM_TYPE_ROM,part[i]->memidx,part[i]->addr,(char*)g_bootparam,sizeof(g_bootparam) / BLOCK_SIZE);
            if(len >=  sizeof(g_bootparam) / BLOCK_SIZE)
            {
                break;
            }
        }
        if(j >= 3)
        {
            
            wind_warn("write param %d fail.",i + 1);
            err ++;
        }
        wind_debug("write param %d OK.",i + 1);
    }
    if(err >= 2)
    {
        wind_warn("write both params failed.");
        return -1;
    }
    wind_notice("write boot param complete.");
    return 0;
}

w_int32_t boot_param_check_debug_mode(void)
{
    boot_param_s *bp = (boot_param_s*)boot_param_instance();
    if(W_NULL == bp)
    {
        return 0;
    }
    return bp->debug_mode;
}

void *boot_param_mem_map(void)
{
    boot_param_s *bp = boot_param_instance();
    return (void*)(sizeof(boot_param_s)+(w_uint32_t)bp);
}



#ifdef __cplusplus
}
#endif

