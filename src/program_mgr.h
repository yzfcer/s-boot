/*********************************************************************************
  *Copyright(C),2016-2018,yzfcer@163.com
  *FileName:  
  *Author:      Zhou Jiangcun
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
#include "boot_type.h"
#include "mem_map.h"

#ifdef __cplusplus
extern "C" {
#endif

//img文件保留长度，这部分一般存储文件的一些相关信息
//同时还在0x2000位置保留了文件版本信息
#define RESERVE_FILE_LENTH 0x3000
typedef enum
{
    APP_IDX_PRO1,
    APP_IDX_PROBAK,
}app_idx_e;

#define STRUCT_MBR_IDX(type,member) (uint32_t)(&(type*)0->member)

typedef struct __img_head_s img_head_s;
struct __img_head_s
{
    uint8_t  magic[8];
    uint32_t img_len;
    uint32_t head_len;
    uint32_t head_ver;
    uint8_t  bin_ver[16];
    uint32_t bin_crc;
    uint32_t encry_type;
    uint32_t endian_test;
    uint8_t  reserve[16];
    uint8_t  img_name[64];
    uint8_t  board_name[32];
    uint8_t  arch_name[32];
    uint8_t  cpu_name[32];
    uint8_t  reserve1[508-224];
    uint32_t head_crc;
};

int32_t download_img_file(memtype_e type);
int32_t change_boot_app(int32_t index);
int32_t copy_region_data(region_s *src,region_s *dest);
void clean_program(void);
int32_t check_programs(void);
int32_t write_encrypt_code_to_run(region_s *src,region_s *run);
int32_t check_and_decrypt_img(region_s *img);

#ifdef __cplusplus
}
#endif

#endif
