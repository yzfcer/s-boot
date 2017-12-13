#include "boot_hw_if.h"
#include "boot_part.h"
#include "mem_map.h"
#include "wind_debug.h"
#include "wind_string.h"
#include "mem_driver.h"
#include "wind_debug.h"

static w_int32_t pmidx = 0;
static phymem_s g_phymem[PHYMEM_COUNT];
static w_int32_t ptidx = 0;
static part_s g_part[PART_COUNT];


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



w_bool_t phymem_register(w_int16_t mtype,w_uint32_t base,w_int32_t size)
{
    phymem_s *pm;
    if(pmidx >= PHYMEM_COUNT)
    {
        wind_error("memidx out of range.");
        return B_FALSE;
    }
    pm = g_phymem;
    pm[pmidx].memidx = pmidx;
    pm[pmidx].type = mtype;
    pm[pmidx].base = base;
    pm[pmidx].size = size;
    pm[pmidx].used = 0;
    pmidx ++;
    return B_TRUE;
}


w_int32_t phymem_get_count(void)
{
    return pmidx;
}
static char * get_mem_type(w_int16_t type)
{
    switch(type)
    {
        case MEM_TYPE_RAM:return "RAM";
        case MEM_TYPE_ROM:return "ROM";
        default:return "none";
    }
}
void phymem_print_detail(void)
{
#define PHY_FORMAT "%-8d%-8s0x%-12x0x%-12X\r\n" 
#define PHY_PARAM(mem) (mem).memidx,get_mem_type((mem).type),(mem).base,(mem).size
        w_int32_t i,count;
        phymem_s *pm = phymem_get_list();
        wind_printf("physical memory details:\r\n");
        wind_printf("------------------------------------------\r\n");
        wind_printf("%-8s%-8s%-14s%-14s\r\n","index","type","base","size");
        wind_printf("------------------------------------------\r\n");
        //pm = phymem_get_list();
        count = phymem_get_count();
        for(i = 0;i < count;i ++)
        {
            wind_printf(PHY_FORMAT,PHY_PARAM(pm[i]));
        }
        wind_printf("------------------------------------------\r\n");
        wind_printf("\r\n");
}


w_bool_t part_create(char *name,w_int16_t midx,w_int32_t size)
{
    phymem_s *pm;
    part_s *pt;
    if(midx >= pmidx)
    {
        wind_error("memidx out of range.");
        return B_FALSE;
    }
        
    pm = g_phymem;
    if(pm[midx].size - pm[midx].used < size)
    {
        wind_error("has no enough space to create part.");
        return B_FALSE;
    }
        
    if(ptidx >= PART_COUNT)
    {
        wind_error("part idx out of range.");
        return B_FALSE;
    }
    pt = g_part;
    wind_strncpy(pt[ptidx].name,name,PART_NAME_LEN);
    pt[ptidx].memidx = midx;
    pt[ptidx].memtype = pm[midx].type;
    pt[ptidx].addr = pm[midx].base + pm[midx].used;
    pt[ptidx].size = size;
    ptidx ++;
    pm[midx].used += size;
    return B_TRUE;
}

w_int32_t part_get_count(void)
{
    return ptidx;
}


part_s *part_get_inst_name(char *name)
{
    w_int32_t i;
    part_s *pt;
    w_int32_t count = part_get_count();
    for(i = 0;i < count;i ++)
    {
        pt = &g_part[i];
        if(wind_strcmp(pt->name,name) == 0)
            return pt;
    }
    wind_error("find region %s failed.",name);
    return NULL;
}

part_s *part_get_inst_idx(w_int16_t memidx)
{
    if(memidx >= part_get_count())
        return NULL;
    return &g_part[memidx];
}



part_s *part_get_list(void)
{
    return g_part;
}

void part_print_detail(void)
{
#define REGION_FORMAT "%-15s%-8d0x%-12x0x%-12x\r\n" 
#define REGION_PARAM(part) (part).name,(part).memidx,(part).addr,(part).size,(part).name
    w_int32_t i,count;
    part_s *part = part_get_list();
    wind_printf("memory part details:\r\n");
    wind_printf("------------------------------------------\r\n");
    wind_printf("%-15s%-8s%-14s%-14s\r\n","part","memidx","addr","size");
    wind_printf("------------------------------------------\r\n");
    //part = part_get_list();
    count = part_get_count();
    for(i = 0;i < count;i ++)
    {
        wind_printf(REGION_FORMAT,REGION_PARAM(part[i]));
    }
    wind_printf("------------------------------------------\r\n");
    wind_printf("\r\n");
}


