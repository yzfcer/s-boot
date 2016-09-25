/***************************************************************************************
** �ļ�����
** �ļ���Ϣ��
** ����ʱ�䣺2016.09.25
** �����ˣ���纣��
**�ļ��汾1.0
** �޸ļ�¼��
***************************************************************************************/
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
