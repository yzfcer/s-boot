#include "boot_hw_if.h"
#include "boot_part.h"
#include "wind_debug.h"
#include "wind_string.h"
#include "mem_driver.h"
#include "wind_debug.h"
#include "phy_mem.h"
#include "boot_param.h"
#include "boot_config.h"
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
    //parts_create();
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

w_uint32_t part_share_addr(void)
{
    part_s *entry = part_get_inst_name("share");
	return entry->addr;
}


void part_print_status(void)
{
#define REGION_FORMAT1 "%-15s%-8d0x%-10x0x%-9x0x%-9x%-9s%4d%%\r\n" 
#define REGION_PARAM1(reg) (reg).name,(reg).memidx,(reg).addr,(reg).size,\
                (reg).datalen,(reg).name,(reg).size?((reg).datalen*100)/(reg).size:0
    w_int32_t i;
    w_int32_t count = part_get_count();
    part_s *reg = part_get_list();

    wind_printf("%-15s%-8s%-12s%-11s%-11s%-9s%-8s\r\n","region","memidx","addr","size","datalen","type","usage");
    for(i = 0;i < count;i ++)
    {
        wind_printf(REGION_FORMAT1,REGION_PARAM1(reg[i]));
    }
}

void part_copy_info(part_s *src,part_s *dest)
{
	wind_strcpy(dest->name,src->name);
    dest->memtype = src->memtype;
    dest->memidx = src->memidx;
    dest->addr = src->addr;
    dest->size = src->size;
    dest->datalen = src->datalen;
    dest->crc = src->crc;
    dest->status = src->status;
}
static void print_copy_percents(w_int32_t numerator, w_int32_t denominator,w_int32_t del)
{
    if(del)
        wind_printf("%c%c%c%c",8,8,8,8);
    wind_printf("%3d%%",numerator*100/denominator);
        feed_watchdog();
}

w_int32_t part_copy_data(part_s *src,part_s *dest)
{
    w_int32_t i,j,len,blocks,times;
    w_uint32_t addr;
    w_uint8_t *buff = get_block_buffer();

    if(0 >= src->datalen)
        return 0;
    if(dest->size < src->datalen)
    {
        wind_warn("space is NOT enough.");
        return -1;
    }
    wind_notice("copy data from \"%s\" to \"%s\" lenth %d.",
                src->name,dest->name,src->datalen);
    wind_debug("source type %s,addr 0x%x,lenth %d dest type,%s,addr 0x%x,lenth %d.",
                phymem_type(src->memtype),src->addr,src->datalen,
                phymem_type(dest->memtype),dest->addr,dest->size);
    
    blocks = (src->datalen + BLOCK_SIZE - 1) / BLOCK_SIZE;
    wind_printf("complete:");
    print_copy_percents(0,1,0);
    for(i = 0;i < blocks;i ++)
    {    
        for(times = 0;times < 3;times ++)
        {
            addr = src->addr + i * BLOCK_SIZE;
            if(i >= blocks - 1)
            {
                for(j = 0;j < BLOCK_SIZE;j ++)
                    buff[j] = 0;
            }
            len = read_block(src->memtype,src->memidx,addr,buff,1);
            if(len > 0)
                break;
        }
        if(times >= 3)
        {
            wind_warn("read block 0x%x,lenth %d failed.",addr,BLOCK_SIZE);
            dest->status = MEM_ERROR;
            return -1;
        }

        for(times = 0;times < 3;times ++)
        {
            addr = dest->addr + i * BLOCK_SIZE;
            len = write_block(dest->memtype,dest->memidx,addr,buff,1);
            if(len > 0)
                break;
        }
        if(times >= 3)
        {
            wind_warn("read block 0x%x,lenth %d failed.",addr,BLOCK_SIZE);
            dest->status = MEM_ERROR;
            return -1;
        }
        print_copy_percents(i,blocks,1);
        feed_watchdog();
    }
    print_copy_percents(i,blocks,1);
    wind_printf("\r\n");

    dest->datalen = src->datalen;
    dest->crc = src->crc;
    dest->status = MEM_NORMAL;

    wind_debug("copy data OK."); 
    return 0;    
}



