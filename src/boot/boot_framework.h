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
    BOOT_SET_APP_PARAM,
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

void set_boot_status(boot_status_e status);
void go_to_next_step(void);

#ifdef __cplusplus
}
#endif
#endif

