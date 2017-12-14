#ifndef BOOT_PART_H__
#define BOOT_PART_H__
#include "wind_type.h"

#define PART_NAME_LEN 12
#define PART_COUNT 10

//�ռ������
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

#endif
