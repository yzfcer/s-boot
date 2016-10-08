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
#include "port.h"
#include <stdio.h>
#include <windows.h>
#include <winbase.h>
#include "boot_param.h"
#include <conio.h>

int device_init(void)
{
	return 0;
}

int device_deinit(void)
{
	return 0;
}

int boot_output(char *buf,int len)
{
    int i;
    for(i = 0;i < len;i ++)
    {
        putchar(buf[i]);
    }
    return len;
}

int32_t boot_getchar_noblocking(char *ch)
{
    char c;
    c = _kbhit();
    if(c)
    {
        *ch = getch();
        return 0;
    }
	return -1;
}

int32_t boot_receive_img(uint32_t addr,uint32_t maxlen)
{
	return 0;
}

uint32_t boot_get_sys_ms(void)
{
	return GetTickCount();
}

void boot_jump_to_app(void)
{

}

void feed_watchdog(void)
{

}

uint8_t is_chip_lock()
{
	return 0;
}

void set_chip_lock(uint8_t lock_enable)
{

}

int32_t encrypt_data(uint8_t *data,int32_t lenth)
{
    return lenth;
}
int32_t decrypt_data(uint8_t *data,int32_t lenth)
{
    return lenth;
}










