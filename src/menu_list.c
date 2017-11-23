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
#include "menu_list.h"
#include "boot_port.h"
#include "sys_debug.h"
#include "boot_param.h"
#include "boot_framework.h"
#include "sys_debug.h"

#include "crc.h"
#include "share_param.h"
#include "program_mgr.h"
#include "boot_test.h"
#include "boot_hw_if.h"

#ifdef __cplusplus
extern "C" {
#endif


extern int string_len(const char *str);
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
        sys_printf("%s?[y/n]\r\n",info);
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



static void download_img_to_rom(void)
{
    download_img_file(MEM_TYPE_ROM);    
}

static void download_img_to_ram(void)
{
    int32_t ret;
    boot_param_s *bp = (boot_param_s*)get_boot_params();
    if(bp->mem_map.run.ram.size <= 0)
    {
        sys_warn("img can not download to RAM,device NOT support.");
        return;
    }
    ret = download_img_file(MEM_TYPE_RAM);
    if(0 == ret)
    {
        g_go_ahead = 1;
    }
    set_boot_status(BOOT_SET_APP_PARAM);
    exit_menu();
}

static void download_filesystem(void)
{
    sys_warn("s-boot can NOT support file system right now.");
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
        sys_printf("set debug mode options:\r\n");
        for(i = 0;i < sizeof(mode)/sizeof(char*);i ++)
        {
            sys_printf("[%d] %s\r\n",i+1,mode[i]);
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
    sys_printf("current memory map info:\r\n");
    print_map_info(&bp->mem_map);
}

static void lock_mcu(void)
{
    boot_param_s *bp = (boot_param_s *)get_boot_params();
    if(is_chip_lock())
    {
        sys_notice("MCU has been locked before.");
        return;
    }
    set_chip_lock(1);
    exit_menu();
}

static void unlock_mcu(void)
{
    boot_param_s *bp = (boot_param_s *)get_boot_params();
    if(!is_chip_lock())
    {
        sys_notice("MCU has NOT been locked before.");
        return;
    }
    set_chip_lock(0);
    exit_menu();
}

static void show_program_status(void)
{
    boot_param_s *bp = (boot_param_s *)get_boot_params();
    print_program_space(&bp->mem_map);
}

#if BOOT_TEST_ENABLE
void bootloader_test(void)
{
    if(0 != test_entry())
    {
        exit_menu();
    }
}
#endif

static void do_clear_flash_data(uint8_t unlock)
{
    clean_program();
    param_init();
    (void)param_flush();
    
    param_clear_buffer();
    if(unlock)
        set_chip_lock(0);
}

static void clear_boot_param(void)
{
    if(make_sure_input("Are you sure to clear params"))
        do_clear_flash_data(0);
    sys_printf("clear boot param complete.\r\n");
}

static void exit_and_save(void)
{
    int32_t ret;
    
    ret = param_flush();
    if(ret != 0)
    {
        sys_printf("write param fialed.\r\n");
    }    //这里是否还需要对参数做进一步的验证
    exit_menu();
}


static menu_handle_TB g_menu_handleTB[] = 
{
    {'1',0,0,"download img file to ROM",download_img_to_rom},
    {'2',0,0,"download img file to RAM",download_img_to_ram},
    {'3',0,0,"download file system",download_filesystem},
    {'4',0,0,"show memory map",show_memmap},
    {'5',0,0,"show program status",show_program_status},
    {'b',2,2,"set debug mode",set_debug_mode},
    {'k',0,0,"lock MCU chip",lock_mcu},
    {'r',2,2,"clear boot params",clear_boot_param},
#if BOOT_TEST_ENABLE
    {'t',0,0,"bootloader test",bootloader_test},
#endif
    {'u',2,2,"unlock MCU chip",unlock_mcu},
    
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
    sys_printf("\r\n\r\nmenu list:\r\n");
    for(i = 0;i < sizeof(g_menu_handleTB)/sizeof(menu_handle_TB);i ++)
    {
        if(!g_menu_handleTB[i].prio)
        {
            sys_printf("[%c] %s\r\n",g_menu_handleTB[i].key,g_menu_handleTB[i].menu_item);
        }
    }
}

//开启超级权限，可以删除程序，修改重要参数等
int32_t open_super_prio(void)
{
    int32_t i;
    int len;
    int32_t prio = 0;
    char *prio1 = "test";
    char *prio2 = "sudo";
    uint8_t *buff = get_block_buffer();
    len = read_line_blockig((char*)buff,BLOCK_SIZE);
    if(is_string_equal((char*)buff,prio1,string_len(prio1)))
    {
        prio = 1;
    }
    else if(is_string_equal((char*)buff,prio2,string_len(prio2)))
    {
        prio = 2;
    }
    else
    {
        return -1;
    }
    
    for(i = 0;i < sizeof(g_menu_handleTB)/sizeof(menu_handle_TB);i ++)
    {
        if(g_menu_handleTB[i].prio <= prio)
            g_menu_handleTB[i].prio = 0;
    }
    super_prio_flag = 1;
    return 0;
}

void run_menu(void)
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
                sys_printf("\r\n");
                ret = open_super_prio();
                if(0 == ret)
                {
                    sys_notice("You have opened advanced function.");
                }
            }
        }
    }
}    



#ifdef __cplusplus
}
#endif
