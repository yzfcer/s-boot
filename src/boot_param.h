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
#ifndef BOOT_PARAM_H__
#define BOOT_PARAM_H__
#include "boot_type.h"
#include "mem_map.h"
#ifdef __cplusplus
extern "C" {
#endif

#define BOOT_PARAM_MAGIC 0X2561C5A0
#define BOOT_VERSION 0x000001
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
    uint32_t flush_num;//��������д����
    
    uint8_t version;//bootloader`
    uint8_t debug_mode;//����ģʽ
    uint8_t wait_sec;//�ȴ������¼�����
    bool_t encrypt_en;//�������ʹ��
    bool_t lock_en;//оƬ����ʹ��
    mem_map_s mem_map;//ϵͳ�Ŀռ�ӳ���
    uint32_t crc;//CRCУ��
}boot_param_s;

void *get_boot_params(void);
void *get_boot_params_from_ROM(void);

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

