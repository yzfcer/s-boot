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
 
//硬件RAM数量和基地址和空间大小定义
#define RAM_COUNT 2
#define RAM1_BASE 0x20000000
#define RAM1_SIZE 0x40000
#define RAM2_BASE 0x60000000
#define RAM2_SIZE 0x100000

//硬件RAM数量和基地址和空间大小定义
#define ROM_COUNT 2
#define ROM1_BASE 0x00000000
#define ROM1_SIZE 0x100000
#define ROM2_BASE 0xA0000000
#define ROM2_SIZE 0x800000
//无效地址
#define MEM_BASE_INVALID 0xffffffff


/***************************************************************
 s-boot依次需要定义如下的ROM数据存储空间:
【bootloader程序区】
【bootloader参数区】
【bootloader参数备份区】
【系统程序存储区】
【系统程序备份区】
【系统程序参数区】
【ROM文件系统】
【系统程序ROM运行区】
其中两份bootloader参数是完全一样的，两份系统程序可能不一样，
其中第一份是运行程序，第二份用于在第一份出错时修复使用,
文件系统不是必须的，如果不用文件系统，将size定义成0
***************************************************************/

#define BOOT_PROGRAM_IDX  0
#define BOOT_PROGRAM_ADDR 0
#define BOOT_PROGRAM_SIZE 0xE000

#define BOOT_PARAM1_IDX  0
#define BOOT_PARAM1_ADDR 0xE000
#define BOOT_PARAM1_SIZE 0x1000

#define BOOT_PARAM2_IDX  0
#define BOOT_PARAM2_ADDR 0xf000
#define BOOT_PARAM2_SIZE 0x1000

#define SYS_PROGRAM1_IDX  0
#define SYS_PROGRAM1_ADDR 0x10000
#define SYS_PROGRAM1_SIZE 0x40000

#define SYS_PROGRAM2_IDX  0
#define SYS_PROGRAM2_ADDR 0x50000
#define SYS_PROGRAM2_SIZE 0x40000

#define SYS_PARAM_IDX  0
#define SYS_PARAM_ADDR 0x90000
#define SYS_PARAM_SIZE 0x2000

#define SYS_ROMFS_IDX  0
#define SYS_ROMFS_ADDR 0xA0000
#define SYS_ROMFS_SIZE 0

#define SYS_ROMRUN_IDX  SYS_PROGRAM1_IDX
#define SYS_ROMRUN_ADDR SYS_PROGRAM1_ADDR
#define SYS_ROMRUN_SIZE SYS_PROGRAM1_SIZE

/***************************************************************
 s-boot需要定义如下的RAM数据存储空间:
【boot和系统内存数据区】
【boot和系统共享参数区】
【系统程序下载/升级缓存区】
【系统程序RAM运行区】
***************************************************************/
#define DATA_RAM_IDX  0
#define DATA_RAM_ADDR 0
#define DATA_RAM_SIZE 0x3F000

#define SYS_SHAREPRM_IDX  0
#define SYS_SHAREPRM_ADDR 0x3F000
#define SYS_SHAREPRM_SIZE 0x1000

#define SYS_LOADBUF_IDX  1
#define SYS_LOADBUF_ADDR 0
#define SYS_LOADBUF_SIZE 0x40000

#define SYS_RAMRUN_IDX  1
#define SYS_RAMRUN_ADDR 0x40000
#define SYS_RAMRUN_SIZE 0xC0000

uint32_t get_ram_base(int32_t idx);
uint32_t get_rom_base(int32_t idx);
uint32_t get_ram_lenth(int32_t idx);
uint32_t get_rom_lenth(int32_t idx);

int read_rom(uint8_t memidx,uint32_t addr,uint8_t *buf,int32_t lenth);
int write_rom(uint8_t memidx,uint32_t addr,uint8_t *buf,int32_t lenth);
int erase_rom(uint8_t memidx,uint32_t addr,int32_t lenth);

#endif
