#include "port.h"
#include <stdio.h>
#include <windows.h>
#include <winbase.h>
#include "boot_param.h"
void feed_watchdog(void)
{
}

uint8_t is_chip_lock()
{
	return 0;
}

void set_chip_lock(uint8_t lock_en)
{

}


void boot_run_system(void)
{

}



int32_t read_char_noblocking(char *ch)
{
	scanf("%c",ch);
	return 0;
}

int32_t boot_get_sys_ms(void)
{
	return GetTickCount();
}

int32_t read_char_blocking(char *ch)
{
	scanf("%c",ch);
	return 0;
}

int32_t read_str_withblockig(char *buff,int32_t len)
{
	return 0;
}

int32_t receive_img_data(uint32_t addr,uint32_t maxlen)
{
	return 0;
}



int32_t device_init(void)
{
	return 0;
}

int32_t device_deinit(void)
{
	return 0;
}

uint8_t check_app_type_switch(void)
{
	return 0;
}
