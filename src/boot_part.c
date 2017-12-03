#include "boot_hw_if.h"
#include "boot_part.h"
#include "mem_map.h"
#include "sys_debug.h"
#include "wind_string.h"
#include "mem_driver.h"

//检查定义的分区之间是否存在冲突
static w_int32_t do_check_conflict(part_s *entry1,part_s *entry2)
{
    if(entry1->memidx != entry2->memidx)
        return -1;
    if(entry1->addr == entry2->addr)
        return -1;
    if(entry1->addr < entry2->addr)
    {
        if(entry1->addr + entry1->size > entry2->addr)
        {
            return -1;
        }
    }
    if(entry2->addr < entry1->addr)
    {
        if(entry2->addr + entry2->size > entry1->addr)
        {
            return -1;
        }
    }
    return 0;
}

w_int32_t part_check_conflict(void)
{
    w_int32_t i,j;
    
    int count = part_get_count();
    part_s *part = part_get_list();
    
    for(i = 0;i < count - 1;i ++)
    {
        for(j = i + 1;j < count;j ++)
        {
            if(do_check_conflict(&part[i],&part[j]))
            {
                sys_warn("map %d region \"%s\" and \"%s\" conflict.",i+1,part[i].name,part[j].name);
                return -1;
            }
        }
    }
    return 0;
}

extern phymem_s g_phymem[];
phymem_s *phymem_get_list(void)
{
    return g_phymem;
}



phymem_s *phymem_get_instance(w_int32_t idx)
{
    if(idx >= phymem_get_count())
        return (phymem_s *)NULL;
    return &g_phymem[idx];
}

extern part_s g_region_map[];
#if 0
static void set_part(char *name,w_int32_t pidx,w_int32_t midx,w_uint32_t addr,w_int32_t size)
{
    part_s *pt = g_region_map;
    wind_strcpy(pt[pidx],name);
    pt[pidx].memidx = midx;
    pt[pidx].addr = addr;
    pt[pidx].size = size;
    return size;
}

void part_init(void)
{
    w_int32_t partid = 0;
    w_int32_t base = mem_get_base(0);
    base += set_part("boot",partid++,0,base,BOOT_PROGRAM_SIZE);
    base += set_part("btpara1",partid++,0,base,BOOT_PARAM1_SIZE);
    base += set_part("btpara2",partid++,0,base,BOOT_PARAM2_SIZE);
    base += set_part("img1",partid++,0,base,SYS_PROGRAM1_SIZE);
    base += set_part("img2",partid++,0,base,SYS_PROGRAM2_SIZE);
    base = get_ram_base(0);
    base += 0x10000;
    base += set_part("cache",partid++,0,base,BOOT_PARAM2_SIZE);
    base += set_part("btpara2",partid++,0,base,BOOT_PARAM2_SIZE);
}
#endif

part_s *part_get_inst_name(char *name)
{
    w_int32_t i;
    part_s *rety;
    w_int32_t count = part_get_count();
    for(i = 0;i < count;i ++)
    {
        rety = &g_region_map[i];
        if(wind_strcmp(rety->name,name) == 0)
            return rety;
    }
    sys_error("find region %s failed.",name);
    return NULL;
}

part_s *part_get_inst_idx(w_int16_t memidx)
{
    if(memidx >= part_get_count())
        return NULL;
    return &g_region_map[memidx];
}



part_s *part_get_list(void)
{
    return g_region_map;
}

void part_print_detail(void)
{
#define REGION_FORMAT "%-15s%-8d0x%-12x0x%-12x\r\n" 
#define REGION_PARAM(reg) (part)->name,(part)->memidx,(part)->addr,(part)->size,(part)->name
    w_int32_t i,count;
    part_s *part = part_get_list();
    wind_printf("memory part details:\r\n");
    wind_printf("%-15s%-8s%-14s%-14s\r\n","part","memidx","addr","size");
    part = part_get_list();
    count = part_get_count();
    for(i = 0;i < count;i ++)
    {
        wind_printf(REGION_FORMAT,REGION_PARAM((region_s*)&part[i]));
    }
    wind_printf("\r\n");
}


