#ifndef BOOT_CONFIG_H__
#define BOOT_CONFIG_H__
#ifdef __cplusplus
extern "C" {
#endif

//外部RAM是否存在
#define XRAM_EXIST 1
//外部FLASH是否存在
#define XFLASH_EXIST 1

//存储系统总体空间定义
#define IRAM_LENTH 0x40000
#define IFLASH_LENTH 0x100000
#define XRAM_LENTH 0x100000
#define XFLASH_LENTH 0x400000


//在只使用内部FLASH时，FLASH的地址划分从低地址开始依次如下:
//【bootloader】【bootloader参数】【bootloader参数备份】【系统程序】【系统程序备份】【保留区(存储系统程序参数)】
//其中两份bootloader参数是完全一样的，两份系统程序可能不一样，其中第一份是运行程序，第二份用于在第一份出错时修复使用
#define BOOT_PROGRAM_LENTH 0xE000
#define BOOT_PARAM_LENTH 0x1000
#define BOOT_PROGRAM_MIN_LENTH 0x10000
#define BOOT_PRODUCT_LENTH 0x28000
#define BOOT_RESERVED_LENTH 0x1000
#define BOOT_RAM_LENTH 0x8000
#define BOOT_PROGRAMBUF_LENTH 0x18000
#define BOOT_SHARE_PARAM_LENTH 0x1000


#define BOOT2_PROGRAM1_LENTH 0x180000
#define BOOT2_PROGRAMBAK_LENTH 0x180000
#define BOOT2_PRODUCT_LENTH 0x100000

#ifdef __cplusplus
}
#endif
#endif
