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


//-----------------------------------------------------------------------

//#define DEFAULT_MAP_INDEX 1
typedef enum 
{
    MEM_TYPE_RAM,
    MEM_TYPE_ROM,
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
    memtype_e type; 
    uint32_t index;
    uint32_t addr;
    uint32_t maxlen;
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
    region_s program2_region;//�ڶ��ݳ��������
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
    region_s flash;//bootloader����ʹ�õ��ڴ���
    region_s ram;//�������ص��ڴ���
}run_map_s;


typedef struct __mem_map_s
{
    rom_map_s rom;
    ram_map_s ram;
    run_map_s run;
}mem_map_s;

void init_map_info(mem_map_s *map);
int32_t mem_region_init(void);
int32_t check_map_valid(void);

void print_map_info(mem_map_s *map);
void print_program_space(mem_map_s *map);

uint32_t get_share_addr(void);
char *memtype_name(uint32_t type);
char *region_name(uint32_t regidx);
mem_map_s *get_memory_map(void);
int32_t set_default_map(int32_t index);
void choose_default_map(void);

void copy_region_info(region_s *src,region_s *dest);

#ifdef __cplusplus
}
#endif

#endif

