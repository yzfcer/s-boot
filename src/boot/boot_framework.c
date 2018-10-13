/*********************************************************************************
  *Copyright(C),2017-2020,yzfcer@163.com
  *FileName:  
  *Author:      Jason Zhou
  *Version:     1.0
  *Date:        2017/04/08
  *Description:  
  *Others:  
  *History:  
     1.Date:
       Author:
       Modification:
**********************************************************************************/
#include "boot_framework.h"
#include "boot_param.h"
#include "boot_menu.h"
#include "boot_port.h"
#include "boot_share_param.h"
#include "boot_part.h"
#include "boot_media.h"
#include "wind_debug.h"
#include "wind_crc32.h"
#include "boot_img.h"
#include "boot_hw_if.h"
#include "wind_string.h"
#ifdef __cplusplus
extern "C" {
#endif

static volatile w_int32_t g_boot_status = BOOT_INIT;
void boot_status_set(boot_status_e status)
{
    g_boot_status = status;
}

w_int32_t boot_status_get(void)
{
    return g_boot_status;
}
void boot_status_go_next(void)
{
    g_boot_status ++;
}

void print_boot_info(void)
{
    wind_printf("\r\n");
    wind_printf("+---------------------------------------------+\r\n");
    wind_printf("               wind-boot %d.%d.%d\r\n",(w_uint8_t)(BOOT_VERSION >> 16),
                (w_uint8_t)(BOOT_VERSION >> 8),(w_uint8_t)(BOOT_VERSION));
    wind_printf("      *** To Make Ease For Developing ***\r\n");
    wind_printf("+---------------------------------------------+\r\n");
    wind_printf("Build: %s %s\r\n",__DATE__,__TIME__);
    wind_printf("Borad: %s\r\n",BOARD_NAME);
    wind_printf("Arch : %s\r\n",ARCH_NAME);
    wind_printf("CPU  : %s\r\n\r\n",CPU_NAME);
}

static w_int32_t boot_init(void)
{
    boot_param_init();
    boot_media_init();
    boot_part_init();
    boot_status_go_next();
    return 0;
}

static w_int32_t boot_debug_mode_check(void)
{
    boot_param_s *bp = boot_param_get();
    if(bp->debug_mode)
    {
        wind_warn("bootloader mode:DEBUG");
        boot_status_set(BOOT_WAIT_KEY_PRESS);
    }
    else
    {
        
        wind_notice("bootloader mode:NORMAL");
        boot_status_go_next();
    }
    return 0;
}


static w_int32_t boot_first_check(void)
{
    w_int32_t ret;
    boot_param_s *bp;
    bp = boot_param_from_rom();
    if(W_NULL != bp)
    {
        boot_status_go_next();
        wind_notice("find it is NOT the first running time.");
        return 0;
    }
    wind_notice("NO valid boot params found.");
    boot_param_reset();
    ret = boot_param_flush();
    if(0 != ret)
    {
        wind_error("write boot params failed.");
        boot_status_set(BOOT_ERROR);
        return -1;
    }
    boot_status_go_next();
    wind_notice("check first running time OK.");
    return 0;
    
}

static w_int32_t boot_chip_lock_check(void)
{
    if(is_chip_lock())
    {
        wind_notice("MCU chip is locked.");
    }
    else
    {
        wind_warn("MCU chip is unlocked.");
    }
    boot_status_go_next();
    return 0;
}



static w_int32_t boot_img_valid_check(void)
{
    w_int32_t ret;
    ret = boot_img_check();
    boot_status_go_next();
    return ret;
}



static w_int32_t  boot_upgrade_check(void)
{
    w_int32_t ret;
    w_part_s img,*tmp;
     share_param_s *sp = share_param_get();
    
    if(!sp->upgrade_flag)
    {
        wind_notice("upgrade flags is invalid,need NOT update image.");
        boot_status_go_next();
        return 0;
    }

    sp->upgrade_flag = 0;
    share_param_flush(sp);
    wind_notice("handling upgrade event,please wait...");
    tmp = boot_part_get(PART_CACHE);
    
    wind_error("--------Upgrade error----------");
    
    ret = check_img_valid(&img);
    if(0 != ret)
    {
        wind_error("check img file ERROR");
        return -1;
    }
    tmp = boot_part_get(PART_IMG1);
    if(MEDIA_TYPE_ROM == tmp->mtype)
    {
        ret = flush_img_to_rom(&img);
    }
    else
    {
        wind_error("memory type ERROR.");
        boot_status_set(BOOT_ERROR);
        return -1;
    }
    if(0 != ret)
    {
        wind_warn("flush upgrade img failed.");
        boot_status_set(BOOT_ERROR);
        return -1;
    }
    
    ret = boot_param_flush();
    if(0 != ret)
    {
        wind_error("update params failed.");
        boot_status_set(BOOT_ERROR);
        return -1;
    }
    boot_status_go_next();
    return 0;
    
}


static w_int32_t boot_wait_key_press(void)
{
    char ch = 0;

    boot_param_s *bp = (boot_param_s *)boot_param_get();
    wind_printf("press any key to enter menu list:");
    if(0 == wait_for_key_input(bp->wait_sec,&ch,1))
    {
        boot_status_go_next();
        wind_printf("\r\n");
        return 0;
    }
    boot_status_set(BOOT_LOAD_IMG);
    wind_printf("\r\n");
    return 0;
}

static w_int32_t boot_menu_list(void)
{
    run_menu();
    if(get_menu_go_direction())
        boot_status_go_next();
    else
        boot_status_set(BOOT_INIT);
    return 0;
}
static w_int32_t boot_load_img(void)
{
    w_mem_status_e mem_stat = MEM_ERROR;
    w_part_s *part = W_NULL,*tmp;
    boot_param_s *bp = W_NULL; 

    wind_notice("begin to load image to running space...");
    bp = (boot_param_s *)boot_param_get();
    part = boot_part_get(PART_SYSRUN);
    
    if(W_NULL == bp)
    {
        wind_error("get boot param failed.");
        boot_status_set(BOOT_ERROR);
        return -1;
    }
    
    // debug模式不需要检查，直接调入image
    if(bp->debug_mode)
    {
        boot_status_go_next();
        return 0;
    }
    tmp = boot_part_get(PART_IMG1);
    if(tmp->datalen <= 0)
    {
        wind_notice("program is NOT existing.");
        boot_status_set(BOOT_MENU_LIST);
        return -1;
    }
	tmp = boot_part_get(PART_SYSRUN);
    if(MEDIA_TYPE_ROM == tmp->mtype)
    {
        if(MEM_NORMAL == tmp->status)
        {
            mem_stat = MEM_NORMAL;
        }        
    }
    else 
    {
        tmp = boot_part_get(PART_IMG1);
        if(MEM_NORMAL == tmp->status)
        {
            mem_stat = MEM_NORMAL;
        }
    }
    if(MEM_NORMAL != mem_stat)
    {
        wind_warn("program has some ERRORs.");
        boot_status_set(BOOT_MENU_LIST);
        return -1;
    }

    if(MEDIA_TYPE_ROM == part->mtype)
    {
        wind_notice("need not load image to ROM.");
        boot_status_set(BOOT_SET_SHARE_PARAM);
        return 0;
    }
    else
    {
        wind_error("running memory type is not supported.");
        boot_status_set(BOOT_ERROR);
        return -1;
    }
    
}

static w_int32_t boot_set_system_param(void)
{
    w_part_s *tmp;
    wind_notice("set share params for system");
    share_param_init();
    boot_status_set(BOOT_RUN_SYSTEM);
    return 0;
}


static w_int32_t boot_error_handle(void)
{
    char ch;
    while(1)
    {
        if(0 == read_char_blocking(&ch))
            wind_error("Some error occured in bootloader and system boot failed.");
    }
    return 0;
}

static w_int32_t boot_run_system(void)
{
    boot_exit_hook();
	wind_notice("jump to system running space\r\n\r\n\r\n");
	boot_jump_to_system();
	return 0;
}


boot_handle_TB g_status_handTB[] = 
{
    {BOOT_INIT,"boot init",boot_init},
    {BOOT_FIRST_CHECK,"first run chack",boot_first_check},   
    {BOOT_DEBUG_MODE_CHECK,"debug mode check",boot_debug_mode_check},

    {BOOT_CHIP_LOCK_CHECK,"chip lock status check",boot_chip_lock_check},
    {BOOT_IMG_VALID_CHECK,"image valid check",boot_img_valid_check},
    {BOOT_UPGRADE_CHECK,"upgrade status check",boot_upgrade_check},
    
    {BOOT_WAIT_KEY_PRESS,"wait for any key press",boot_wait_key_press},
    {BOOT_MENU_LIST,"enter menu list",boot_menu_list},
    {BOOT_LOAD_IMG,"load image",boot_load_img},
    {BOOT_SET_SHARE_PARAM,"set share param",boot_set_system_param},
    {BOOT_RUN_SYSTEM,"run system",boot_run_system},
    {BOOT_ERROR,"error",boot_error_handle},
};



void boot_loop(void)
{
    w_int32_t i,ret;
    boot_enter_main_hook();
    print_boot_info();
    while(1)
    {
        for(i = 0;i < sizeof(g_status_handTB)/sizeof(boot_handle_TB);i ++)
        {
            if(g_boot_status == g_status_handTB[i].status)
            {
                wind_printf("[step%-2d] %-48s\r\n",g_status_handTB[i].status+1,
                            g_status_handTB[i].stepname);
                ret = g_status_handTB[i].handle();
                wind_notice("%-48s [%s]\r\n",g_status_handTB[i].stepname,ret?"ERROR":"OK");
                break;
            }
        }
        if(i >= sizeof(g_status_handTB)/sizeof(boot_handle_TB))
        {
            wind_error("unkown status %d.",g_boot_status);
            boot_status_set(BOOT_ERROR);
        }
    }
}


w_int32_t main(void)
{
    boot_loop();
    return 0;
}

#ifdef __cplusplus
}
#endif
