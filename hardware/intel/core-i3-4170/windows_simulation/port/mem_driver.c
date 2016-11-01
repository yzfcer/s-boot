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

uint8_t g_ram1[RAM1_SIZE];
uint8_t g_ram2[RAM2_SIZE];
uint8_t g_rom1[ROM1_SIZE];
uint8_t g_rom2[ROM2_SIZE];

typedef struct
{
    uint32_t base;
    uint32_t size;
}MEM_SPACE_S;

MEM_SPACE_S g_rom[] = 
{
    {ROM1_BASE,ROM1_SIZE},
    {ROM2_BASE,ROM2_SIZE},
};

MEM_SPACE_S g_ram[] = 
{
    {RAM1_BASE,RAM1_SIZE},
    {RAM2_BASE,RAM2_SIZE},
};



uint32_t get_rom_base(int32_t idx)
{
    int count = sizeof(g_rom)/sizeof(MEM_SPACE_S);
    if(idx >= count)
        return MEM_BASE_INVALID;
    return g_rom[idx].base;
}

uint32_t get_rom_lenth(int32_t idx)
{
    int count = sizeof(g_rom)/sizeof(MEM_SPACE_S);
    if(idx >= count)
        return MEM_BASE_INVALID;
    return g_rom[idx].size;
}

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

uint32_t get_ram_lenth(int32_t idx)
{
    int count = sizeof(g_ram)/sizeof(MEM_SPACE_S);
    if(idx >= count)
        return MEM_BASE_INVALID;
    return g_ram[idx].size;
}




static void flush_rom_file(uint8_t memidx)
{
    FILE *fil;
    char filename[16];
    uint32_t len;
    uint8_t *buf;
    switch(memidx)
    {
    case 0:
        buf = (uint8_t *)(g_rom1);
        break;
    case 1:
        buf = (uint8_t *)(g_rom2);
    default:
        return ;
    }
    len = get_rom_lenth(memidx);
    sprintf(filename,"rom%d.bin",memidx);
    fil = fopen(filename,"wb+");
    fwrite(buf,1,len,fil);
    fclose(fil);
}

static void read_rom_file(uint8_t memidx)
{
    FILE *fil;
    char filename[16];
    uint32_t len;
    uint8_t *buf;// = (uint8_t*)(get_rom_base(memidx);
    switch(memidx)
    {
    case 0:
        buf = (uint8_t *)(g_rom1);
        break;
    case 1:
        buf = (uint8_t *)(g_rom2);
    default:
        return ;
    }
    len = get_rom_lenth(memidx);
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

int read_rom(uint8_t memidx,uint32_t addr,uint8_t *buf,int32_t lenth)
{
    uint8_t *src;
    uint32_t base = get_rom_base(memidx);
    switch(memidx)
    {
    case 0:
        src = (uint8_t *)(g_rom1+addr-base);
        break;
    case 1:
        src = (uint8_t *)(g_rom2+addr-base);
    default:
        return -1;
    }
    read_rom_file(memidx);
    memcpy(buf,src,lenth);
    return lenth;
}

int write_rom(uint8_t memidx,uint32_t addr,uint8_t *buf,int32_t lenth)
{
    uint8_t *dest;
    uint32_t base = get_rom_base(memidx);
    uint8_t *src;
    switch(memidx)
    {
    case 0:
        dest = (uint8_t *)(g_rom1+addr-base);
        break;
    case 1:
        dest = (uint8_t *)(g_rom2+addr-base);
    default:
        return -1;
    }
    memcpy(dest,buf,lenth);
    flush_rom_file(memidx);
    return lenth;
}

int erase_rom(uint8_t memidx,uint32_t addr,int32_t lenth)
{
    uint8_t *dest = (uint8_t *)addr;
    memset(dest,0,lenth);
    flush_rom_file(memidx);
    return lenth;
}


