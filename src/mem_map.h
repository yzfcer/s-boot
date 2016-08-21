/***************************************************************************************
** �ļ�����
** �ļ���Ϣ��
** ����ʱ�䣺2015.10.13
** �����ˣ��ܽ���
**�ļ��汾1.0
** �޸ļ�¼��
***************************************************************************************/
#ifndef MEM_MAP_H__
#define MEM_MAP_H__
#include "boot_type.h"
#include "share_param.h"
#ifdef __cplusplus
extern "C" {
#endif

#define INVALID_REAL_ADDR 0xffffffff

#if 0

//--------------------------------------------------
//�����ڲ�Flashʱ����ռ�Ļ���
#define BOOT_IPROGRAM1_BASE (BOOT_PARAM2_BASE+BOOT_PARAM_LENTH)
#define BOOT_IPROGRAM_LENTH 0x64000
#define BOOT_IPROGRAM2_BASE (BOOT_IPROGRAM1_BASE + BOOT_IPROGRAM_LENTH)
#define BOOT_IPRODUCT_BASE (BOOT_IPROGRAM2_BASE + BOOT_IPROGRAM_LENTH)
#define BOOT_IPRODUCT_LENTH 0x24000
#define BOOT_RESERVED_LENTH 0x4000
#define BOOT_RESERVED_BASE (IFLASH_BASE + IFLASH_LENTH - BOOT_RESERVED_LENTH)

#define BOOT_IPROBUFFER_LENTH 0x19000
#define BOOT_IPROBUFFER_BASE (BOOT_SHARE_DATA_BASE - BOOT_IPROBUFFER_LENTH)
#define BOOT_IRUN_BASE BOOT_IPROGRAM1_BASE
#define BOOT_IRUN_LENTH BOOT_IPROGRAM_LENTH

//�����ⲿFlashʱ����ռ�Ļ���
#define BOOT_XPROGRAM1_BASE 0x0
#define BOOT_XPROGRAM_LENTH 0xC8000
#define BOOT_XPROGRAM2_BASE (BOOT_XPROGRAM1_BASE + BOOT_XPROGRAM_LENTH)
#define BOOT_XPRODUCT_BASE (BOOT_XPROGRAM2_BASE + BOOT_XPROGRAM_LENTH)
#define BOOT_SFLASH_RESERVED 0x8000
#define BOOT_XPRODUCT_LENTH (0x64000 - BOOT_SFLASH_RESERVED)

#define BOOT_XPROBUFFER_LENTH 0xC8000
#define BOOT_XPROBUFFER_BASE (XRAM_BASE + XRAM_LENTH - BOOT_XPROBUFFER_LENTH)
#define BOOT_XRUN_BASE BOOT_IPROGRAM1_BASE
#define BOOT_XRUN_LENTH (BOOT_IPROGRAM_LENTH*2)
#endif
//-----------------------------------------------------------------------

//#define DEFAULT_MAP_INDEX 1
typedef enum 
{
    IRAM,
    XRAM,
    IFLASH,
    XFLASH
}memtype_e;

typedef enum
{
    MEM_NULL = 0,
    MEM_NORMAL,
    MEM_ERROR      
}mem_status_e;

typedef struct __region
{
    char *regname;
    uint32_t base;
    uint32_t maxlen;
    memtype_e type;   
    uint32_t lenth;
    uint32_t crc;
    uint32_t status; 
}region_s;

typedef struct __ROM_map_s
{
    region_s boot_region;//bootloader�ĳ��������
    region_s param1_region;//��һ�ݲ���������
    region_s param2_region;//�ڶ��ݲ���������
    region_s program1_region;//��һ�ݳ��������
    region_s programbak_region;//�ڶ��ݳ��������
    region_s product_region;//�������Գ��������
    region_s reserve_region;//������
}rom_map_s;

typedef struct __RAM_map_s
{
    region_s app_region;//bootloader����ʹ�õ��ڴ���
    region_s probuf_region;//�������ص��ڴ���
    region_s share_region;//bootloader��Ӧ�ó���Ľ�����
}ram_map_s;

typedef struct __RUN_map_s
{
    region_s iflash;//bootloader����ʹ�õ��ڴ���
    region_s iram;//�������ص��ڴ���
    region_s xram;//bootloader��Ӧ�ó���Ľ�����
}run_map_s;


typedef struct __mem_map_s
{
    rom_map_s rom;
    ram_map_s ram;
    run_map_s run;
}mem_map_s;

extern const char *sys_memtype[];
extern const char *sys_mem_name[];
int32_t mem_region_init(void);
int32_t check_map_valid(void);

void print32_t_map_info(mem_map_s *map);
void print32_t_map_list(void);
void print32_t_program_space(mem_map_s *map);

uint32_t get_share_addr(void);
mem_map_s *get_map_by_index(int32_t id);
mem_map_s *get_default_map(void);
int32_t set_default_map(int32_t index);
void choose_default_map(void);

void copy_region_info(region_s *src,region_s *dest);

#ifdef __cplusplus
}
#endif

#endif

