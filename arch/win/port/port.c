/***************************************************************************************
** �ļ�����
** �ļ���Ϣ��
** ����ʱ�䣺2016.09.25
** �����ˣ���纣��
**�ļ��汾 1.0
** �޸ļ�¼��
***************************************************************************************/
#include "port.h"
#include <stdio.h>
#include <windows.h>
#include <winbase.h>
#include "boot_param.h"
#include <conio.h>

int32_t device_init(void)
{
	return 0;
}

int32_t device_deinit(void)
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

void boot_run_system(void)
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











