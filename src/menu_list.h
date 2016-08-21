

/***************************************************************************************
** 文件名：
** 文件信息：
** 创建时间：2015.10.13
** 创建人：周江村
**文件版本1.0
** 修改记录：
***************************************************************************************/
#ifndef MENU_LIST_H__
#define MENU_LIST_H__
#include "boot_type.h"
#include "mem_map.h"
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
void menu_entry(void);
void print32_t_menu_list(void);
int32_t get_menu_go_direction(void);
int32_t flush_code_to_iflash(region_s *code_region);
int32_t flush_code_to_sflash(region_s *img,region_s *bin);

#ifdef __cplusplus
}
#endif

#endif
