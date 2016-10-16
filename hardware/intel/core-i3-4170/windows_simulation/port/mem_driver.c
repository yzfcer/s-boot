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
#include "mem_driver.h"
#include <string.h>
#include <stdio.h>
//存储系统总体空间定义
#define RAM1_LENTH 0x40000
#define RAM2_LENTH 0x100000
#define ROM1_LENTH 0x100000
#define ROM2_LENTH 0x400000

uint8_t g_ram1[RAM1_LENTH];
uint8_t g_ram2[RAM2_LENTH];
uint8_t g_rom1[ROM1_LENTH];
uint8_t g_rom2[ROM2_LENTH];

uint32_t get_ram_base(int32_t idx)
{
    switch(idx)
    {
        case 0:
            return (uint32_t)&g_ram1;
        case 1:
            return (uint32_t)&g_ram2;
        default:
            return MEM_BASE_INVALID;
    }
}

uint32_t get_rom_base(int32_t idx)
{
    switch(idx)
    {
        case 0:
            return (uint32_t)&g_rom1;
        case 1:
            return (uint32_t)&g_rom2;
        default:
            return MEM_BASE_INVALID;
    }
}

uint32_t get_ram_lenth(int32_t idx)
{
    switch(idx)
    {
        case 0:
            return RAM1_LENTH;
        case 1:
            return RAM2_LENTH;
        default:
            return 0;
    }
}

uint32_t get_rom_lenth(int32_t idx)
{
    switch(idx)
    {
        case 0:
            return ROM1_LENTH;
        case 1:
            return ROM2_LENTH;
        default:
            return 0;
    }
}



static void flush_rom_file(uint8_t memidx)
{
    int fd;
    FILE *fil;
    char filename[16];
    uint8_t *buf = (uint8_t*)get_rom_base(memidx);
    uint32_t len = get_rom_lenth(memidx);
    sprintf(filename,"rom%d.bin",memidx);
    fil = fopen(filename,"wb+");
    fwrite(buf,1,len,fil);
    fclose(fil);
}

static void read_rom_file(uint8_t memidx)
{
    int fd;
    FILE *fil;
    char filename[16];
    uint8_t *buf = (uint8_t*)get_rom_base(memidx);
    uint32_t len = get_rom_lenth(memidx);
    sprintf(filename,"rom%d.bin",memidx);
    fil = fopen(filename,"rb");
    if(!fil)
        flush_rom_file(memidx);
    fil = fopen(filename,"rb");
    if(!fil)
        return;
    fread(buf,1,len,fil);
    fclose(fil);
}

int read_rom(uint8_t memidx,uint32_t offsetaddr,uint8_t *buf,int32_t lenth)
{
    uint32_t realaddr = get_rom_base(memidx) + offsetaddr;
    uint8_t *src = (uint8_t *)realaddr;
    read_rom_file(memidx);
    memcpy(buf,src,lenth);
    return lenth;
}

int write_rom(uint8_t memidx,uint32_t offsetaddr,uint8_t *buf,int32_t lenth)
{
    uint32_t realaddr = get_rom_base(memidx) + offsetaddr;
    uint8_t *dest = (uint8_t *)realaddr;
    memcpy(dest,buf,lenth);
    flush_rom_file(memidx);
    return lenth;
}

int erase_rom(uint8_t memidx,uint32_t offsetaddr,int32_t lenth)
{
    uint32_t realaddr = get_rom_base(memidx) + offsetaddr;
    uint8_t *dest = (uint8_t *)realaddr;
    memset(dest,0,lenth);
    flush_rom_file(memidx);
    return lenth;
}


