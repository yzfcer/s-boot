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
#ifndef PORT_H__
#define PORT_H__
#include "boot_type.h"
#include <stdio.h>

#define CPU_NAME "Intel core i3-4170"
#define ARCH_NAME "Intel i3"
#define BOARD_NAME "windows simulation"


int device_init(void);
int device_deinit(void);

int boot_output(char *buf,int len);
int32_t boot_getchar_noblocking(char *ch);
int32_t boot_receive_img(uint32_t addr,uint32_t maxlen);

uint32_t boot_get_sys_ms(void);
void boot_jump_to_app(void);

void feed_watchdog(void);
uint8_t is_chip_lock();
void set_chip_lock(uint8_t lock_enable);

#endif
