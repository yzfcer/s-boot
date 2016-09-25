#include "boot_config.h"
#include "mem_driver.h"

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



