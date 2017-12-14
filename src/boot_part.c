#include "boot_hw_if.h"
#include "boot_part.h"
#include "mem_map.h"
#include "wind_debug.h"
#include "wind_string.h"
#include "mem_driver.h"
#include "wind_debug.h"
#include "phy_mem.h"
#include "boot_param.h"

static part_s *get_part(void)
{
    
    boot_param_s *bp = boot_param_instance();
    return (part_s*)(sizeof(boot_param_s)+sizeof(phymem_s)+bp->phymem_cnt + (w_uint32_t)bp);
}
static w_int32_t get_part_count(void)
{
    boot_param_s *bp = boot_param_instance();
    return bp->part_cnt;
}

static void set_part_count(w_int32_t count)
{
    boot_param_s *bp = boot_param_instance();
    bp->part_cnt = count;
}



w_int32_t part_init_all(part_s *pt)
{
    wind_memset(pt,0,PART_COUNT*sizeof(part_s));
    parts_create();
    return 0;
}

w_bool_t part_create(char *name,w_int8_t midx,w_int32_t size)
{
    phymem_s *pm;
    part_s *pt;
    w_int32_t ptcnt;
    w_int32_t pmcnt = phymem_get_count();
    if(midx >= pmcnt)
    {
        wind_error("memidx out of range.");
        return B_FALSE;
    }
        
    pm = phymem_get_list();
    if(pm[midx].size - pm[midx].used < size)
    {
        wind_error("has no enough space to create part.");
        return B_FALSE;
    }
    ptcnt = part_get_count();
    if(ptcnt >= PART_COUNT)
    {
        wind_error("part idx out of range.");
        return B_FALSE;
    }
    pt = get_part();
    wind_strncpy(pt[ptcnt].name,name,PART_NAME_LEN);
    pt[ptcnt].memidx = midx;
    pt[ptcnt].memtype = pm[midx].type;
    pt[ptcnt].addr = pm[midx].base + pm[midx].used;
    pt[ptcnt].size = size;
    ptcnt ++;
    set_part_count(ptcnt);
    pm[midx].used += size;
    return B_TRUE;
}

w_int32_t part_get_count(void)
{
    return get_part_count();
}


part_s *part_get_inst_name(char *name)
{
    w_int32_t i;
    part_s *pt;
    w_int32_t count = part_get_count();
    pt = get_part();
    for(i = 0;i < count;i ++)
    {
        if(wind_strcmp(pt[i].name,name) == 0)
            return pt;
    }
    wind_error("find region %s failed.",name);
    return NULL;
}

part_s *part_get_inst_idx(w_int8_t memidx)
{
    part_s *pt;
    if(memidx >= part_get_count())
        return NULL;
    pt = get_part();
    return &pt[memidx];
}

part_s *part_get_list(void)
{
    return get_part();
}

void part_print_detail(void)
{
#define REGION_FORMAT "%-12s%-8d0x%-12x0x%-12x\r\n" 
#define REGION_PARAM(pt) (pt).name,(pt).memidx,(pt).addr,(pt).size,(pt).name
    w_int32_t i,count;
    part_s *pt = part_get_list();
    wind_printf("memory pt details:\r\n");
    wind_printf("----------------------------------------------\r\n");
    wind_printf("%-12s%-8s%-14s%-14s\r\n","pt","memidx","addr","size");
    wind_printf("----------------------------------------------\r\n");
    //pt = part_get_list();
    count = part_get_count();
    for(i = 0;i < count;i ++)
    {
        wind_printf(REGION_FORMAT,REGION_PARAM(pt[i]));
    }
    wind_printf("----------------------------------------------\r\n");
    wind_printf("\r\n");
}


