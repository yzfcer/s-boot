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
#ifndef MEM_DRIVER_H__
#define MEM_DRIVER_H__
#include "wind_type.h"


void phy_mems_register(void);
void parts_create(void);

//´æ´¢Çý¶¯
w_bool_t mem_drv_init(void);
w_int32_t mem_read(w_int32_t memidx,w_uint32_t addr,w_uint8_t *buf,w_int32_t lenth);
w_int32_t mem_write(w_int32_t memidx,w_uint32_t addr,w_uint8_t *buf,w_int32_t lenth);
w_int32_t mem_erase(w_int32_t memidx,w_uint32_t addr,w_int32_t lenth);

#endif
