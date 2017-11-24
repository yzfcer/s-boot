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
#ifndef BOOT_CONFIG_H__
#define BOOT_CONFIG_H__
#ifdef __cplusplus
extern "C" {
#endif

//使能debug信息打印功能
#define BOOT_DEBUG_ENABLE 0

//测试功能使能
#define BOOT_TEST_ENABLE 1

//加密方式
#define ENCRYPT_NONE 0
#define ENCRYPT_AES 1
#define ENCRYPT_DES 2
#define ENCRYPT_RC4 3
#define ENCRYPT_TYPE ENCRYPT_RC4


//芯片锁定使能
#define MCU_LOCK_ENABLE 0

//等待键盘输入时间秒数
#define WAIT_KEY_SEC 5

//运行位置
#define RUN_IN_RAM 1
#define RUN_IN_ROM 2
#define RUN_SPACE_TYPE RUN_IN_RAM

//读写ROM时的基本块大小
#define BLOCK_SIZE 4096

#ifdef __cplusplus
}
#endif
#endif

