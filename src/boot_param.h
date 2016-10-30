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

//�洢��flash�����������ݵ�����ṹ��
//�ⲿ���������ڴ���û�б��ݣ�����Ҫʱ��flash����
typedef struct 
{
    uint32_t magic;//ħ���֣����ڲ���ͷ��У��
    uint32_t lenth;//�������ṹ��ĳ���
    
    uint8_t version;    //bootloader`
    uint8_t debug_mode; //����ģʽ
    uint8_t wait_sec;   //�ȴ������¼�����
    uint8_t run_type;   //������RAM��Flash
    uint8_t encrypt_type;//�������ʹ��
    bool_t lock_en;     //оƬ����ʹ��
    
    mem_map_s mem_map;//ϵͳ�Ŀռ�ӳ���
}boot_param_s;

void *get_boot_params(void);
void *get_boot_params_from_ROM(void);

void param_init(const mem_map_s *mmap);
int32_t param_check_valid(uint8_t *prmbuf);
int32_t param_check_debug_mode(void);
void    param_clear_buffer(void);
int32_t param_read(void);
int32_t param_flush(void);

#ifdef __cplusplus
}
#endif

#endif

