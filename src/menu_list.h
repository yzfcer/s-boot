

/***************************************************************************************
** �ļ�����
** �ļ���Ϣ��
** ����ʱ�䣺2015.10.13
** �����ˣ��ܽ���
**�ļ��汾1.0
** �޸ļ�¼��
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
int32_t flush_code_to_sflash(region_s *img,region_s *bin);

#ifdef __cplusplus
}
#endif

#endif
