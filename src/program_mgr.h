/***************************************************************************************
** 文件名：
** 文件信息：
** 创建时间：2016.09.25
** 创建人：清风海岸
**文件版本1.0
** 修改记录：
***************************************************************************************/
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
    DOWN_IFLASH,
    DOWN_SFLASH,
    DOWN_IRAM,
    DOWN_XRAM,
}downtype_e;

typedef enum
{
    APP_IDX_PRO1,
    APP_IDX_PROBAK,
}app_idx_e;

int32_t download_img_file(downtype_e type);
int32_t change_boot_app(int32_t index);void clean_program(void);
int32_t copy_data_on_memory(region_s *src,region_s *dest);
void clean_program(void);
int32_t check_programs(void);
int32_t write_encrypt_code_to_run(region_s *src,region_s *run);
int32_t check_and_decrypt_img(region_s *img,region_s *bin);

#ifdef __cplusplus
}
#endif

#endif
