#include "boot_config.h"
#include "mem_driver.h"

uint8_t g_iram[IRAM_LENTH];
uint8_t g_xram[XRAM_LENTH];

uint32_t get_iram_base(void)
{
    return (uint32_t)g_iram;
}

uint32_t get_xram_base(void)
{
    return (uint32_t)g_xram;
}

uint32_t get_iflash_base(void)
{
    return 0;
}
uint32_t get_xflash_base(void)
{
    return 0;
}

uint32_t get_iram_lenth(void)
{
    return IRAM_LENTH;
}
uint32_t get_xram_lenth(void)
{
    return XRAM_LENTH;
}
uint32_t get_iflash_lenth(void)
{
    return IFLASH_LENTH;
}
uint32_t get_xflash_lenth(void)
{
    return XFLASH_LENTH;
}


