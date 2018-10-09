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
#include "boot_part.h"
#include "boot_media.h"
#include <conio.h>
#include "wind_debug.h"

w_int32_t boot_enter_main_hook(void)
{
	return 0;
}

w_int32_t boot_exit_hook(void)
{
	return 0;
}
void      wind_std_port_init(void)
{
    
}

w_int32_t wind_std_output(w_uint8_t *buf,w_int32_t len)
{
    w_int32_t i;
    for(i = 0;i < len;i ++)
    {
        putchar(buf[i]);
    }
    return len;
}

w_int32_t wind_std_input(w_uint8_t *buff,w_int32_t len)
{
    char c;
    w_int32_t i;
    for(i = 0;i < len;i ++)
    {
        c = _kbhit();
        if(c)
        {
            buff[i] = getch();
        }
        else
            return i;
    }
	return len;
}

w_int32_t boot_receive_img(w_part_s *part)
{
    FILE *file;
    w_int32_t len;
    w_uint8_t *buff = get_common_buffer();
    file = fopen("imgfile.none.img","rb");
    if(!file)
        return -1;
    boot_part_seek(part,0);
    while(1)
    {
        len = fread((w_uint8_t*)buff,1,COMMBUF_SIZE,file);
        if(len > 0)
            boot_part_write(part,buff,len);
        else
            break;
    }
    fclose(file);
	return part->datalen;
}

w_uint32_t boot_get_sys_ms(void)
{
	return GetTickCount();
}


extern w_media_s media_ram1;
extern w_media_s media_ram2;
extern w_media_s media_rom1;
extern w_media_s media_rom2;
w_err_t boot_medias_register(void)
{
    boot_media_register(&media_ram1);
    boot_media_register(&media_ram2);
    boot_media_register(&media_rom1);
    boot_media_register(&media_rom2);
    return W_ERR_OK;
}

w_err_t boot_parts_create(void)
{
    w_media_s *media;
    media = boot_media_get("rom1");
    boot_part_create(PART_BOOT,media,BOOT_SIZE);
    boot_part_create(PART_PARAM1,media,BT_PARA1_SIZE);
    boot_part_create(PART_PARAM2,media,BT_PARA2_SIZE);
    boot_part_create(PART_IMGPARA,media,IMG_PARA_SIZE);
    boot_part_create(PART_SYSRUN,media,RAMRUN_SIZE);
    
    media = boot_media_get("rom2");
    boot_part_create(PART_IMG1,media,IMG1_SIZE);
    boot_part_create(PART_IMG2,media,IMG2_SIZE);
    
    media = boot_media_get("ram1");
    boot_part_create(PART_CACHE,media,CACHE_SIZE);
    boot_part_create(PART_SHARE,media,SHARE_SIZE);
    
    return W_ERR_OK;
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

w_bool_t is_chip_lock()
{
	return B_TRUE;
}

void set_chip_lock(w_uint8_t lock_enable)
{

}












