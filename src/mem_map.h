/*********************************************************************************
  *Copyright(C),2016-2018,yzfcer@163.com
  *FileName:  
  *Author:      Jason Zhou
  *Version:     1.0
  *Date:        2016/10/08
  *Description:  
  *Others:  
  *History:  
     1.Date:
       Author:
       Modification:
**********************************************************************************/
#ifndef MEM_MAP_H__
#define MEM_MAP_H__
#include "wind_type.h"
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
    w_uint32_t index;
    w_uint32_t addr;
    w_uint32_t size;
    w_uint32_t datalen;
    w_uint32_t crc;
    w_uint32_t status; 
}region_s;

typedef struct __ROM_map_s
{
    region_s boot_program;//bootloader�ĳ��������
    region_s boot_param1;//��һ�ݲ���������
    region_s boot_param2;//�ڶ��ݲ���������
    region_s sys_program1;//��һ�ݳ��������
    region_s sys_program2;//�ڶ��ݳ��������
    region_s sys_param;//Ϊ���ص�ϵͳ�����Ĳ�����
    region_s rom_fs;//�ļ�ϵͳ
}rom_map_s;

typedef struct __RAM_map_s
{
    region_s data_ram;//bootloader����ʹ�õ��ڴ���
    region_s load_buffer;//�������ص��ڴ���
    region_s share_param;//bootloader��Ӧ�ó���Ĳ���������
}ram_map_s;

typedef struct __RUN_map_s
{
    region_s flash;//bootloader��flash����
    region_s ram;//bootloader��ram����
}run_map_s;


typedef struct __mem_map_s
{
    rom_map_s rom;
    ram_map_s ram;
    run_map_s run;
}mem_map_s;

void init_map_info(mem_map_s *map);
w_int32_t mem_region_init(void);
w_int32_t check_map_valid(void);

void print_map_info(mem_map_s *map);
void print_program_space(mem_map_s *map);

w_uint32_t get_share_addr(void);
char *memtype_name(w_uint32_t type);
char *region_name(w_uint32_t regidx);
mem_map_s *get_memory_map(void);

void copy_region_info(region_s *src,region_s *dest);

#ifdef __cplusplus
}
#endif

#endif

