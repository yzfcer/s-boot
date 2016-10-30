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
#include "boot_param.h"
#include "port.h"
#include "boot_debug.h"
#include "crc.h"
#include "mem_map.h"
#include "boot_test.h"
#include "share_param.h"
#include "boot_hw_if.h"
#include "mem_driver.h"
#ifdef __cplusplus
extern "C" {
#endif
#if BOOT_TEST_ENABLE
extern upgrade_region_s g_upgrade_status;
boot_stub_s boot_stub = {0,0,0};
void clear_errors(void)
{
    
}

void set_error(uint8_t err_type,uint8_t err)
{
    if(err_type < ERR_CNT)
    {
        boot_stub.err[err_type] = err;
    }
}
uint8_t get_error(uint8_t err_type)
{
    if(err_type < ERR_CNT)
    {
        return boot_stub.err[err_type];
    }
    return 0;
}

void test_xram_error(void)
{
    set_error(ERR_XRam,1);
}

void test_sflash_error(void)
{
    set_error(ERR_SFlash,1);
}

void destroy_code_space(region_s *code)
{
    uint32_t i;
    uint8_t *buff = get_block_buffer();
    for(i = 0;i < BLOCK_SIZE;i ++)
    {
        buff[i] = 0xff;
    }
    write_block(code->type,code->index,code->addr,buff,1);
}

void test_pro1_error(void)
{
    boot_param_s *bp = (boot_param_s*)get_boot_params();
    destroy_code_space(&bp->mem_map.rom.program1_region);
}
void test_probak_error(void)
{
    boot_param_s *bp = (boot_param_s*)get_boot_params();
    destroy_code_space(&bp->mem_map.rom.program2_region);
}


void test_run_error(void)
{
    boot_param_s *bp = (boot_param_s*)get_boot_params();
    destroy_code_space(&bp->mem_map.run.flash);
}
void test_app_type_not_match(void)
{
    set_error(ERR_APP_TYPE,1);
}




void test_upgrade(void)
{
    region_s *img;

    boot_param_s *bp = (boot_param_s*)get_boot_params();
    extern mem_status_s g_memstatus;
    img = &bp->mem_map.ram.probuf_region;
    sys_printf("begin to receive file data,please wait.\r\n");
    img->lenth = boot_receive_img(img->addr,img->maxlen);
    if(img->lenth <= 0)
    {
        sys_error("receive img data failed.");
        return;
    }

    sp_init_share_param();

    g_upgrade_status.addr = img->addr;
    g_upgrade_status.lenth = img->lenth;
    g_upgrade_status.mem_type = img->type;
    g_upgrade_status.lenth = img->lenth;
    g_upgrade_status.crc = img->type;
    sp_set_upgrade_param(&g_upgrade_status);
    return;
}

void test_rollback(void)
{
    sp_set_app_rollback(1);
}
boot_test_s g_boottest[] = 
{
    {'1',"test XRAM error",test_xram_error},
    {'2',"test MEM_TYPE_ROM error",test_sflash_error},
    {'3',"test program1 error",test_pro1_error},
    {'4',"test program bak error",test_probak_error},
    {'5',"test running program error",test_run_error},
    {'6',"test upgrade program",test_upgrade},
    {'6',"test rollback program",test_rollback},
    {'7',"test app type NOT match",test_app_type_not_match},
};

void print32_t_boottest(void)
{
    int32_t i;
    sys_printf("choose test items:\r\n");
    for(i = 0;i < sizeof(g_boottest)/sizeof(boot_test_s);i ++)
    {
        sys_printf("[%c] %s\r\n",g_boottest[i].key,g_boottest[i].item_desc);
    }
}

int32_t test_entry(void)
{
    char ch;
    int32_t i;
    while(1)
    {
        print32_t_boottest();
        if(0 != read_char_blocking(&ch))
            return -1;
        for(i = 0;i < sizeof(g_boottest)/sizeof(boot_test_s);i ++)
        {
            if(ch == g_boottest[i].key)
            {
                g_boottest[i].test();
                return 0;
            }
        }
        return 0;
    }
}

#endif
#ifdef __cplusplus
}
#endif
