/***************************************************************************************
** �ļ�����
** �ļ���Ϣ��
** ����ʱ�䣺2016.09.25
** �����ˣ���纣��
**�ļ��汾 1.0
** �޸ļ�¼��
***************************************************************************************/
#ifndef F_FCSR_H_H
#define F_FCSR_H_H
#include "boot_type.h"
#ifdef __cplusplus
extern "C" {
#endif

//*************************
// FCSR״̬�ṹ��
//*************************
typedef struct _ECRYPT_ctx
{
    uint8_t state[20];
    uint8_t carry[20];
    uint8_t filter[20];
    uint8_t init[10];
} ECRYPT_ctx;

//************************
// ��Կ�����㷨
//************************
extern void ECRYPT_keysetup(  // ����Կ���˲��������Ӧ�Ľṹ��Ա��
  ECRYPT_ctx* ctx, 
  uint8_t* key); 


//************************
// ��ʼֵIV�����㷨
//************************
extern void ECRYPT_ivsetup(
  ECRYPT_ctx* ctx, 
  uint8_t* iv);

//************************
// ������һ����Կ��
//************************
extern void ECRYPT_process_keys(
    ECRYPT_ctx* ctx, 
    uint8_t* keys, 
    uint32_t msglen); 

//************************
// ����Կ����/��������
//************************
extern void ECRYPT_Encrypt(
    uint8_t *keys,
    uint8_t* en_data,
    uint32_t msglen) ; 

/* Update the shift register and the carry register of the FCSR */
//************************
// FCSR��תһ��
//************************
extern void ECRYPT_clock(ECRYPT_ctx* ctx);

/* Produce one byte of keystream from the int32_ternal state of the register */
//************************
// ��FCSR�˲����һ�ֽ�
//************************
extern uint8_t ECRYPT_filter(ECRYPT_ctx* ctx);

//extern uint8_t  cKEY[];
//extern uint8_t  cIV[];

int32_t decrypt_data(uint32_t addr,int32_t filelen,int32_t encrypt);

#ifdef __cplusplus
}
#endif

#endif

