#ifndef BOOT_CONFIG_H__
#define BOOT_CONFIG_H__
#ifdef __cplusplus
extern "C" {
#endif

//�ⲿRAM�Ƿ����
#define XRAM_EXIST 1
//�ⲿFLASH�Ƿ����
#define XFLASH_EXIST 1

//�洢ϵͳ����ռ䶨��
#define IRAM_LENTH 0x40000
#define IFLASH_LENTH 0x100000
#define XRAM_LENTH 0x100000
#define XFLASH_LENTH 0x400000


//��ֻʹ���ڲ�FLASHʱ��FLASH�ĵ�ַ���ִӵ͵�ַ��ʼ��������:
//��bootloader����bootloader��������bootloader�������ݡ���ϵͳ���򡿡�ϵͳ���򱸷ݡ���������(�洢ϵͳ�������)��
//��������bootloader��������ȫһ���ģ�����ϵͳ������ܲ�һ�������е�һ�������г��򣬵ڶ��������ڵ�һ�ݳ���ʱ�޸�ʹ��
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
