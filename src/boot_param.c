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
#include "boot_config.h" 
#include "boot_param.h"
#include "port.h"
#include "boot_debug.h"
#include "crc.h"
#include "mem_driver.h"

#ifdef __cplusplus
extern "C" {
#endif


static char g_bootparam[PARAM_LENTH];
uint32_t g_param_write_num = 0;
boot_param_s *g_pbp = NULL;

static void init_reg_name(boot_param_s *bp)
{
    int32_t i;
    region_s *mp = (region_s*)&bp->mem_map;
    int32_t cnt = (sizeof(boot_param_s)/sizeof(region_s));
    for(i = 0;i < cnt;i ++)
    {
        mp[i].regname = region_name(i);
    }
}

void *get_boot_params(void)
{
    if(NULL == g_pbp)
        g_pbp = (boot_param_s*)get_boot_params_from_ROM();
    return (void *)g_pbp;
}

void *get_boot_params_from_ROM(void)
{
    int32_t i,ret;
    mem_map_s *map;
    region_s *src,*dest;
    ret = param_read();
    if(0 != ret)
    {
        boot_warn("get boot params failed.");
        g_pbp = NULL;
        return NULL;
    }
    map = get_memory_map();
    init_reg_name((boot_param_s *)&g_bootparam);
    g_pbp = (boot_param_s *)&g_bootparam;
    g_param_write_num = g_pbp->flush_num;
    src = (region_s*)map;
    dest = (region_s*)&g_pbp->mem_map;
    for(i = 0;i < sizeof(g_pbp->mem_map)/sizeof(region_s);i ++)
    {
        dest[i].addr = src[i].addr;
    }
    return (void*)g_pbp;
}

void upate_bootparam_crc(uint8_t *prmbuf)
{
    uint32_t *crc = (uint32_t*)&prmbuf[sizeof(boot_param_s)];
    *crc = calc_crc32((char*)prmbuf,sizeof(boot_param_s),0);
}

//在bootloader首次运行时调用，烧入默认的参数
void param_init(const mem_map_s *mmap)
{
    int32_t i;
    char *src,*dest;
    boot_param_s *bp = (boot_param_s*)g_bootparam;
    bp->magic = BOOT_PARAM_MAGIC;
    bp->lenth = sizeof(boot_param_s);
    bp->flush_num = g_param_write_num;

    bp->version = BOOT_VERSION;
    bp->debug_mode = 0;
    bp->wait_sec = WAIT_KEY_SEC;
    bp->run_type = RUN_SPACE_TYPE;
    bp->encrypt_type = ENCRYPT_TYPE;
    bp->lock_en = MCU_LOCK_ENABLE;
    
    src = (char*)mmap;
    dest = (char*)&bp->mem_map;
    for(i = 0;i < sizeof(mem_map_s);i ++)
    {
        dest[i] = src[i];
    }
    init_map_info(&bp->mem_map);
    boot_notice("init boot param OK.");
}


//检查参数是否有效，有效返回1，无效返回0
int32_t param_check_valid(uint8_t *prmbuf)
{
    boot_param_s *bp = (boot_param_s *)prmbuf;
    uint32_t *crc = (uint32_t*)&prmbuf[sizeof(boot_param_s)];
    if(bp->magic != BOOT_PARAM_MAGIC)
    {
        boot_warn("param block is invalid.");
        return -1;
    }
    if(bp->lenth!= sizeof(boot_param_s))
    {
        boot_warn("param block lenth is invalid.");
        return -1;
    }
    if(*crc != calc_crc32((char*)bp,sizeof(boot_param_s),0))
    {
        boot_warn("param block crc is invalid.");
        return -1;
    }
    if(bp->version > BOOT_VERSION)
    {
        boot_warn("param block version is not matched.");
        return -1;
    }
    return 0;
}
void param_clear_buffer(void)
{
    int32_t i;
    for(i = 0;i < sizeof(g_bootparam);i ++)
    {
        g_bootparam[i] = 0;
    }
    g_pbp = NULL;
}



int32_t param_read(void)
{
    uint32_t err = 0;
    int32_t i,j,len,ret;
    region_s *reg[2];
    mem_map_s *map = get_memory_map();
    reg[0] = &map->rom.param1_region;
    reg[1] = &map->rom.param2_region;
    
    for(i = 0;i < 2;i ++)
    {
        for(j = 0;j < 3;j ++)
        {
            len = read_block(MEM_TYPE_ROM,reg[i]->index,reg[i]->addr,g_bootparam,sizeof(g_bootparam) / BLOCK_SIZE);
            if(len >= sizeof(g_bootparam) / BLOCK_SIZE)
            {
                break;
            }
        }
        ret = param_check_valid(g_bootparam);
        if(0 == ret)
        {
            break;
        }
        else
        {
            boot_warn("read param %d fail.",i + 1);
            err ++;
        }
    }
    if(err >= 2)
    {
        boot_warn("read both params failed.");
        return -1;
    }
    boot_notice(" *** BOOT param program times:%d.",((boot_param_s *)g_bootparam)->flush_num);
    return 0;
    
}

int32_t param_flush(void)
{
    int32_t i,j,len,err = 0;
    region_s *reg[2];
    boot_param_s *bp = (boot_param_s *)g_bootparam;
    mem_map_s *map = get_memory_map();
    reg[0] = &map->rom.param1_region;
    reg[1] = &map->rom.param2_region;
    
    bp->flush_num ++;
    bp->mem_map.rom.param1_region.lenth = sizeof(boot_param_s);
    bp->mem_map.rom.param2_region.lenth = sizeof(boot_param_s);    
    upate_bootparam_crc(bp);
    for(i = 0;i < 2;i ++)
    {
        for(j = 0;j < 3;j ++)
        {
            len = write_block(MEM_TYPE_ROM,reg[i]->index,reg[i]->addr,(char*)g_bootparam,sizeof(g_bootparam) / BLOCK_SIZE);
            if(len >=  sizeof(g_bootparam) / BLOCK_SIZE)
            {
                break;
            }
        }
        if(j >= 3)
        {
            
            boot_warn("write param %d fail.",i + 1);
            err ++;
        }
        boot_debug("write param %d OK.",i + 1);
    }
    if(err >= 2)
    {
        boot_warn("write both params failed.");
        return -1;
    }
    boot_notice("write boot param complete.");
    return 0;
}

int32_t param_check_debug_mode(void)
{
    boot_param_s *bp = (boot_param_s*)get_boot_params();
    if(NULL == bp)
    {
        return 0;
    }
    return bp->debug_mode;
}

#ifdef __cplusplus
}
#endif

