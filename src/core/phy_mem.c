#include "wind_debug.h"
#include "boot_param.h"
#if 0
static w_int32_t pmcnt = 0;
static phymem_s g_phymem[PHYMEM_COUNT];

static phymem_s *get_phymem(void)
{
    //boot_param_s *bp = boot_param_instance();
    //return (phymem_s*)(sizeof(boot_param_s)+(w_uint32_t)bp);
    return g_phymem;
}

static w_int32_t get_phymem_count(void)
{
    //boot_param_s *bp = boot_param_instance();
    //return bp->phymem_cnt;
    return pmcnt;
}


static void set_phymem_count(w_int32_t count)
{
    //boot_param_s *bp = boot_param_instance();
    //bp->phymem_cnt = count;
    pmcnt = count;
}


phymem_s *phymem_get_instance(w_int32_t idx)
{
    phymem_s *pm;
    if(idx >= phymem_get_count())
        return (phymem_s *)W_NULL;
    pm = get_phymem();
    return &pm[idx];
}



w_bool_t phymem_register(w_int16_t mtype,w_uint32_t base,w_int32_t size)
{
    phymem_s *pm;
    w_int32_t pmcnt = get_phymem_count();
    if(pmcnt >= PHYMEM_COUNT)
    {
        wind_error("memidx out of range.");
        return B_FALSE;
    }
    pm = get_phymem();
    pm[pmcnt].memidx = pmcnt;
    pm[pmcnt].type = mtype;
    pm[pmcnt].base = base;
    pm[pmcnt].size = size;
    pm[pmcnt].used = 0;
    pmcnt ++;
    set_phymem_count(pmcnt);
    return B_TRUE;
}


w_int32_t phymem_get_count(void)
{
    return get_phymem_count();
}



#if 0

phymem_s *phymem_get_list(void)
{
    return get_phymem();
}

char * phymem_type(w_int16_t type)
{
    switch(type)
    {
        case MEDIA_TYPE_RAM:return "RAM";
        case MEDIA_TYPE_ROM:return "ROM";
        default:return "none";
    }
}

void phymem_print_detail(void)
{
#define PHY_FORMAT "%-8d%-8s0x%-12x0x%-12X\r\n" 
#define PHY_PARAM(mem) (mem).memidx,phymem_type((mem).type),(mem).base,(mem).size
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
#endif
#endif

