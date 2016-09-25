/***************************************************************************************
** 文件名：
** 文件信息：
** 创建时间：2016.09.25
** 创建人：清风海岸
**文件版本 1.0
** 修改记录：
***************************************************************************************/
#include "menu_list.h"
#include "port.h"
#include "boot_debug.h"
#include "boot_param.h"
#include "boot_framework.h"
#include "boot_debug.h"

#include "crc.h"
#include "share_param.h"
#include "program_mgr.h"
#include "boot_test.h"
#include "boot_hw_if.h"

#ifdef __cplusplus
extern "C" {
#endif



//退出菜单标记，如果只为有效，回到菜单时将退出菜单
static uint8_t exit_menu_flag = 0;
//高级权限标志，拥有高级权限可以使用更多功能
static int32_t super_prio_flag = 0;
//退出菜单的的运行方向，如果不为0，则向下执行，否则重新初始化
static int32_t g_go_ahead = 0;
static void exit_menu(void);
static int32_t make_sure_input(char *info)
{
    char ch;
    while(1)
    {
        boot_printf("%s?[y/n]\r\n",info);
        if(0 != read_char_blocking(&ch))
        {
            exit_menu();
            return 0;
        }
        if('y' == ch)
            return 1;
        else if('n' == ch)
            return 0;
    }
}
int32_t get_menu_go_direction(void)
{
    return g_go_ahead;
}
static int32_t is_string_equal(char *str1,char *str2,int32_t len)
{
    int32_t i;
    for(i = 0;i < len;i ++)
    {
        if(str1[i] != str2[i])
            return 0;
    }
    return 1;
}
static void download_img_to_iflash(void)
{
    boot_param_s *bp = (boot_param_s*)get_boot_params();
    if(MEM_TYPE_ROM!= bp->mem_map.rom.program1_region.type)
    {
        boot_warn("img can not download to MEM_TYPE_ROM,device NOT support.");
        return;
    }
    download_img_file(DOWN_IFLASH);
}
static void download_img_to_sflash(void)
{
    boot_param_s *bp = (boot_param_s*)get_boot_params();
    if(MEM_TYPE_ROM!= bp->mem_map.rom.program1_region.type)
    {
        boot_warn("img can not download to MEM_TYPE_ROM,device NOT support.");
        return;
    }
    download_img_file(DOWN_SFLASH);
}

static void download_img_to_iram(void)
{
    int32_t ret;
    boot_param_s *bp = (boot_param_s*)get_boot_params();
    if(MEM_TYPE_RAM != bp->mem_map.run.flash.type)
    {
        boot_warn("img can not download to MEM_TYPE_RAM,device NOT support.");
        return;
    }
    ret = download_img_file(DOWN_IRAM);
    if(0 == ret)
    {
        g_go_ahead = 1;
    }
    set_boot_status(BOOT_SET_APP_PARAM);
    exit_menu();
}

static void download_img_to_xram(void)
{
    int32_t ret;
    boot_param_s *bp = (boot_param_s*)get_boot_params();
    if(MEM_TYPE_RAM != bp->mem_map.run.flash.type)
    {
        boot_warn("img can not download to XRAM,device NOT support.");
        return;
    }
    ret = download_img_file(DOWN_XRAM);
    if(0 == ret)
    {
        g_go_ahead = 1;
    }
    set_boot_status(BOOT_SET_APP_PARAM);
    exit_menu();
}


static void set_debug_mode(void)
{
    int32_t i;
    char ch;
    char *mode[] = 
    {
        "Normal",
        "Debug",
    };
    boot_param_s *bp = (boot_param_s*)get_boot_params();
    while(1)
    {
        boot_printf("set debug mode options:\r\n");
        for(i = 0;i < sizeof(mode)/sizeof(char*);i ++)
        {
            boot_printf("[%d] %s\r\n",i+1,mode[i]);
        }
        if(0 != read_char_blocking(&ch))
        {
            exit_menu();
            return;
        }
        if(ch >= '1' && ch <= '0' + sizeof(mode)/sizeof(char*))
        {
            if(ch == '1')
                bp->debug_mode = 0;
            else if(ch == '2')
                bp->debug_mode = 1;
            break;
        }
    }
    
}


static void show_memmap(void)
{
    boot_param_s *bp = (boot_param_s *)get_boot_params();
    boot_printf("current memory map info:\r\n");
    print_map_info(&bp->mem_map);
}

static void lock_MCU(void)
{
    boot_param_s *bp = (boot_param_s *)get_boot_params();
    if(is_chip_lock())
    {
        boot_notice("MCU has been locked before.");
        return;
    }
    set_chip_lock(1);
    exit_menu();
}

static void show_program_status(void)
{
    boot_param_s *bp = (boot_param_s *)get_boot_params();
    print_program_space(&bp->mem_map);
}

#if BOOT_TEST_ENABLE
static void set_bootloader_error_test(void)
{
    if(0 != test_entry())
    {
        exit_menu();
    }
}
#endif

static void do_clear_flash_data(uint8_t unlock)
{
    boot_param_s *bp = (boot_param_s*)get_boot_params();
    clean_program();
    
    init_boot_param(&bp->mem_map);
    (void)write_param();
    
    clear_boot_param_buffer();
    if(unlock)
        set_chip_lock(0);
}

static void clear_boot_param(void)
{
    if(make_sure_input("Are you sure to unlock MCU"))
        do_clear_flash_data(0);
    boot_printf("clear boot param complete.\r\n");
}

static void set_default_boot_img(void)
{
    char ch;
    int32_t i,ret;
    char *pro[] = 
    {
        "program1",
        "program2",
        "cancel"
    };
    boot_param_s *bp = (boot_param_s *)get_boot_params();
    while(1)
    {
        boot_printf("choose the following program:\r\n");
        for(i = 0;i < sizeof(pro)/sizeof(char*);i ++)
        {
            boot_printf("[%d] %s\r\n",i+1,pro[i]);
        }
        if(0 != read_char_blocking(&ch))
        {
            exit_menu();
            return;
        }
        if(ch >= '1' && ch <= '0' + sizeof(pro)/sizeof(char*))
        {
            if(ch != '0' + sizeof(pro)/sizeof(char*))
            {
                ret = change_boot_app(ch - '1');
                if(0 == ret)
                {
                    boot_notice("set default boot img OK.");
                }
                else
                {
                    boot_warn("set boot app error.");
                }
            }
                
            break;
        }
    }
}


static void exit_and_save(void)
{
    int32_t ret;
    
    ret = write_param();
    if(ret != 0)
    {
        boot_printf("write param fialed.\r\n");
    }    //这里是否还需要对参数做进一步的验证
    exit_menu();
}


static menu_handle_TB g_menu_handleTB[] = 
{
    {'1',0,0,"download img file to irom",download_img_to_iflash},
    {'2',0,0,"download img file to xrom",download_img_to_sflash},
    {'3',0,0,"download img file to iram",download_img_to_iram},
    {'4',0,0,"download img file to xram",download_img_to_xram},
    
    {'b',2,2,"set debug mode",set_debug_mode},
    {'d',0,0,"show memory map",show_memmap},
    {'k',0,0,"lock MCU chip",lock_MCU},
    {'p',0,0,"show program status",show_program_status},
    {'r',2,2,"clear boot params",clear_boot_param},
#if BOOT_TEST_ENABLE
    {'t',0,0,"set bootloader test",set_bootloader_error_test},
#endif
    {'s',1,1,"set default boot img",set_default_boot_img},
    //{'u',2,2,"unlock MCU chip",unlock_mcu},
    
    {'x',1,1,"exit menu and save",exit_and_save},
    {'q',0,0,"exit menu",exit_menu}
};

static void exit_menu(void)
{
    int32_t i;
    exit_menu_flag = 1;
    if(super_prio_flag)
    {
        for(i = 0;i < sizeof(g_menu_handleTB)/sizeof(menu_handle_TB);i ++)
        {
            g_menu_handleTB[i].prio = g_menu_handleTB[i].prio_bak;
        }
    }
}
void print32_t_menu_list(void)
{
    int32_t i;
    boot_printf("\r\n\r\npress key to choose the following functions:\r\n");
    for(i = 0;i < sizeof(g_menu_handleTB)/sizeof(menu_handle_TB);i ++)
    {
        if(!g_menu_handleTB[i].prio)
        {
            boot_printf("[%c] %s\r\n",g_menu_handleTB[i].key,g_menu_handleTB[i].menu_item);
        }
    }
}

//开启超级权限，可以删除程序，修改重要参数等
int32_t open_super_prio(void)
{
    int32_t i;
    char cmdbuf[6];
    int32_t prio = 0;
    char *prio1 = "test";
    char *prio2 = "sudo";
    read_line_blockig(cmdbuf,10);
    if(is_string_equal(cmdbuf,prio1,4))
    {
        prio = 1;
    }
    else if(is_string_equal(cmdbuf,prio2,4))
    {
        prio = 2;
    }
    else
    {
        return -1;
    }
    
    for(i = 0;i < sizeof(g_menu_handleTB)/sizeof(menu_handle_TB);i ++)
    {
        //g_menu_handleTB[i].prio_bak = g_menu_handleTB[i].prio;
        if(g_menu_handleTB[i].prio <= prio)
            g_menu_handleTB[i].prio = 0;
    }
    super_prio_flag = 1;
    return 0;
}

void menu_entry(void)
{
    char ch;
    int32_t i,ret;
    
    exit_menu_flag = 0;
    g_go_ahead = 0;
    while(0 == exit_menu_flag)
    {
        print32_t_menu_list();
        if(0 != read_char_blocking(&ch))
        {
            exit_menu();
            return;
        }
        for(i = 0;i < sizeof(g_menu_handleTB)/sizeof(menu_handle_TB);i ++)
        {
            if(ch == g_menu_handleTB[i].key && 0 == g_menu_handleTB[i].prio)
            {
                g_menu_handleTB[i].handle();
                break;
            }
        }
        if(i >= sizeof(g_menu_handleTB)/sizeof(menu_handle_TB))
        {
            if(ch == '0')
            {
                boot_printf("\r\n");
                ret = open_super_prio();
                if(0 == ret)
                {
                    boot_printf("You have got some advanced priority. ^_^\r\n");
                }
            }
            //else
            //    boot_warn("no such an option to handle:%c.",ch);
        }
    }
}    



#ifdef __cplusplus
}
#endif
