#ifndef BOOT_PART_H__
#define BOOT_PART_H__
#include "wind_type.h"

#define PART_NAME_LEN 12
#define PART_COUNT 10
#define INVALID_REAL_ADDR 0xffffffff

typedef enum
{
    MEM_NULL = 0,
    MEM_NORMAL=1,
    MEM_ERROR=2,      
}mem_status_e;

//空间分区表
typedef struct 
{
    char name[PART_NAME_LEN];
    w_int8_t memidx;
    w_int8_t memtype;
    w_uint16_t status;
    w_uint32_t addr;
    w_int32_t size;
    w_int32_t datalen;
    w_uint32_t crc;
}part_s;

w_bool_t  part_create(char *name,w_int8_t midx,w_int32_t size);
w_int32_t part_get_count(void);

part_s *part_get_inst_name(char *name);
part_s *part_get_inst_idx(w_int8_t memidx);
part_s *part_get_list(void);
void part_print_detail(void);


void part_print_status(void);

void part_copy_info(part_s *src,part_s *dest);
w_int32_t part_copy_data(part_s *src,part_s *dest);
w_uint32_t part_share_addr(void);

#endif
