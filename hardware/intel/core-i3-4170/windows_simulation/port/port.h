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

//初始化芯片系统时钟，通常在使用了PLL之类的功能时，在正常运行前需要稳定的时钟源
int sys_clock_init(void);

//初始化一个可以提供毫秒时间的定时器
int boot_time_init(void);
//初始化调试输出接口
int boot_debug_init(void);
int boot_debug_output(char *buf,int len);
//初始化硬件看门狗，如果使用看门狗的话，否则可以空置函数体
int  watchdog_init(void);
void watchdog_feed(void);

//处理bootloader退出时的工作，如果没有必要，可以空置
int boot_exit(void);

//从控制接口读取一个字符，如果读取失败，则返回-1
int32_t boot_getchar_noblocking(char *ch);

//读取下载的映像文件到限制了大小的指定的空间
int32_t boot_receive_img(uint32_t addr,uint32_t maxlen);

//获取运行的毫秒时间，建议用一个毫秒计数器实现
uint32_t boot_get_sys_ms(void);

//跳转到被引导的程序，一般是操作系统，用汇编实现
void boot_jump_to_app(void);

//查询和设置芯片代码安全锁，如果没有必要可以空置
bool_t is_chip_lock();
void set_chip_lock(uint8_t lock_enable);

#endif
