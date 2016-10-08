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

//img�ļ��������ȣ��ⲿ��һ��洢�ļ���һЩ�����Ϣ
//ͬʱ����0x2000λ�ñ������ļ��汾��Ϣ
#define RESERVE_FILE_LENTH 0x3000
typedef enum
{
    APP_IDX_PRO1,
    APP_IDX_PROBAK,
}app_idx_e;

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
