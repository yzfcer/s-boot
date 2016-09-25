/***************************************************************************************
** �ļ�����
** �ļ���Ϣ��
** ����ʱ�䣺2015.10.13
** �����ˣ��ܽ���
**�ļ��汾1.0
** �޸ļ�¼��
***************************************************************************************/
#ifndef BOOT_PARAM_H__
#define BOOT_PARAM_H__
#include "mem_map.h"
#ifdef __cplusplus
extern "C" {
#endif

#define BOOT_PARAM_MAGIC 0X2561C5A0
#define BOOT_VERSION 1
#define BLOCK_SIZE 4096

typedef struct 
{
    uint32_t lenth;
    uint32_t crc;
}pro_verify_s;

//�洢��flash�����������ݵ�����ṹ��
//�ⲿ���������ڴ���û�б��ݣ�����Ҫʱ��flash����
typedef struct 
{
    uint32_t magic;//ħ���֣����ڲ���ͷ��У��
    uint32_t lenth;//�������ṹ��ĳ���
    
    uint8_t version;//bootloader�İ汾��
    uint8_t debug_mode;
    uint8_t pad1[1];

    uint32_t flush_num;//��������д����
    mem_map_s mem_map;//ϵͳ�Ŀռ�ӳ���

    uint32_t crc;//CRCУ��
}boot_param_s;

void *sys_boot_params(void);
void init_map_info(mem_map_s *map);

void *get_boot_params_from_ROM(void);
void upate_bootparam_crc(boot_param_s *btprm);
void init_boot_param(const mem_map_s *mmap);
int32_t check_boot_param(boot_param_s *btprm);
int32_t check_app_debug_mode(void);
void clear_boot_param_buffer(void);
int32_t read_param(void);
int32_t write_param(void);

#ifdef __cplusplus
}
#endif

#endif

