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
#include "boot_framework.h"
#include "mem_map.h"
#include "boot_param.h"
#include "menu_list.h"
#include "port.h"
#include "share_param.h"
#include "sys_debug.h"
#include "crc.h"
#include "program_mgr.h"
#include "boot_hw_if.h"
#include "mem_driver.h"
#include "boot_check.h"
#ifdef __cplusplus
extern "C" {
#endif

static volatile int s_boot_status = BOOT_INIT;
mem_status_s g_memstatus;
upgrade_region_s g_upgrade_status;
sysparam_region_s g_sysparam_reg;

void print_boot_info(void)
{
    sys_printf("\r\n");
    sys_printf("+---------------------------------------------+\r\n");
    sys_printf("               s-boot %d.%d.%d\r\n",(uint8_t)(BOOT_VERSION >> 16),
                (uint8_t)(BOOT_VERSION >> 8),(uint8_t)(BOOT_VERSION));
    sys_printf("      *** To Make Ease For Developing ***\r\n");
    sys_printf("+---------------------------------------------+\r\n");
    sys_printf("Build: %s %s\r\n",__DATE__,__TIME__);
    sys_printf("Borad: %s\r\n",BOARD_NAME);
    sys_printf("Arch : %s\r\n",ARCH_NAME);
    sys_printf("CPU  : %s\r\n",CPU_NAME);
    sys_printf("\r\n\r\n");
}

static int32_t boot_init(void)
{
    print_boot_info();
	mem_region_init();
    param_clear_buffer();
    go_to_next_step();
    sys_notice("bootloader init OK.");
    return 0;
}

static int32_t boot_app_debug_check(void)
{
    int32_t dbg_mode = param_check_debug_mode();
    if(dbg_mode)
    {
        sys_warn("bootloader mode:DEBUG");
        set_boot_status(BOOT_WAIT_KEY_PRESS);
    }
    else
    {
        
        sys_notice("bootloader mode:NORMAL");
        go_to_next_step();
    }
    return 0;
}


static int32_t boot_first_check(void)
{
    int32_t ret;
    mem_map_s *map;
    boot_param_s *bp;
    sys_notice("begin to check first running time...");
    bp = (boot_param_s *)get_boot_params();
    if(NULL != bp)
    {
        go_to_next_step();
        sys_notice("find it is NOT the first running time.");
        return 0;
    }
    sys_notice("NO valid boot params found.");
    map = get_memory_map();
    print_map_info(map);
    param_init(map);
    ret = param_flush();
    if(0 != ret)
    {
        sys_error("write boot params failed.");
        set_boot_status(BOOT_ERROR);
        return -1;
    }
    go_to_next_step();
    sys_notice("check first running time OK.");
    return 0;
    
}

static int32_t boot_chip_lock_check(void)
{
    if(is_chip_lock())
    {
        sys_notice("MCU chip is locked.");
    }
    else
    {
        sys_warn("MCU chip is unlocked.");
    }
    go_to_next_step();
    return 0;
}



static int32_t boot_self_check(void)
{
    int32_t ret;
    boot_param_s *bp = (boot_param_s *)get_boot_params();
    ret = check_map_valid();
    if(ret)
    {
        sys_error("memory map params ERROR.");
        set_boot_status(BOOT_ERROR);
        return -1;        
    }
    ret = check_rom_programs();
    go_to_next_step();
    return ret;
}



static int32_t  boot_upgrade_check(void)
{
    int32_t ret;
    region_s img;
    boot_param_s *bp = (boot_param_s *)get_boot_params();
    
    if(NULL == bp)
    {
        sys_warn("get boot params failed.");
        return -1;
    }
    ret = sp_get_upgrade_param(&g_upgrade_status);
    if(0 != ret)
    {
        sys_notice("get upgrade params NULL.");
        sp_init_share_param();
        go_to_next_step();
        return 0;
    }
    
    if((!g_upgrade_status.flag))
    {
        sys_notice("upgrade flags is invalid,need NOT upgrade App.");
        go_to_next_step();
        return 0;
    }
    g_upgrade_status.flag = 0;
    sp_set_upgrade_param(&g_upgrade_status);
    
    sys_notice("handling upgrade event,please wait...");
    
    img.regname = bp->mem_map.ram.load_buffer.regname;
    img.size = bp->mem_map.ram.load_buffer.size;
    img.addr = g_upgrade_status.addr;
    img.datalen = g_upgrade_status.datalen;
    img.type = (memtype_e)g_upgrade_status.mem_type;

    ret = check_img_valid(&img);
    if(0 != ret)
    {
        sys_error("check img file ERROR");
        return -1;
    }
    
    if(MEM_TYPE_ROM == bp->mem_map.rom.sys_program1.type)
    {
        ret = flush_img_to_rom(&img);
    }
    else
    {
        sys_error("memory type ERROR.");
        set_boot_status(BOOT_ERROR);
        return -1;
    }
    if(0 != ret)
    {
        sys_warn("flush upgrade img failed.");
        set_boot_status(BOOT_ERROR);
        return -1;
    }
    
    ret = param_flush();
    if(0 != ret)
    {
        sys_error("update params failed.");
        set_boot_status(BOOT_ERROR);
        return -1;
    }
    go_to_next_step();
    return 0;
    
}

static int32_t  boot_rollback_check(void)
{
    uint8_t roll_flag;
    int32_t ret;
    boot_param_s *bp = (boot_param_s *)get_boot_params();
    
    sys_notice("begin to check app roll back status...");
    ret = sp_get_app_rollback(&roll_flag);
    if(0 != ret)
    {
        sys_notice("get upgrade params NULL.");
        sp_init_share_param();
        go_to_next_step();
        return 0;
    }
    if(!roll_flag)
    {
        sys_notice("roll back flag is invalid,need NOT to roll back.");
        go_to_next_step();
        return 0;
    }
    sys_notice("begin to roll back...");
    ret = roll_back_program();
    sp_set_app_rollback(0);
    return ret;
}


static int32_t boot_wait_key_press(void)
{
    char ch = 0;

    boot_param_s *bp = (boot_param_s *)get_boot_params();
    sys_printf("press any key to enter menu list:");
    if(0 == wait_for_key_input(bp->wait_sec,&ch,1))
    {
        go_to_next_step();
        sys_printf("\r\n");
        return 0;
    }
    set_boot_status(BOOT_LOAD_APP);
    sys_printf("\r\n");
    return 0;
}

static int32_t boot_menu_list(void)
{
    run_menu();
    if(get_menu_go_direction())
        go_to_next_step();
    else
        set_boot_status(BOOT_INIT);
    return 0;
}
static int32_t boot_load_app(void)
{
    mem_status_e mem_stat = MEM_ERROR;
    region_s *regi = NULL;
    boot_param_s *bp = NULL; 

    sys_notice("begin to load App to running space...");
    bp = (boot_param_s *)get_boot_params();
    regi = &bp->mem_map.run.flash;
    
    if(NULL == bp)
    {
        sys_error("get boot param failed.");
        set_boot_status(BOOT_ERROR);
        return -1;
    }
    
    // debug模式不需要检查，直接调入App
    if(bp->debug_mode)
    {
        go_to_next_step();
        return 0;
    }
    if(bp->mem_map.rom.sys_program1.datalen <= 0)
    {
        sys_notice("program is NOT existing.");
        set_boot_status(BOOT_MENU_LIST);
        return -1;
    }

    if(MEM_TYPE_ROM == bp->mem_map.run.flash.type)
    {
        if(MEM_NORMAL == bp->mem_map.run.flash.status)
        {
            mem_stat = MEM_NORMAL;
        }        
    }
    else 
    {
        if(MEM_NORMAL == bp->mem_map.rom.sys_program1.status)
        {
            mem_stat = MEM_NORMAL;
        }
    }
    if(MEM_NORMAL != mem_stat)
    {
        sys_warn("program has some ERRORs.");
        set_boot_status(BOOT_MENU_LIST);
        return -1;
    }

    if(MEM_TYPE_ROM == regi->type)
    {
        sys_notice("need not load App to a NORFlash ROM.");
        set_boot_status(BOOT_SET_APP_PARAM);
        return 0;
    }
    else
    {
        sys_error("running memory type is not supported.");
        set_boot_status(BOOT_ERROR);
        return -1;
    }
    
}

static int32_t boot_set_app_param(void)
{
    boot_param_s *bp = (boot_param_s *)get_boot_params();
    sys_notice("begin to set App params...");
    sp_init_share_param();
    
    sp_set_app_rollback(1);
    
    g_upgrade_status.addr = bp->mem_map.ram.load_buffer.addr;
    g_upgrade_status.flag = 0;
    g_upgrade_status.size = bp->mem_map.ram.load_buffer.size;
    g_upgrade_status.mem_type = bp->mem_map.ram.load_buffer.type;
    sp_set_upgrade_param(&g_upgrade_status);
    sp_get_upgrade_param(&g_upgrade_status);
    sys_printf("set upgrade params:\r\n");
    sys_printf("addr:0x%x\r\n",g_upgrade_status.addr);
    sys_printf("lenth:0x%x\r\n",g_upgrade_status.datalen);

    g_sysparam_reg.addr = bp->mem_map.rom.sys_param.addr;
    g_sysparam_reg.size = bp->mem_map.rom.sys_param.size;
    g_sysparam_reg.mem_type = bp->mem_map.rom.sys_param.type;
    sys_printf("set sysparam region params:\r\n");
    sys_printf("addr:0x%x\r\n",g_sysparam_reg.addr);
    sys_printf("lenth:0x%x\r\n",g_sysparam_reg.size);
    sp_set_sysparam_param(&g_sysparam_reg);
    
    sp_set_mem_status(&g_memstatus);
    sys_notice("set App params OK.");
    set_boot_status(BOOT_JUMP_TO_APP);
    return 0;
}


static int32_t boot_error_handle(void)
{
    char ch;
    while(1)
    {
        if(0 == read_char_blocking(&ch))
            sys_error("Some error occured in bootloader and system boot failed.");
    }
    return 0;
}
static int32_t boot_run_system(void)
{
	sys_notice("begin to jump to App space...");
	sys_printf("\r\n\r\n\r\n");
	boot_jump_to_app();
	return 0;
}


boot_handle_TB g_status_handTB[] = 
{
    {BOOT_INIT,"init",boot_init},
    {BOOT_FIRST_CHECK,"first_check",boot_first_check},   
    {BOOT_APP_DEBUG_CHECK,"app_debug_check",boot_app_debug_check},

    {BOOT_CHIP_LOCK_CHECK,"chip_lock_check",boot_chip_lock_check},
    {BOOT_SELF_CHECK,"self_check",boot_self_check},
    {BOOT_UPGRADE_CHECK,"upgrade_check",boot_upgrade_check},
    {BOOT_ROLLBACK_CHECK,"rollback_check",boot_rollback_check},
    
    {BOOT_WAIT_KEY_PRESS,"wait_key_press",boot_wait_key_press},
    {BOOT_MENU_LIST,"menu_list",boot_menu_list},
    {BOOT_LOAD_APP,"load_app",boot_load_app},
    {BOOT_SET_APP_PARAM,"set_app_param",boot_set_app_param},
    {BOOT_JUMP_TO_APP,"run_system",boot_run_system},
    {BOOT_ERROR,"error",boot_error_handle},
};


void set_boot_status(boot_status_e status)
{
    s_boot_status = status;
}

int get_boot_status(void)
{
    return s_boot_status;
}
void go_to_next_step(void)
{
    s_boot_status ++;
}

void boot_loop(void)
{
    int32_t i,ret;
    device_init();
    while(1)
    {
        for(i = 0;i < sizeof(g_status_handTB)/sizeof(boot_handle_TB);i ++)
        {
            if(s_boot_status == g_status_handTB[i].status)
            {
                sys_debug("step[%d] %s.",g_status_handTB[i].status+1,g_status_handTB[i].stepname);
                ret = g_status_handTB[i].handle();
                if(0 != ret)
                {
                    sys_printf("step[%d] %s captures some errors.\r\n",i + 1,g_status_handTB[i].stepname);
                }
                break;
            }
        }
        if(i >= sizeof(g_status_handTB)/sizeof(boot_handle_TB))
        {
            sys_error("unkown status %d.",s_boot_status);
            set_boot_status(BOOT_ERROR);
        }
    }
}


int32_t main(void)
{
    boot_loop();
    return 0;
}

#ifdef __cplusplus
}
#endif
