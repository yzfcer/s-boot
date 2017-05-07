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
#include "mem_map.h"
#include "port.h"
#include "share_param.h"
#include "sys_debug.h"
#include "boot_param.h"
#include "mem_driver.h"
#ifdef __cplusplus
extern "C" {
#endif
#define SET_REG(reg,idx,base,title,memtype) do{\
                    reg.regname = g_reg_name[idx];\
                    reg.idx = title##_IDX;\
                    reg.addr = (base[title##_IDX]) + title##_ADDR;\
                    reg.size = title##_SIZE;\
                    reg.type = MEM_TYPE_##memtype;\
                    idx ++;\
                    }while(0)

mem_map_s g_memmap;
char *g_reg_name[] = 
{
    "boot_program",
    "boot_param1",
    "boot_param2",
    "sys_program1",
    "sys_program2",
    "sys_param",
    "sys_romfs",
    
    "data_ram",
    "load_buffer",
    "share_param",
    
    "run_ifalsh",
    "run_iram",
};

char *g_memtype_name[] = 
{
    "RAM",
    "ROM",
};

uint32_t get_share_addr (void)
{
    mem_map_s *map = get_memory_map();
	return map->ram.share_param.addr;
}

char *memtype_name(uint32_t type)
{
    return g_memtype_name[type];
}
char *region_name(uint32_t regidx)
{
    return g_reg_name[regidx];
}

mem_map_s *get_memory_map(void)
{
    return &g_memmap;
}


//检查定义的区域之间是否存在冲突
static int32_t check_region_conflict(region_s *reg1,region_s *reg2)
{
    if(reg1->type != reg2->type)
        return 0;
    if(reg1->addr == reg2->addr)
    {
        return -1;
    }
    else if(reg1->addr < reg2->addr)
    {
        if(reg1->addr + reg1->size > reg2->addr)
        {
            return -1;
        }
    }
    else if(reg2->addr < reg1->addr)
    {
        if(reg2->addr + reg2->size > reg1->addr)
        {
            return -1;
        }
    }
    return 0;
}


void init_map_info(mem_map_s *map)
{
    int32_t i;
    region_s *rg = (region_s*)map;
    for(i = 0;i < sizeof(mem_map_s)/sizeof(region_s);i ++)
    {
        rg[i].crc = 0xffffff;
        rg[i].datalen = 0;
        rg[i].status = MEM_NULL;
    }
}

static uint32_t alloc_region(region_s * reg,uint32_t *base,uint32_t size)
{
    uint32_t idx = reg->index;
	reg->addr = base[idx]+reg->addr;
	reg->size = size;
	return size;
}

int32_t mem_region_init(void)
{
    int i;
    int index = 0;
    uint32_t rambase[RAM_COUNT];
    uint32_t rombase[ROM_COUNT];
	mem_map_s *map = &g_memmap;
    for(i = 0;i < RAM_COUNT;i ++)
    {
        rambase[i] = get_ram_base(i);
    }
    for(i = 0;i < ROM_COUNT;i ++)
    {
        rombase[i] = get_rom_base(i);
    }
	SET_REG(map->rom.boot_program,index,rombase,BOOT_PROGRAM,ROM);
	SET_REG(map->rom.boot_param1,index,rombase,BOOT_PARAM1,ROM);
	SET_REG(map->rom.boot_param2,index,rombase,BOOT_PARAM2,ROM);
	SET_REG(map->rom.sys_program1,index,rombase,SYS_PROGRAM1,ROM);
	SET_REG(map->rom.sys_program2,index,rombase,SYS_PROGRAM2,ROM);
	SET_REG(map->rom.sys_param,index,rombase,SYS_PARAM,ROM);   
	SET_REG(map->rom.rom_fs,index,rombase,SYS_ROMFS,ROM);   
    
	SET_REG(map->ram.data_ram,index,rambase,DATA_RAM,RAM);
	SET_REG(map->ram.load_buffer,index,rambase,SYS_LOADBUF,RAM);
	SET_REG(map->ram.share_param,index,rambase,SYS_SHAREPRM,RAM);

    SET_REG(map->run.flash,index,rombase,SYS_ROMRUN,ROM);
	SET_REG(map->run.ram,index,rambase,SYS_RAMRUN,RAM);
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
                sys_warn("map %d region \"%s\" and \"%s\" conflict.",i+1,reg[i].regname,reg[j].regname);
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
        if(MEM_TYPE_ROM != reg[i].type && MEM_TYPE_ROM != reg[i].type)
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
        if(MEM_TYPE_RAM != reg[i].type)
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
                sys_warn("map %d region \"%s\" and \"%s\" conflict.",i+1,reg[i].regname,reg[j].regname);
                return -1;
            }
        }
    }
    return 0;
}

