/***************************************************************************************
** 文件名：
** 文件信息：
** 创建时间：2016.09.25
** 创建人：清风海岸
**文件版本 1.0
** 修改记录：
***************************************************************************************/
#ifndef PORT_H__
#define PORT_H__
#include "boot_type.h"
#include <stdio.h>



int32_t device_init(void);
int32_t device_deinit(void);

int boot_output(char *buf,int len);
int32_t boot_getchar_noblocking(char *ch);
int32_t boot_receive_img(uint32_t addr,uint32_t maxlen);

uint32_t boot_get_sys_ms(void);
void boot_run_system(void);

void feed_watchdog(void);
uint8_t is_chip_lock();
void set_chip_lock(uint8_t lock_enable);

#endif
