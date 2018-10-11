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
#ifndef BOOT_MENU_H__
#define BOOT_MENU_H__
#include "wind_type.h"
#include "boot_part.h"
#ifdef __cplusplus
extern "C" {
#endif



typedef w_err_t (*menu_handle_fn)(void);
typedef struct
{
    char key;//输入的键值
    char prio;//菜单权限
    char prio_bak;//菜单权限的备份
    const char *menu_item;//菜单项
    menu_handle_fn handle;//处理的句柄
}w_menu_tb_s;

extern w_menu_tb_s g_menu_list[];
void run_menu(void);
void print32_t_menu_list(void);
w_int32_t get_menu_go_direction(void);
w_int32_t flush_img_to_rom(w_part_s *code_part);

#ifdef __cplusplus
}
#endif

#endif
