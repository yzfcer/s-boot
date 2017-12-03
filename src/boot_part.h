#ifndef BOOT_PART_H__
#define BOOT_PART_H__
#include "wind_type.h"
#define PART_NAME_LEN 20
#define MEM_NAME_LEN 20


#define MEM_TYPE_ROM 1
#define MEM_TYPE_RAM 2


//物理存储空间
typedef struct 
{
    w_int16_t memidx;
    w_int16_t type;
    w_uint32_t base;
    w_int32_t size;
}phymem_s;

phymem_s *phymem_get_list(void);
w_int32_t phymem_get_lcount(void);
phymem_s *phymem_get_instance(w_int32_t idx);

//空间分区表
typedef struct 
{
    char name[PART_NAME_LEN];
    w_int16_t memidx;
    w_int16_t memtype;
    w_uint32_t addr;
    w_int32_t size;
}part_s;

//void part_init(void);
part_s *part_get_inst_name(char *name);
part_s *part_get_inst_idx(w_int16_t memidx);
part_s *part_get_list(void);
void part_print_detail(void);
w_int32_t part_check_conflict(void);

#endif
