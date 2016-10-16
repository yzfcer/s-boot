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
#ifndef MEM_DRIVER_H__
#define MEM_DRIVER_H__
#include "boot_type.h"
#include "mem_map.h"

#define RAM_COUNT 2
#define ROM_COUNT 2

#define MEM_BASE_INVALID 0xffffffff


#define BOOTLOADER_IDX 0
#define PARAM1_IDX 0
#define PARAM2_IDX 0
#define PROGRAM1_IDX 0
#define PROGRAM2_IDX 0
#define RESERVED_IDX 0

//��ֻʹ���ڲ�FLASHʱ��FLASH�ĵ�ַ���ִӵ͵�ַ��ʼ��������:
//��bootloader����bootloader��������bootloader�������ݡ���ϵͳ���򡿡�ϵͳ���򱸷ݡ���������(�洢ϵͳ�������)��
//��������bootloader��������ȫһ���ģ�����ϵͳ������ܲ�һ�������е�һ�������г��򣬵ڶ��������ڵ�һ�ݳ���ʱ�޸�ʹ��
#define BOOTLOADER_LENTH 0xE000
#define PARAM_LENTH 0x1000
#define PROGRAM_LENTH 0x40000
#define RESERVED_LENTH 0x1000

#define BOOTRAM_LENTH 0x10000
#define PROGRAMBUF_LENTH 0x18000
#define SHARE_PARAM_LENTH 0x1000

uint32_t get_ram_base(int32_t idx);
uint32_t get_rom_base(int32_t idx);
uint32_t get_ram_lenth(int32_t idx);
uint32_t get_rom_lenth(int32_t idx);

int read_rom(uint8_t memidx,uint32_t offsetaddr,uint8_t *buf,int32_t lenth);
int write_rom(uint8_t memidx,uint32_t offsetaddr,uint8_t *buf,int32_t lenth);
int erase_rom(uint8_t memidx,uint32_t offsetaddr,int32_t lenth);

#endif
