/***************************************************************************************
** 文件名：
** 文件信息：
** 创建时间：2016.09.25
** 创建人：清风海岸
**文件版本 1.0
** 修改记录：
***************************************************************************************/
#ifndef F_FCSR_H_H
#define F_FCSR_H_H
#include "boot_type.h"
#ifdef __cplusplus
extern "C" {
#endif

//*************************
// FCSR状态结构体
//*************************
typedef struct _ECRYPT_ctx
{
    uint8_t state[20];
    uint8_t carry[20];
    uint8_t filter[20];
    uint8_t init[10];
} ECRYPT_ctx;

//************************
// 密钥加载算法
//************************
extern void ECRYPT_keysetup(  // 将密钥和滤波器存入对应的结构成员中
  ECRYPT_ctx* ctx, 
  uint8_t* key); 


//************************
// 初始值IV加载算法
//************************
extern void ECRYPT_ivsetup(
  ECRYPT_ctx* ctx, 
  uint8_t* iv);

//************************
// 产生出一段密钥流
//************************
extern void ECRYPT_process_keys(
    ECRYPT_ctx* ctx, 
    uint8_t* keys, 
    uint32_t msglen); 

//************************
// 用密钥流加/解密数据
//************************
extern void ECRYPT_Encrypt(
    uint8_t *keys,
    uint8_t* en_data,
    uint32_t msglen) ; 

/* Update the shift register and the carry register of the FCSR */
//************************
// FCSR跳转一次
//************************
extern void ECRYPT_clock(ECRYPT_ctx* ctx);

/* Produce one byte of keystream from the int32_ternal state of the register */
//************************
// 对FCSR滤波输出一字节
//************************
extern uint8_t ECRYPT_filter(ECRYPT_ctx* ctx);

//extern uint8_t  cKEY[];
//extern uint8_t  cIV[];

int32_t decrypt_data(uint32_t addr,int32_t filelen,int32_t encrypt);

#ifdef __cplusplus
}
#endif

#endif

