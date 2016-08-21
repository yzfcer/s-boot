#ifndef PORT_H__
#define PORT_H__
#include "boot_type.h"
#include <stdio.h>
#define printk_rt(fmt,...) printf(fmt,##__VA_ARGS__)
void feed_watchdog(void);
uint8_t is_chip_lock();
void set_chip_lock(uint8_t lock_en);


int32_t boot_get_sys_ms(void);
void boot_run_system(void);

int32_t read_char_noblocking(char *ch);
int32_t read_char_blocking(char *ch);
int32_t read_str_withblockig(char *buff,int32_t len);
int32_t receive_img_data(uint32_t addr,uint32_t maxlen);

int32_t device_init(void);
int32_t device_deinit(void);

uint8_t check_app_type_switch(void);
#endif
