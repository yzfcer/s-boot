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
//硬件RAM和ROM的数量
#define RAM_COUNT 2
#define ROM_COUNT 2

//硬件RAM和ROM的基地址和空间大小
#define RAM1_BASE 0x20000000
#define RAM2_BASE 0x60000000
#define RAM1_SIZE 0x40000
#define RAM2_SIZE 0x100000

#define ROM1_BASE 0x00000000
#define ROM2_BASE 0xA0000000
#define ROM1_SIZE 0x100000
#define ROM2_SIZE 0x800000

#define MEM_BASE_INVALID 0xffffffff



#define BOOTLOADER_IDX 0
#define PARAM1_IDX 0
#define PARAM2_IDX 0
#define PROGRAM1_IDX 0
#define PROGRAM2_IDX 0
#define RESERVED_IDX 0

//在只使用内部FLASH时，FLASH的地址划分从低地址开始依次如下:
//【bootloader】【bootloader参数】【bootloader参数备份】【系统程序】【系统程序备份】【保留区(存储系统程序参数)】
//其中两份bootloader参数是完全一样的，两份系统程序可能不一样，其中第一份是运行程序，第二份用于在第一份出错时修复使用
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

int read_rom(uint8_t memidx,uint32_t addr,uint8_t *buf,int32_t lenth);
int write_rom(uint8_t memidx,uint32_t addr,uint8_t *buf,int32_t lenth);
int erase_rom(uint8_t memidx,uint32_t addr,int32_t lenth);

#endif
