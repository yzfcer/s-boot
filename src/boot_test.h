/***************************************************************************************
** 文件名：
** 文件信息：
** 创建时间：2015.10.23
** 创建人：周江村
**文件版本1.0
** 修改记录：
***************************************************************************************/
#ifndef BOOT_TEST_H__
#define BOOT_TEST_H__
#include "mem_map.h"
#ifdef __cplusplus
extern "C" {
#endif

#define BOOT_TEST_ENABLE 1
#if BOOT_TEST_ENABLE
typedef enum 
{
    ERR_SFlash,
    ERR_XRam,
    ERR_PRama,
    ERR_APP_TYPE,
    ERR_CNT
}sys_error;

typedef struct __boot_stub
{
    uint8_t err[ERR_CNT];
}boot_stub_s;


typedef struct 
{
    char key;
    char *item_desc;
    void (*test)(void);
}boot_test_s;
void set_error(uint8_t err_type,uint8_t err);
uint8_t get_error(uint8_t err_type);
int32_t test_entry(void);
#else
#define get_error(t) 0
#endif

#ifdef __cplusplus
}
#endif

#endif

