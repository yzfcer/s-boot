/***************************************************************************************
** 文件名：
** 文件信息：
** 创建时间：2016.09.25
** 创建人：清风海岸
**文件版本 1.0
** 修改记录：
***************************************************************************************/
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
    int32_t ret;

    ret = read_param();
    if(0 != ret)
    {
        boot_warn("get boot params failed.");
        g_pbp = NULL;
        return NULL;
    }
    init_reg_name((boot_param_s *)&g_bootparam);
    g_pbp = (boot_param_s *)&g_bootparam;
    g_param_write_num = g_pbp->flush_num;
    return (void*)g_pbp;
}

void upate_bootparam_crc(boot_param_s *btprm)
{
    btprm->crc = calc_crc32((char*)btprm,sizeof(boot_param_s) - sizeof(btprm->crc),0);
}

//在bootloader首次运行时调用，烧入默认的参数
void init_boot_param(const mem_map_s *mmap)
{
    int32_t i;
    char *src,*dest;
    boot_param_s *bp = (boot_param_s*)g_bootparam;
    bp->magic = BOOT_PARAM_MAGIC;
    bp->lenth = sizeof(boot_param_s);
    
    bp->version = BOOT_VERSION;
    bp->debug_mode = 0;
    bp->pad1[0] = 0xaa;

    bp->flush_num = g_param_write_num;
    
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
int32_t check_boot_param(boot_param_s *btprm)
{
    
    boot_param_s *bp = btprm;
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
    if(bp->crc != calc_crc32((char*)bp,sizeof(boot_param_s) - sizeof(bp->crc),0))
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
void clear_boot_param_buffer(void)
{
    int32_t i;
    for(i = 0;i < sizeof(g_bootparam);i ++)
    {
        g_bootparam[i] = 0;
    }
    g_pbp = NULL;
}



int32_t read_param(void)
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
        ret = check_boot_param((boot_param_s *)g_bootparam);
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

int32_t write_param(void)
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

int32_t check_app_debug_mode(void)
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

