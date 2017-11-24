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
#ifndef PROGRAM_MGR_H__
#define PROGRAM_MGR_H__
#include "wind_type.h"
#include "mem_map.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct __img_head_s img_head_s;
struct __img_head_s
{
    w_uint8_t  magic[8];
    w_uint32_t img_len;
    w_uint32_t head_len;
    w_uint32_t head_ver;
    w_uint8_t  bin_ver[16];
    w_uint32_t bin_crc;
    w_int32_t encry_type;
    w_uint32_t endian_test;
    w_uint8_t  reserve[16];
    w_uint8_t  img_name[64];
    w_uint8_t  board_name[32];
    w_uint8_t  arch_name[32];
    w_uint8_t  cpu_name[32];
    w_uint8_t  reserve1[508-224];
    w_uint32_t head_crc;
};

w_int32_t decrypt_img_data(region_s *img,region_s *bin);
w_int32_t download_img_file(memtype_e type);
w_int32_t change_boot_app(w_int32_t index);
w_int32_t copy_region_data(region_s *src,region_s *dest);
w_int32_t check_img_valid(region_s *img);
w_int32_t roll_back_program(void);

w_int32_t clean_program(void);

#ifdef __cplusplus
}
#endif

#endif
