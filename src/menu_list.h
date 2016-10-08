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
    char key;//����ļ�ֵ
    char prio;//�˵�Ȩ��
    char prio_bak;//�˵�Ȩ�޵ı���
    const char *menu_item;//�˵���
    Menu_handle handle;//����ľ��
}menu_handle_TB;

extern menu_handle_TB g_menu_handleTB[];
void menu_entry(void);
void print32_t_menu_list(void);
int32_t get_menu_go_direction(void);
int32_t flush_code_to_iflash(region_s *code_region);

#ifdef __cplusplus
}
#endif

#endif
