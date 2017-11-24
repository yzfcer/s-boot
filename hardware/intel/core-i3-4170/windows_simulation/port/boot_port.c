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
#include "boot_port.h"
#include <stdio.h>
#include <windows.h>
#include <winbase.h>
#include "boot_param.h"
#include <conio.h>

w_int32_t device_init(void)
{
	return 0;
}

w_int32_t device_deinit(void)
{
	return 0;
}

w_int32_t boot_output(char *buf,w_int32_t len)
{
    w_int32_t i;
    for(i = 0;i < len;i ++)
    {
        putchar(buf[i]);
    }
    return len;
}

w_int32_t boot_getchar_noblocking(char *ch)
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

w_int32_t boot_receive_img(w_uint32_t addr,w_uint32_t maxlen)
{
    FILE *fil;
    w_int32_t len;
    fil = fopen("imgfile.none.img","rb");
    if(!fil)
        return -1;
    len = fread((w_uint8_t*)addr,1,maxlen,fil);
    fclose(fil);
	return len;
}

w_uint32_t boot_get_sys_ms(void)
{
	return GetTickCount();
}

void boot_jump_to_app(void)
{
    char ch;
    while(1)
    {
        scanf("%c",&ch);
    }
}

void feed_watchdog(void)
{

}

bool_t is_chip_lock()
{
	return B_TRUE;
}

void set_chip_lock(w_uint8_t lock_enable)
{

}












