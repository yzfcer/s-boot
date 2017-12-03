/*********************************************************************************
  *Copyright(C),2016-2018,yzfcer@163.com
  *FileName:  
  *Author:      Jason Zhou
  *Version:     1.0
  *Date:        2016/10/08
  *Description:  
  *Others:  
  *History:  
     1.Date:
       Author:
       Modification:
**********************************************************************************/
#ifndef MEM_MAP_H__
#define MEM_MAP_H__
#include "wind_type.h"
#include "share_param.h"
#ifdef __cplusplus
extern "C" {
#endif
#define REG_NAME_LEN 20
#define INVALID_REAL_ADDR 0xffffffff


//-----------------------------------------------------------------------

//#define DEFAULT_MAP_INDEX 1

typedef enum
{
    MEM_NULL = 0,
    MEM_NORMAL=1,
    MEM_ERROR=2,      
}mem_status_e;


typedef struct __region
{
    char name[REG_NAME_LEN];
    w_int16_t type;
    w_int16_t memidx;
    w_uint32_t addr;
    w_int32_t size;
    w_int32_t datalen;
    w_uint32_t crc;
    w_uint32_t status;
}region_s;

region_s *mem_map_get_reg(char *name);
region_s *mem_map_get_list(void);
w_int32_t mem_map_get_reg_count(void);
void mem_map_print_status(void);

w_int32_t mem_map_reset(region_s *map);
w_int32_t mem_map_check(void);

void mem_map_copy_info(region_s *src,region_s *dest);
w_int32_t mem_map_copy_data(region_s *src,region_s *dest);

w_uint32_t mem_map_share_addr(void);
char *memtype_name(w_int16_t type);

#ifdef __cplusplus
}
#endif

#endif

