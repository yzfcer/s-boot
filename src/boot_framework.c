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
#include "boot_debug.h"
#include "crc.h"
#include "program_mgr.h"
#include "boot_hw_if.h"
#include "mem_driver.h"

#ifdef __cplusplus
extern "C" {
#endif

static volatile int s_boot_status = BOOT_INIT;
mem_status_s g_memstatus;
upgrade_region_s g_upgrade_status;
reserve_region_s g_reserve_reg;

void print_boot_info(void)
{
    boot_printf("\r\n");
    boot_printf("s-boot %d.%d\r\n",(uint8_t)(BOOT_VERSION >> 8),(uint8_t)(BOOT_VERSION));
    boot_printf("*** To Make ease For developing ***\r\n");
    boot_printf("Build: %s %s\r\n",__DATE__,__TIME__);
    boot_printf("Borad: %s\r\n",BOARD_NAME);
    boot_printf("Arch : %s\r\n",ARCH_NAME);
    boot_printf("CPU  : %s\r\n",CPU_NAME);
    boot_printf("\r\n\r\n");
}

static int32_t boot_init(void)
{
    print_boot_info();
	mem_region_init();
    clear_boot_param_buffer();
    go_to_next_step();
    boot_notice("bootloader init OK.");
    return 0;
}

static int32_t boot_app_debug_check(void)
{
    int32_t dbg_mode = check_app_debug_mode();
    if(dbg_mode)
    {
        boot_warn("bootloader mode:DEBUG");
        set_boot_status(BOOT_WAIT_KEY_PRESS);
    }
    else
    {
        
        boot_notice("bootloader mode:NORMAL");
        go_to_next_step();
    }
    return 0;
}

static int32_t boot_first_check(void)
{
    int32_t ret;
    mem_map_s *map;
    boot_param_s *bp = (boot_param_s *)get_boot_params();
    if(NULL == bp)
    {
        bp = (boot_param_s *)get_boot_params();
    }
    boot_notice("begin to check first running time...");
    if(NULL != bp)
    {
        go_to_next_step();
        boot_notice("find it is NOT the first running time.");
        return 0;
    }
    boot_notice("NO valid boot params found.");
    map = get_memory_map();
    print_map_info(map);
    init_boot_param(map);
    ret = write_param();
    if(0 != ret)
    {
        boot_error("write boot params failed.");
        set_boot_status(BOOT_ERROR);
        return -1;
    }
    go_to_next_step();
    boot_notice("check first running time OK.");
    return 0;
    
}
static int32_t boot_chip_lock_check(void)
{
    if(!is_chip_lock())
    {
        boot_warn("MCU chip is locked.");
    }
    else
    {
        boot_notice("MCU chip is unlocked.");
    }
    go_to_next_step();
    return 0;
}

int32_t repair_rom_space(region_s *src,region_s *dest)
{
    int32_t ret;
    
    if(MEM_NORMAL != src->status)
    {
        boot_error("can NOT find available source to repir program1.");
        src = NULL;
        return -1;
    }
    
    boot_notice("repair program from %s to %s",src->regname,dest->regname);
    if(dest->type == src->type)
        ret = copy_region_data(src,dest);
    else if(MEM_TYPE_ROM == src->type || MEM_TYPE_ROM == dest->type)
        ret = write_encrypt_code_to_run(src,dest);
    else
    {
        boot_warn("can not repair space.");
        ret = -1;
    }
    
    if(0 != ret)
    {
        boot_error("repir space %s base 0x%x,lenth %d failed.",
                    memtype_name(dest->type),dest->addr,dest->maxlen);
        return -1;
    }
    return 0;
}


int32_t repair_running_space(boot_param_s *bp)
{
    int32_t ret;
    region_s *src,*dest;
    
    dest = &bp->mem_map.run.flash;
    if(MEM_NORMAL == bp->mem_map.rom.program1_region.status)
        src = &bp->mem_map.rom.program1_region;
    else if(MEM_NORMAL == bp->mem_map.rom.program2_region.status)
        src = &bp->mem_map.rom.program2_region;
    else
        src = NULL;
    
    if(NULL == src)
    {
        boot_warn("can not find an available source for repairing.");
        ret = -1;
    }
    else
    {
        boot_notice("repair program from %s to %s",src->regname,dest->regname);
        ret = repair_rom_space(src,dest);
    }
    return ret;
}




static int32_t repair_program(boot_param_s *bp)
{
    int32_t ret = 0;
    boot_notice("programs has errors,try to repair ...");
    if(MEM_ERROR == bp->mem_map.run.flash.status)
    {
        if(0 != repair_running_space(bp))
            ret = -1;
    }
    if(MEM_ERROR == bp->mem_map.rom.program1_region.status)
    {
        if(0 != repair_rom_space(&bp->mem_map.rom.program2_region,&bp->mem_map.rom.program1_region))
            ret = -1;
    }
    if(MEM_ERROR == bp->mem_map.rom.program2_region.status)
    {
        if(0 != repair_rom_space(&bp->mem_map.rom.program1_region,&bp->mem_map.rom.program2_region))
            ret = -1;
    }
    (void)write_param();
    return ret;
}
static int32_t boot_self_check(void)
{
    int32_t ret;
    boot_param_s *bp = (boot_param_s *)get_boot_params();
    ret = check_map_valid();
    if(ret)
    {
        boot_error("memory map params ERROR.");
        set_boot_status(BOOT_ERROR);
        return -1;        
    }
        
    ret = check_programs();
    if(0 != ret)
    {
        ret = repair_program(bp);
        if(0 != ret)
        {
            boot_error("repairing program failed");
            go_to_next_step();
            return -1;
        }
    }
    go_to_next_step();
    return 0;
}



static int32_t  boot_upgrade_check(void)
{
    int32_t ret;
    region_s img;
    boot_param_s *bp = (boot_param_s *)get_boot_params();
    
    if(NULL == bp)
    {
        boot_warn("get boot params failed.");
        return -1;
    }
    ret = sp_get_upgrade_param(&g_upgrade_status);
    if(0 != ret)
    {
        boot_notice("get upgrade params NULL.");
        sp_init_share_param();
        go_to_next_step();
        return 0;
    }
    
    if((!g_upgrade_status.flag))
    {
        boot_notice("upgrade flags is invalid,need NOT upgrade App.");
        go_to_next_step();
        return 0;
    }
    g_upgrade_status.flag = 0;
    sp_set_upgrade_param(&g_upgrade_status);
    
    boot_notice("handling upgrade event,please wait...");
    
    img.regname = bp->mem_map.ram.probuf_region.regname;
    img.maxlen = bp->mem_map.ram.probuf_region.maxlen;
    img.addr = g_upgrade_status.addr;
    img.lenth = g_upgrade_status.lenth;
    img.type = (memtype_e)g_upgrade_status.mem_type;

    ret = check_and_decrypt_img(&img);
    if(0 != ret)
    {
        boot_error("check img file ERROR");
        return -1;
    }
    
    if(MEM_TYPE_ROM == bp->mem_map.rom.program1_region.type)
    {
        ret = flush_code_to_iflash(&img);
    }
    else
    {
        boot_error("memory type ERROR.");
        set_boot_status(BOOT_ERROR);
        return -1;
    }
    if(0 != ret)
    {
        boot_warn("flush upgrade img failed.");
        set_boot_status(BOOT_ERROR);
        return -1;
    }
    
    ret = write_param();
    if(0 != ret)
    {
        boot_error("update params failed.");
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
    
    boot_notice("begin to check app roll back status...");
    ret = sp_get_app_rollback(&roll_flag);
    if(0 != ret)
    {
        boot_notice("get upgrade params NULL.");
        sp_init_share_param();
        go_to_next_step();
        return 0;
    }
    if(!roll_flag)
    {
        boot_notice("roll back flag is invalid,need NOT to roll back.");
        go_to_next_step();
        return 0;
    }
    sp_set_app_rollback(0);
    ret = change_boot_app(APP_IDX_PROBAK);
    if(0 != ret)
    {
        boot_error("app roll back failed.");
        set_boot_status(BOOT_ERROR);
    }
    else
    {
        boot_notice("check app roll back OK.");
        go_to_next_step();
    }
    return ret;
}


static int32_t boot_wait_key_press(void)
{
    char ch = 0;
    boot_printf("press any key to enter menu list:");
    if(0 == wait_for_key_input(3,&ch,1))
    {
        go_to_next_step();
        boot_printf("\r\n");
        return 0;
    }
    set_boot_status(BOOT_LOAD_APP);
    boot_printf("\r\n");
    return 0;
}

static int32_t boot_menu_list(void)
{
    menu_entry();
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

    boot_notice("begin to load App to running space...");
    bp = (boot_param_s *)get_boot_params();
    regi = &bp->mem_map.run.flash;
    
    if(NULL == bp)
    {
        boot_error("get boot param failed.");
        set_boot_status(BOOT_ERROR);
        return -1;
    }
    
    // debug模式不需要检查，直接调入App
    if(bp->debug_mode)
    {
        go_to_next_step();
        return 0;
    }
    if(bp->mem_map.rom.program1_region.lenth <= 0)
    {
        boot_notice("program is NOT existing.");
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
        if(MEM_NORMAL == bp->mem_map.rom.program1_region.status)
        {
            mem_stat = MEM_NORMAL;
        }
    }
    if(MEM_NORMAL != mem_stat)
    {
        boot_warn("program has some ERRORs.");
        set_boot_status(BOOT_MENU_LIST);
        return -1;
    }

    if(MEM_TYPE_ROM == regi->type)
    {
        boot_notice("need not load App to a NORFlash ROM.");
        set_boot_status(BOOT_SET_APP_PARAM);
        return 0;
    }
    else
    {
        boot_error("running memory type is not supported.");
        set_boot_status(BOOT_ERROR);
        return -1;
    }
    
}

static int32_t boot_set_app_param(void)
{
    boot_param_s *bp = (boot_param_s *)get_boot_params();
    boot_notice("begin to set App params...");
    sp_init_share_param();
    
    sp_set_app_rollback(1);
    
    g_upgrade_status.addr = bp->mem_map.ram.probuf_region.addr;
    g_upgrade_status.crc = 0xffffffff;
    g_upgrade_status.flag = 0;
    g_upgrade_status.lenth = bp->mem_map.ram.probuf_region.maxlen;
    g_upgrade_status.mem_type = bp->mem_map.ram.probuf_region.type;
    sp_set_upgrade_param(&g_upgrade_status);
    sp_get_upgrade_param(&g_upgrade_status);
    boot_printf("set upgrade params:\r\n");
    boot_printf("addr:0x%x\r\n",g_upgrade_status.addr);
    boot_printf("lenth:0x%x\r\n",g_upgrade_status.lenth);

    g_reserve_reg.addr = bp->mem_map.rom.reserve_region.addr;
    g_reserve_reg.lenth = bp->mem_map.rom.reserve_region.maxlen;
    g_reserve_reg.mem_type = bp->mem_map.rom.reserve_region.type;
    boot_printf("set reserve region params:\r\n");
    boot_printf("addr:0x%x\r\n",g_reserve_reg.addr);
    boot_printf("lenth:0x%x\r\n",g_reserve_reg.lenth);
    sp_set_reserve_param(&g_reserve_reg);
    
    sp_set_mem_status(&g_memstatus);
    boot_notice("set App params OK.");
    set_boot_status(BOOT_JUMP_TO_APP);
    return 0;
}


static int32_t boot_error_handle(void)
{
    char ch;
    while(1)
    {
        if(0 == read_char_blocking(&ch))
            boot_error("Some error occured in bootloader and system boot failed.");
    }
    return 0;
}
static int32_t boot_run_system(void)
{
	boot_notice("begin to jump to App space...");
	boot_printf("(^_^)\r\n\r\n\r\n");
	boot_jump_to_app();
	return 0;
}


boot_handle_TB g_status_handTB[] = 
{
    {BOOT_INIT,"init",boot_init},
    {BOOT_APP_DEBUG_CHECK,"app_debug_check",boot_app_debug_check},
    {BOOT_FIRST_CHECK,"first_check",boot_first_check},
    {BOOT_CHIP_LOCK_CHECK,"chip_lock_check",boot_chip_lock_check},
    {BOOT_SELF_CHECK,"self_check",boot_self_check},
    {BOOT_UPGRADE_CHECK,"upgrade_check",boot_upgrade_check},
    {BOOT_ROLLBACK_CHECK,"rollback_check",boot_rollback_check},
    
    {BOOT_WAIT_KEY_PRESS,"wait_key_press",boot_wait_key_press},
    {BOOT_MENU_LIST,"menu_list",boot_menu_list},
    {BOOT_LOAD_APP,"load_app",boot_load_app},
    {BOOT_SET_APP_PARAM,"set_app_param",boot_set_app_param},
    {BOOT_JUMP_TO_APP,"jump_to_app",boot_run_system},
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
                boot_debug("step[%d] %s.",g_status_handTB[i].status+1,g_status_handTB[i].stepname);
                ret = g_status_handTB[i].handle();
                if(0 != ret)
                {
                    boot_printf("step[%d] %s captures some errors.\r\n",i + 1,g_status_handTB[i].stepname);
                }
                break;
            }
        }
        if(i >= sizeof(g_status_handTB)/sizeof(boot_handle_TB))
        {
            boot_error("unkown status %d.",s_boot_status);
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
