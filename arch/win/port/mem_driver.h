#ifndef MEM_DRIVER_H__
#define MEM_DRIVER_H__
#include "boot_type.h"
#include "mem_map.h"

uint32_t get_iram_base(void);
uint32_t get_xram_base(void);
uint32_t get_iflash_base(void);
uint32_t get_xflash_base(void);

uint32_t get_iram_lenth(void);
uint32_t get_xram_lenth(void);
uint32_t get_iflash_lenth(void);
uint32_t get_xflash_lenth(void);



#endif
