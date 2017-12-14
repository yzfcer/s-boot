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
#ifndef MENU_LIST_H__
#define MENU_LIST_H__
#include "wind_type.h"
#include "boot_part.h"
#ifdef __cplusplus
extern "C" {
#endif



typedef void (*Menu_handle)(void);
typedef struct
{
    char key;//输入的键值
    char prio;//菜单权限
    char prio_bak;//菜单权限的备份
    const char *menu_item;//菜单项
    Menu_handle handle;//处理的句柄
}menu_handle_TB;

extern menu_handle_TB g_menu_handleTB[];
void run_menu(void);
void print32_t_menu_list(void);
w_int32_t get_menu_go_direction(void);
w_int32_t flush_img_to_rom(part_s *code_part);

#ifdef __cplusplus
}
#endif

#endif
