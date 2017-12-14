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
#include "boot_part.h"
#ifdef __cplusplus
extern "C" {
#endif
#define REG_NAME_LEN 12
#define INVALID_REAL_ADDR 0xffffffff


//-----------------------------------------------------------------------
typedef enum
{
    MEM_NULL = 0,
    MEM_NORMAL=1,
    MEM_ERROR=2,      
}mem_status_e;


part_s *mem_map_get_reg(char *name);
part_s *mem_map_get_list(void);
w_int32_t mem_map_get_reg_count(void);
void mem_map_print_status(void);

w_int32_t mem_map_reset(part_s *map);
w_int32_t mem_map_check(void);

void mem_map_copy_info(part_s *src,part_s *dest);
w_int32_t mem_map_copy_data(part_s *src,part_s *dest);

w_uint32_t mem_map_share_addr(void);
char *memtype_name(w_int16_t type);

#ifdef __cplusplus
}
#endif

#endif

