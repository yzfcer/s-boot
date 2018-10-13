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
#ifndef MAIN_H__
#define MAIN_H__
#include "wind_type.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef enum 
{
    BOOT_INIT = 0,
    BOOT_FIRST_CHECK,
    BOOT_DEBUG_MODE_CHECK,
    BOOT_IMG_VALID_CHECK,
    BOOT_CHIP_LOCK_CHECK,
    BOOT_UPGRADE_CHECK,
    BOOT_WAIT_KEY_PRESS,
    BOOT_MENU_LIST,
    BOOT_LOAD_IMG,
    BOOT_SET_SHARE_PARAM,
    BOOT_RUN_SYSTEM,
    BOOT_ERROR,
    BOOT_STEP_CNT
}boot_status_e;

typedef w_int32_t (*Boot_handle)(void);
typedef struct 
{
    boot_status_e status;
    char *stepname;
    Boot_handle handle;
}boot_handle_TB;

void boot_status_set(boot_status_e status);
void boot_status_go_next(void);

#ifdef __cplusplus
}
#endif
#endif

