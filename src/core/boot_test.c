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
#include "boot_param.h"
#include "boot_port.h"
#include "wind_debug.h"
#include "wind_crc32.h"
#include "boot_part.h"
#include "boot_test.h"
#include "share_param.h"
#include "boot_hw_if.h"
#include "program_mgr.h"

#ifdef __cplusplus
extern "C" {
#endif
#if BOOT_TEST_ENABLE
extern upgrade_info_s g_upgrade_info;
boot_stub_s boot_stub = {0,0,0};
void clear_errors(void)
{
    
}

void set_error(w_uint8_t err_type,w_uint8_t err)
{
    if(err_type < ERR_CNT)
    {
        boot_stub.err[err_type] = err;
    }
}
w_uint8_t get_error(w_uint8_t err_type)
{
    if(err_type < ERR_CNT)
    {
        return boot_stub.err[err_type];
    }
    return 0;
}


void destroy_code_space(w_part_s *code)
{
    boot_part_erase(code);
}

void test_pro1_error(void)
{
    w_part_s *reg = boot_part_get(PART_IMG1);
    destroy_code_space(reg);
}
void test_probak_error(void)
{
    w_part_s *reg = boot_part_get(PART_IMG2);
    destroy_code_space(reg);
}


void test_run_error(void)
{
    w_part_s *reg = boot_part_get(PART_ROMRUN);
    destroy_code_space(reg);
}

void test_upgrade(void)
{
    w_part_s *img;

    img = boot_part_get(PART_CACHE);
    wind_printf("begin to receive file data,please wait.\r\n");
    img->datalen = boot_receive_img(img);
    if(img->datalen <= 0)
    {
        wind_error("receive img data failed.");
        return;
    }
    
    sp_init_share_param();
    g_upgrade_info.flag = 1;
    g_upgrade_info.addr = img->base;
    g_upgrade_info.datalen = img->datalen;
    g_upgrade_info.mem_type = img->mtype;
    sp_set_upgrade_param(&g_upgrade_info);
    return;
}

void test_rollback(void)
{
    sp_set_app_rollback(1);
}
boot_test_s g_boottest[] = 
{
    {'1',"test program1 error",test_pro1_error},
    {'2',"test program bak error",test_probak_error},
    {'3',"test running program error",test_run_error},
    {'4',"test upgrade program",test_upgrade},
    {'5',"test rollback program",test_rollback},
};

void print32_t_boottest(void)
{
    w_int32_t i;
    wind_printf("choose test items:\r\n");
    for(i = 0;i < sizeof(g_boottest)/sizeof(boot_test_s);i ++)
    {
        wind_printf("[%c] %s\r\n",g_boottest[i].key,g_boottest[i].item_desc);
    }
}

w_int32_t test_entry(void)
{
    char ch;
    w_int32_t i;
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
