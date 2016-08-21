/***************************************************************************************
** 文件名：
** 文件信息：
** 创建时间：2015.10.13
** 创建人：周江村
**文件版本1.0
** 修改记录：
***************************************************************************************/
#include "boot_config.h"
#include "mem_map.h"
#include "port.h"
#include "share_param.h"
#include "boot_debug.h"
#include "boot_param.h"
#include "mem_driver.h"
#ifdef __cplusplus
extern "C" {
#endif

mem_map_s g_memmap[] = 
{
    //第一份映射表
    {
        {
            {"boot_rom",0,0,IFLASH,0,0,0},
            {"boot_param1",0,0,IFLASH,0,0,0},
            {"boot_param2",0,0,IFLASH,0,0,0},
            {"app_program1",0,0,IFLASH,0,0,0},
            {"app_program2",0,0,IFLASH,0,0,0},
            {"app_product",0,0,IFLASH,0,0,0},
            {"reserved",0,0,IFLASH,0,0,0}
        },
        {
            {"boot_ram",0,0,IRAM,0,0,0},
            {"code_buffer",0,0,IRAM,0,0,0},
            {"Share_ram",0,0,IRAM,0,0}
        },
        {
            {"Run_iflash",0,0,IFLASH,0,0,0},
            {"Run_iram",0,0,IRAM,0,0,0},
            {"Run_xram",0,0,XRAM,0,0,0}        
        }
    },
    //第二份映射表
    {
        {
            {"boot_rom",0,0,IFLASH,0,0,0},
            {"boot_param1",0,0,IFLASH,0,0,0},
            {"boot_param2",0,0,IFLASH,0,0,0},
            {"app_program1",0,0,XFLASH,0,0},
            {"app_program2",0,0,XFLASH,0,0},
            {"app_product",0,0,XFLASH,0,0},
            {"reserved",0,0,IFLASH,0,0,0}
        },
        {
            {"boot_ram",0,0,IRAM,0,0,0},
            {"code_buffer",0,0,XRAM,0,0,0},
            {"Share_ram",0,0,IRAM,0,0}
        },
        {
            {"Run_iflash",0,0,IFLASH,0,0,0},
            {"Run_iram",0,0,IRAM,0,0,0},
            {"Run_xram",0,0,XRAM,0,0,0}        
        }
    }
};

const char *sys_mem_name[] = 
{
    "boot_rom",
    "boot_param1",
    "boot_param2",
    "app_program1",
    "app_program2",
    "app_product",
    "reserved",
    "boot_ram",
    "code_buffer",
    "Share_ram",
    "Run_ifalsh",
    "Run_iram",
    "Run_xram"
};
mem_map_s *g_default_map = (mem_map_s *)NULL;

const char *sys_memtype[] = 
{
    "IRAM",
    "XRAM",
    "IFLASH",
    "XFLASH",    
};

uint32_t get_share_addr (void)
{
	uint32_t base;
	base = get_iram_base();
	return base + IRAM_LENTH - BOOT_SHARE_PARAM_LENTH;
}

mem_map_s *get_default_map(void)
{
    return g_default_map;
}

int32_t set_default_map(int32_t index)
{
    int32_t num = sizeof(g_memmap)/sizeof(mem_map_s);
    if(index >= 0 && index < num)
    {
        g_default_map = &g_memmap[index];
        init_boot_param(g_default_map);
        return 0;
    }
    return -1;
}

void choose_default_map(void)
{
    int32_t index;
    while(1)
    {
        printk_rt("please choose map:\r\n");
        print32_t_map_list();
        if(XFLASH_EXIST)
        {
            index = 1;
        }
        else
        {
            index = 0;
        }
        set_default_map(index);
        
        boot_notice("system chooses default map No:[%d]",index+1);
        break;
    }
}


//检查定义的区域之间是否存在冲突
static int32_t check_region_conflict(region_s *reg1,region_s *reg2)
{
    if(reg1->type != reg2->type)
        return 0;
    if(reg1->base == reg2->base)
    {
        return -1;
    }
    else if(reg1->base < reg2->base)
    {
        if(reg1->base + reg1->maxlen > reg2->base)
        {
            return -1;
        }
    }
    else if(reg2->base < reg1->base)
    {
        if(reg2->base + reg2->maxlen > reg1->base)
        {
            return -1;
        }
    }
    return 0;
}

static uint32_t alloc_region(region_s * reg,uint32_t base,uint32_t size)
{
	reg->base = base;
	reg->maxlen = size;
	return size;
}

int32_t mem_region_init(void)
{
	uint32_t base;
	uint32_t len;

	mem_map_s *map = &g_memmap[0];
	base = 0;
	base += alloc_region(&map->rom.boot_region,base,BOOT_PROGRAM_LENTH);    
	base += alloc_region(&map->rom.param1_region,base,BOOT_PARAM_LENTH);
	base += alloc_region(&map->rom.param2_region,base,BOOT_PARAM_LENTH);
	len = IFLASH_LENTH - base - BOOT_RESERVED_LENTH - BOOT_PRODUCT_LENTH;
	len /= 2;
	base += alloc_region(&map->rom.program1_region,base,len);
	base += alloc_region(&map->rom.programbak_region,base,len);
	base += alloc_region(&map->rom.product_region,base,BOOT_PRODUCT_LENTH);
	base += alloc_region(&map->rom.reserve_region,base,BOOT_RESERVED_LENTH);    
	base = get_iram_base();
	len = (IRAM_LENTH - BOOT_PROGRAMBUF_LENTH - BOOT_SHARE_PARAM_LENTH);
	base += alloc_region(&map->ram.app_region,base,len);
	base += alloc_region(&map->ram.probuf_region,base,BOOT_PROGRAMBUF_LENTH);
	base += alloc_region(&map->ram.share_region,base,BOOT_SHARE_PARAM_LENTH);
	alloc_region(&map->run.iflash,map->rom.program1_region.base,map->rom.program1_region.maxlen);
    


	map = &g_memmap[1];
	base = 0;
	base += alloc_region(&map->rom.boot_region,base,BOOT_PROGRAM_LENTH);    
	base += alloc_region(&map->rom.param1_region,base,BOOT_PARAM_LENTH);
	base += alloc_region(&map->rom.param2_region,base,BOOT_PARAM_LENTH);
	len = IFLASH_LENTH - base - BOOT_RESERVED_LENTH;
	base += alloc_region(&map->run.iflash,base,len);
	base += alloc_region(&map->rom.reserve_region,base,BOOT_RESERVED_LENTH);

    base = 0;
    base += alloc_region(&map->rom.program1_region,base,BOOT2_PROGRAM1_LENTH);
    base += alloc_region(&map->rom.programbak_region,base,BOOT2_PROGRAMBAK_LENTH);
    base += alloc_region(&map->rom.product_region,base,BOOT2_PRODUCT_LENTH);

    base = get_iram_base();
	base += alloc_region(&map->ram.app_region,base,len);
	base += alloc_region(&map->ram.probuf_region,base,BOOT_PROGRAMBUF_LENTH);
	base += alloc_region(&map->ram.share_region,base,BOOT_SHARE_PARAM_LENTH);

	return 0;
}

static int32_t check_rom_conflict(rom_map_s *rom)
{
    int32_t i,j,size;
    int32_t ret = 0;
    region_s *reg;
    size = sizeof(rom_map_s)/sizeof(region_s);
    reg = (region_s*)rom;
    for(i = 0;i < size - 1;i ++)
    {
        for(j = i + 1;j < size;j ++)
        {
            if(check_region_conflict(&reg[i],&reg[j]))
            {
                boot_warn("map %d region \"%s\" and \"%s\" conflict.",i+1,reg[i].regname,reg[j].regname);
                ret = -1;
            }
        }
    }
    return ret;
}

static int32_t check_rom_type(rom_map_s *rom)
{
    int32_t i,size;
    region_s *reg;
    size = sizeof(rom_map_s)/sizeof(region_s);
    reg = (region_s*)rom;
    
    for(i = 0;i < size;i ++)
    {
        if(IFLASH != reg[i].type && XFLASH != reg[i].type)
            return -1;
    }
    return 0;
}

static int32_t check_ram_type(ram_map_s *ram)
{
    int32_t i,size;
    region_s *reg;
    size = sizeof(ram_map_s)/sizeof(region_s);
    reg = (region_s*)ram;
    
    for(i = 0;i < size;i ++)
    {
        if(IRAM != reg[i].type && XRAM != reg[i].type)
            return -1;
    }
    return 0;
}



static int32_t check_ram_conflict(ram_map_s *ram)
{
    int32_t i,j,size;
    region_s *reg;
    size = sizeof(ram_map_s)/sizeof(region_s);
    reg = (region_s*)ram;
    for(i = 0;i < size - 1;i ++)
    {
        for(j = i + 1;j < size;j ++)
        {
            if(check_region_conflict(&reg[i],&reg[j]))
            {
                boot_warn("map %d region \"%s\" and \"%s\" conflict.",i+1,reg[i].regname,reg[j].regname);
                return -1;
            }
        }
    }
    return 0;
}

static int32_t check_run_type(run_map_s *run)
{
    if(run->iflash.type != IFLASH)
        return -1;
    if(run->iram.type != IRAM)
        return -1;
    if(run->xram.type != XRAM)
        return -1;
    return 0;
}

int32_t check_probuf_and_running(region_s *probuf,region_s *run)
{
    if(probuf->type != run->type)
        return 0;
    if(0 == check_region_conflict(probuf,run))
        return 0;
    if(probuf->base >= run->base)
        return 0;
    return -1;
}
int32_t check_map_valid(void)
{
    int32_t i;
    mem_map_s *map;
    
    boot_notice("begin to check momery map params...");
    for(i = 0;i < sizeof(g_memmap)/sizeof(mem_map_s);i ++)
    {
        map = &g_memmap[i];
        if(check_rom_conflict(&map->rom))
        {
            boot_warn("check rom conflict error.");
            return -1;
        }
        if(check_ram_conflict(&map->ram))
        {
            boot_warn("check ram conflict error.");
            return -1;
        }
        if(check_rom_type(&map->rom))
        {
            boot_warn("check rom type error.");
            return -1;
        }
        if(check_ram_type(&map->ram))
        {
            boot_warn("check ram type error.");
            return -1;
        }
        if(check_run_type(&map->run))
        {
            boot_warn("check running space type error.");
            return -1;
        }
        if(check_probuf_and_running(&map->ram.probuf_region,&map->run.iflash))
        {
            boot_warn("program buffer and running space conflict.");
            return -1;
        }
        
    }
    boot_notice("check momery map params OK.");
    return 0;   
}


void print32_t_map_info(mem_map_s *map)
{
#define REGION_FORMAT "%-15s0x%-12x0x%-12x%s\r\n" 
#define REGION_PARAM(reg) (reg)->regname,(reg)->base,(reg)->maxlen,sys_memtype[(reg)->type]
    int32_t i;
    region_s *reg;
    reg = (region_s*)map;
    printk_rt("%-15s%-14s%-14s%-12s\r\n","area","base","maxlen","type");
    for(i = 0;i < sizeof(mem_map_s)/sizeof(region_s);i ++)
    {
        printk_rt(REGION_FORMAT,REGION_PARAM((region_s*)&reg[i]));
    }
}

void print32_t_program_space(mem_map_s *map)
{
#define REGION_FORMAT1 "%-15s0x%-10x0x%-9x0x%-9x%-9s%4d%%\r\n" 
#define REGION_PARAM1(reg) (reg)->regname,(reg)->base,(reg)->maxlen,\
                        (reg)->lenth,sys_memtype[(reg)->type],((reg)->lenth*100)/(reg)->maxlen
        printk_rt("%-15s%-12s%-11s%-11s%-9s%-8s\r\n","area","base","maxlen","lenth","type","usage");
        printk_rt(REGION_FORMAT1,REGION_PARAM1(&map->rom.program1_region));
        printk_rt(REGION_FORMAT1,REGION_PARAM1(&map->rom.programbak_region));
        printk_rt(REGION_FORMAT1,REGION_PARAM1(&map->rom.product_region));
        printk_rt(REGION_FORMAT1,REGION_PARAM1(&map->run.iflash));
}


void print32_t_map_list(void)
{
    int32_t i;
    boot_param_s *bp = (boot_param_s*)sys_boot_params();
    for(i = 0;i < sizeof(g_memmap)/sizeof(mem_map_s);i ++)
    {
        printk_rt("\r\n[%d] memory map:\r\n",i+1);
        print32_t_map_info(&g_memmap[i]);
    }
}

mem_map_s *get_map_by_index(int32_t id)
{
    if(id >= 0 && id < sizeof(g_memmap)/sizeof(mem_map_s))
    {
        return &g_memmap[id];
    }
    return (mem_map_s *)NULL;
}

void copy_region_info(region_s *src,region_s *dest)
{
    dest->regname = src->regname;
    dest->base = src->base;
    dest->lenth = src->lenth;
    dest->type = src->type;
    dest->crc = src->crc;
    dest->status = src->status;
}


#ifdef __cplusplus
}
#endif
