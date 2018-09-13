/*********************************************************************************
  *Copyright(C),2016-2018,yzfcer@163.com
  *FileName:  
  *Author:      Jason Zhou
  *Version:     1.0
  *Date:        2017/12/14
  *Description:  
  *Others:  
  *History:  
     1.Date:
       Author:
       Modification:
**********************************************************************************/
#ifndef PHY_MEM_H__
#define PHY_MEM_H__
#include "wind_type.h"
#define MEM_NAME_LEN 12
#define PHYMEM_COUNT 6

#define MEM_TYPE_ROM 1
#define MEM_TYPE_RAM 2

//ŒÔ¿Ì¥Ê¥¢ø’º‰
typedef struct 
{
    w_int16_t memidx;
    w_int16_t type;
    w_uint32_t base;
    w_int32_t size;
    w_int32_t used;
}phymem_s;

char * phymem_type(w_int16_t type);
phymem_s *phymem_get_list(void);
w_int32_t phymem_get_lcount(void);
phymem_s *phymem_get_instance(w_int32_t idx);
w_bool_t  phymem_register(w_int16_t mtype,w_uint32_t base,w_int32_t size);
w_int32_t phymem_get_count(void);
void      phymem_print_detail(void);

#endif