static int32_t check_run_type(run_map_s *run)
{
    if(run->flash.type != MEM_TYPE_ROM)
        return -1;
    if(run->ram.type != MEM_TYPE_RAM)
        return -1;
    return 0;
}

int32_t check_probuf_and_running(region_s *probuf,region_s *run)
{
    if(probuf->type != run->type)
        return 0;
    if(0 == check_region_conflict(probuf,run))
        return 0;
    if(probuf->addr >= run->addr)
        return 0;
    return -1;
}
int32_t check_map_valid(void)
{
    mem_map_s *map;
    sys_notice("begin to check momery map params...");
    map = &g_memmap;
    if(check_rom_conflict(&map->rom))
    {
        sys_warn("check rom conflict error.");
        return -1;
    }
    if(check_ram_conflict(&map->ram))
    {
        sys_warn("check ram conflict error.");
        return -1;
    }
    if(check_rom_type(&map->rom))
    {
        sys_warn("check rom type error.");
        return -1;
    }
    if(check_ram_type(&map->ram))
    {
        sys_warn("check ram type error.");
        return -1;
    }
    if(check_run_type(&map->run))
    {
        sys_warn("check running space type error.");
        return -1;
    }
    if(check_probuf_and_running(&map->ram.load_buffer,&map->run.flash))
    {
        sys_warn("program buffer and running space conflict.");
        return -1;
    }
    sys_notice("check momery map params OK.");
    return 0;   
}


void print_map_info(mem_map_s *map)
{
#define REGION_FORMAT "%-15s%-8d0x%-12x0x%-12x%s\r\n" 
#define REGION_PARAM(reg) (reg)->regname,(reg)->index,(reg)->addr,(reg)->size,memtype_name((reg)->type)
    int32_t i;
    region_s *reg;
    sys_printf("memory map as following:\r\n");
    sys_printf("%-15s%-8s%-14s%-14s%-12s\r\n","region","memidx","addr","size","type");
    reg = (region_s*)&map->rom;
    for(i = 0;i < sizeof(map->rom)/sizeof(region_s);i ++)
    {
        sys_printf(REGION_FORMAT,REGION_PARAM((region_s*)&reg[i]));
    }
    sys_printf("\r\n");
    reg = (region_s*)&map->ram;
    for(i = 0;i < sizeof(map->ram)/sizeof(region_s);i ++)
    {
        sys_printf(REGION_FORMAT,REGION_PARAM((region_s*)&reg[i]));
    }
    sys_printf("\r\n");
    reg = (region_s*)&map->run;
    for(i = 0;i < sizeof(map->run)/sizeof(region_s);i ++)
    {
        sys_printf(REGION_FORMAT,REGION_PARAM((region_s*)&reg[i]));
    }
}

void print_program_space(mem_map_s *map)
{
#define REGION_FORMAT1 "%-15s%-8d0x%-10x0x%-9x0x%-9x%-9s%4d%%\r\n" 
#define REGION_PARAM1(reg) (reg)->regname,(reg)->index,(reg)->addr,(reg)->size,\
                        (reg)->datalen,memtype_name((reg)->type),(reg)->size?((reg)->datalen*100)/(reg)->size:0
        sys_printf("%-15s%-8s%-12s%-11s%-11s%-9s%-8s\r\n","region","memidx","addr","size","datalen","type","usage");
        sys_printf(REGION_FORMAT1,REGION_PARAM1(&map->rom.sys_program1));
        sys_printf(REGION_FORMAT1,REGION_PARAM1(&map->rom.sys_program2));
        sys_printf(REGION_FORMAT1,REGION_PARAM1(&map->run.flash));
}

void copy_region_info(region_s *src,region_s *dest)
{
    dest->regname = src->regname;
    dest->type = src->type;
    dest->index = src->index;
    dest->addr = src->addr;
    dest->size = src->size;
    dest->datalen = src->datalen;
    dest->crc = src->crc;
    dest->status = src->status;
}


#ifdef __cplusplus
}
#endif
