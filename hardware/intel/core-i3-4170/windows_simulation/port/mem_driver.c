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
#include "boot_part.h"
#include "phy_mem.h"

//硬件RAM数量和基地址和空间大小定义
#define RAM1_BASE 0x20000000
#define RAM1_SIZE 0x400000
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

#define BOOT_SIZE 0xE000
#define BT_PARA1_SIZE 0x1000
#define BT_PARA2_SIZE 0x1000
#define IMG1_SIZE 0x40000
#define IMG2_SIZE 0x40000
#define IMG_PARA_SIZE 0x2000
#define FS_SIZE 0
#define RUN_SIZE IMG1_SIZE

/***************************************************************
 s-boot需要定义如下的RAM数据存储空间:
【boot和系统共享参数区】
【系统程序下载/升级缓存区】
【系统程序RAM运行区】
***************************************************************/

#define SHARE_SIZE 0x1000
#define CACHE_SIZE 0x40000
#define RAMRUN_SIZE 0xC0000

w_uint8_t g_ram1[RAM1_SIZE];
w_uint8_t g_ram2[RAM2_SIZE];
w_uint8_t g_rom1[ROM1_SIZE];
w_uint8_t g_rom2[ROM2_SIZE];
#if 0
phymem_s g_phymem[] = 
{
    {0,MEM_TYPE_ROM,ROM1_BASE,ROM1_SIZE},
    {1,MEM_TYPE_ROM,ROM2_BASE,ROM2_SIZE},
    {2,MEM_TYPE_RAM,RAM1_BASE,RAM1_SIZE},
    {3,MEM_TYPE_RAM,RAM2_BASE,RAM2_SIZE}
};
#endif

void phymems_register(void)
{
    phymem_register(MEM_TYPE_ROM,g_rom1,ROM1_SIZE);
    phymem_register(MEM_TYPE_ROM,g_rom2,ROM2_SIZE);
    phymem_register(MEM_TYPE_RAM,g_ram1,RAM1_SIZE);
    phymem_register(MEM_TYPE_RAM,g_ram2,RAM2_SIZE);
}

void parts_create(void)
{
    part_create("boot",0,BOOT_SIZE);
    part_create("btpara1",0,BT_PARA1_SIZE);
    part_create("btpara2",0,BT_PARA2_SIZE);
    part_create("img1",1,IMG1_SIZE);
    part_create("img2",1,IMG2_SIZE);
    part_create("imgpara",0,IMG_PARA_SIZE);
    part_create("ramrun",3,RAMRUN_SIZE);
    part_create("romrun",0,RAMRUN_SIZE);
    part_create("cache",2,CACHE_SIZE);
    part_create("share",2,SHARE_SIZE);
}

w_uint32_t mem_get_base(w_int32_t idx)
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

w_uint32_t mem_get_size(w_int32_t idx)
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

static void flush_rom_file(w_int32_t memidx)
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
    len = mem_get_size(memidx);
    sprintf(filename,"rom%d.bin",memidx);
    fil = fopen(filename,"wb+");
    fwrite(buf,1,len,fil);
    fclose(fil);
}

static void read_rom_file(w_int32_t memidx)
{
    FILE *fil;
    char filename[16];
    w_uint32_t len;
    w_uint8_t *buf;// = (w_uint8_t*)(mem_get_base(memidx);
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
    len = mem_get_size(memidx);
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




w_bool_t mem_drv_init(void)
{
    return 0;
}


w_int32_t mem_read(w_int32_t memidx,w_uint32_t addr,w_uint8_t *buf,w_int32_t lenth)
{
    w_uint8_t *src;
    w_uint32_t base = mem_get_base(memidx);
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

w_int32_t mem_write(w_int32_t memidx,w_uint32_t addr,w_uint8_t *buf,w_int32_t lenth)
{
    w_uint8_t *dest;
    w_uint32_t base = mem_get_base(memidx);
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

w_int32_t mem_erase(w_int32_t memidx,w_uint32_t addr,w_int32_t lenth)
{
    w_uint8_t *dest = (w_uint8_t *)addr;
    memset(dest,0,lenth);
    flush_rom_file(memidx);
    return lenth;
}


