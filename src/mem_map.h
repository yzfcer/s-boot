/***************************************************************************************
** 文件名：
** 文件信息：
** 创建时间：2015.10.13
** 创建人：周江村
**文件版本1.0
** 修改记录：
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
    region_s boot_region;//bootloader的程序代码区
    region_s param1_region;//第一份参数代码区
    region_s param2_region;//第二份参数代码区
    region_s program1_region;//第一份程序代码区
    region_s program2_region;//第二份程序代码区
    region_s reserve_region;//保留区
}rom_map_s;

typedef struct __RAM_map_s
{
    region_s app_region;//bootloader运行使用的内存区
    region_s probuf_region;//程序下载的内存区
    region_s share_region;//bootloader与应用程序的交换区
}ram_map_s;

typedef struct __RUN_map_s
{
    region_s flash;//bootloader运行使用的内存区
    region_s ram;//程序下载的内存区
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

