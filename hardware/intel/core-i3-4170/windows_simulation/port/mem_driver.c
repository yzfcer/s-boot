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
#include "mem_driver.h"
#include <string.h>
#include <stdio.h>

w_uint8_t g_ram1[RAM1_SIZE];
w_uint8_t g_ram2[RAM2_SIZE];
w_uint8_t g_rom1[ROM1_SIZE];
w_uint8_t g_rom2[ROM2_SIZE];

#if 0
typedef struct
{
    w_uint32_t base;
    w_uint32_t size;
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
#endif


w_uint32_t get_rom_base(w_int32_t idx)
{
    switch(idx)
    {
        case 0:
            return (w_uint32_t)&g_rom1;
        case 1:
            return (w_uint32_t)&g_rom2;
        default:
            return MEM_BASE_INVALID;
    }
}

w_uint32_t get_rom_lenth(w_int32_t idx)
{
    switch(idx)
    {
        case 0:
            return ROM1_SIZE;
        case 1:
            return ROM2_SIZE;
        default:
            return MEM_BASE_INVALID;
    }
}

w_uint32_t get_ram_base(w_int32_t idx)
{
    switch(idx)
    {
        case 0:
            return (w_uint32_t)&g_ram1;
        case 1:
            return (w_uint32_t)&g_ram2;
        default:
            return MEM_BASE_INVALID;
    }
}

w_uint32_t get_ram_lenth(w_int32_t idx)
{
    switch(idx)
    {
        case 0:
            return RAM1_SIZE;
        case 1:
            return RAM2_SIZE;
        default:
            return MEM_BASE_INVALID;
    }
}




static void flush_rom_file(w_uint8_t memidx)
{
    FILE *fil;
    char filename[16];
    w_uint32_t len;
    w_uint8_t *buf;
    switch(memidx)
    {
    case 0:
        buf = (w_uint8_t *)(g_rom1);
        break;
    case 1:
        buf = (w_uint8_t *)(g_rom2);
    default:
        return ;
    }
    len = get_rom_lenth(memidx);
    sprintf(filename,"rom%d.bin",memidx);
    fil = fopen(filename,"wb+");
    fwrite(buf,1,len,fil);
    fclose(fil);
}

static void read_rom_file(w_uint8_t memidx)
{
    FILE *fil;
    char filename[16];
    w_uint32_t len;
    w_uint8_t *buf;// = (w_uint8_t*)(get_rom_base(memidx);
    switch(memidx)
    {
    case 0:
        buf = (w_uint8_t *)(g_rom1);
        break;
    case 1:
        buf = (w_uint8_t *)(g_rom2);
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

w_int32_t read_rom(w_uint8_t memidx,w_uint32_t addr,w_uint8_t *buf,w_int32_t lenth)
{
    w_uint8_t *src;
    w_uint32_t base = get_rom_base(memidx);
    switch(memidx)
    {
    case 0:
        src = (w_uint8_t *)(g_rom1+addr-base);
        break;
    case 1:
        src = (w_uint8_t *)(g_rom2+addr-base);
    default:
        return -1;
    }
    read_rom_file(memidx);
    memcpy(buf,src,lenth);
    return lenth;
}

w_int32_t write_rom(w_uint8_t memidx,w_uint32_t addr,w_uint8_t *buf,w_int32_t lenth)
{
    w_uint8_t *dest;
    w_uint32_t base = get_rom_base(memidx);
    switch(memidx)
    {
    case 0:
        dest = (w_uint8_t *)(g_rom1+addr-base);
        break;
    case 1:
        dest = (w_uint8_t *)(g_rom2+addr-base);
    default:
        return -1;
    }
    memcpy(dest,buf,lenth);
    flush_rom_file(memidx);
    return lenth;
}

w_int32_t erase_rom(w_uint8_t memidx,w_uint32_t addr,w_int32_t lenth)
{
    w_uint8_t *dest = (w_uint8_t *)addr;
    memset(dest,0,lenth);
    flush_rom_file(memidx);
    return lenth;
}


