/***************************************************************************************
** 文件名：
** 文件信息：
** 创建时间：2015.10.13
** 创建人：周江村
**文件版本1.0
** 修改记录：
***************************************************************************************/
#ifndef MAIN_H__
#define MAIN_H__
#include "boot_type.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef enum 
{
    BOOT_INIT = 0,
    BOOT_APP_DEBUG_CHECK,
    BOOT_FIRST_CHECK,
    BOOT_SELF_CHECK,
    BOOT_CHIP_LOCK_CHECK,
    BOOT_UPGRADE_CHECK,
    BOOT_ROLLBACK_CHECK,
    BOOT_CHANGE_APP_TYPE,
    BOOT_WAIT_KEY_PRESS,
    BOOT_MENU_LIST,
    BOOT_LOAD_APP,
    BOOT_SET_APP_PARAM,
    BOOT_JUMP_TO_APP,
    BOOT_ERROR,
    BOOT_STEP_CNT
}boot_status_e;

typedef int32_t (*Boot_handle)(void);
typedef struct 
{
    boot_status_e status;
    char *stepname;
    Boot_handle handle;
}boot_handle_TB;

void set_boot_status(boot_status_e status);
void go_to_next_step(void);

#ifdef __cplusplus
}
#endif
#endif

