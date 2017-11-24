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
#include "boot_config.h" 
#include "boot_param.h"
#include "boot_port.h"
#include "sys_debug.h"
#include "wind_crc32.h"
#include "mem_driver.h"

#ifdef __cplusplus
extern "C" {
#endif


boot_param_s *g_pbp = NULL;
static w_uint8_t g_bootparam[BOOT_PARAM1_SIZE];


static void upate_bootparam_crc(w_uint8_t *prmbuf)
{
    w_uint32_t *crc = (w_uint32_t*)&prmbuf[sizeof(boot_param_s)];
    *crc = wind_crc32(0xffffffff,prmbuf,sizeof(boot_param_s));
}

void *get_boot_params(void)
{
    if(NULL == g_pbp)
        g_pbp = (boot_param_s*)get_boot_params_from_ROM();
    return (void *)g_pbp;
}

void *get_boot_params_from_ROM(void)
{
    w_int32_t i,ret;
    mem_map_s *map;
    region_s *src,*dest;
    ret = param_read();
    if(0 != ret)
    {
        sys_warn("get boot params failed.");
        g_pbp = NULL;
        return NULL;
    }
    map = get_memory_map();
    g_pbp = (boot_param_s *)&g_bootparam;
    src = (region_s*)map;
    dest = (region_s*)&g_pbp->mem_map;
    for(i = 0;i < sizeof(g_pbp->mem_map)/sizeof(region_s);i ++)
    {
        dest[i].regname = src[i].regname;
        dest[i].addr = src[i].addr;
    }
    
    return (void*)g_pbp;
}


void param_init(void)
{
    w_int32_t i;
    char *src,*dest;
    mem_map_s *map = get_memory_map();
    boot_param_s *bp = (boot_param_s*)g_bootparam;
    print_map_info(map);
    bp->magic = BOOT_PARAM_MAGIC;
    bp->lenth = sizeof(boot_param_s);

    bp->version = BOOT_VERSION;
    bp->debug_mode = 0;
    bp->wait_sec = WAIT_KEY_SEC;
    bp->run_type = RUN_SPACE_TYPE;
    bp->encrypt_type = ENCRYPT_TYPE;
    bp->lock_en = MCU_LOCK_ENABLE;
    
    src = (char*)map;
    dest = (char*)&bp->mem_map;
    for(i = 0;i < sizeof(mem_map_s);i ++)
    {
        dest[i] = src[i];
    }
    init_map_info(&bp->mem_map);
    sys_notice("init boot param OK.");
}


//检查参数是否有效，有效返回1，无效返回0
w_int32_t param_check_valid(w_uint8_t *prmbuf)
{
    boot_param_s *bp = (boot_param_s *)prmbuf;
    w_uint32_t *crc = (w_uint32_t*)&prmbuf[sizeof(boot_param_s)];
    if(bp->magic != BOOT_PARAM_MAGIC)
    {
        sys_warn("param block is invalid.");
        return -1;
    }
    if(bp->lenth!= sizeof(boot_param_s))
    {
        sys_warn("param block lenth is invalid.");
        return -1;
    }
    if(*crc != wind_crc32(0xffffffff,(char*)bp,sizeof(boot_param_s)))
    {
        sys_warn("param block crc is invalid.");
        return -1;
    }
    if(bp->version > BOOT_VERSION)
    {
        sys_warn("param block version is not matched.");
        return -1;
    }
    return 0;
}

void param_clear_buffer(void)
{
    w_int32_t i;
    for(i = 0;i < sizeof(g_bootparam);i ++)
    {
        g_bootparam[i] = 0;
    }
    g_pbp = NULL;
}

w_int32_t param_read(void)
{
    w_uint32_t err = 0;
    w_int32_t i,j,len,ret;
    region_s *reg[2];
    mem_map_s *map = get_memory_map();
    reg[0] = &map->rom.boot_param1;
    reg[1] = &map->rom.boot_param2;
    
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
            sys_warn("read param %d fail.",i + 1);
            err ++;
        }
    }
    if(err >= 2)
    {
        sys_warn("read both params failed.");
        return -1;
    }
    return 0;
    
}

w_int32_t param_flush(void)
{
    w_int32_t i,j,len,err = 0;
    region_s *reg[2];
    boot_param_s *bp = (boot_param_s *)g_bootparam;
    mem_map_s *map = get_memory_map();
    reg[0] = &map->rom.boot_param1;
    reg[1] = &map->rom.boot_param2;
    
    bp->mem_map.rom.boot_param1.datalen = sizeof(boot_param_s);
    bp->mem_map.rom.boot_param2.datalen = sizeof(boot_param_s);    
    upate_bootparam_crc((w_uint8_t*)bp);
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
            
            sys_warn("write param %d fail.",i + 1);
            err ++;
        }
        sys_debug("write param %d OK.",i + 1);
    }
    if(err >= 2)
    {
        sys_warn("write both params failed.");
        return -1;
    }
    sys_notice("write boot param complete.");
    return 0;
}

w_int32_t param_check_debug_mode(void)
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

