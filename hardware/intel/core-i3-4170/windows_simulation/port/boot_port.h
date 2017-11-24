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
#ifndef BOOT_PORT_H__
#define BOOT_PORT_H__
#include "wind_type.h"
#include <stdio.h>

#define CPU_NAME "Intel core i3-4170"
#define ARCH_NAME "Intel i3"
#define BOARD_NAME "windows simulation"


w_int32_t device_init(void);
w_int32_t device_deinit(void);

w_int32_t boot_output(char *buf,w_int32_t len);
w_int32_t boot_getchar_noblocking(char *ch);
w_int32_t boot_receive_img(w_uint32_t addr,w_uint32_t maxlen);

w_uint32_t boot_get_sys_ms(void);
void boot_jump_to_app(void);

void feed_watchdog(void);
w_bool_t is_chip_lock();
void set_chip_lock(w_uint8_t lock_enable);

#endif
