/***************************************************************************************
** �ļ�����
** �ļ���Ϣ��
** ����ʱ�䣺2016.09.25
** �����ˣ���纣��
**�ļ��汾 1.0
** �޸ļ�¼��
***************************************************************************************/
#include "boot_type.h"
#include "encrypt.h"
#include "port.h"
#ifdef __cplusplus
extern "C" {
#endif

uint8_t  cFilter[] =
{
    0x4E,0x25,0xD4,0x3D,0x90,0xD5,0x46,0xAF,0x8A,0xDC,
    0x23,0x86,0xC5,0x9F,0x61,0x26,0xFF,0x5D,0x98,0xAE
};

uint8_t  cKEY[] = 
{
    0x00,0x88,0x63,0x9d,0x6b,0xf8,0x47,0xed,0x59,0xc6    
};

uint8_t  cIV[] = 
{
    0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99
};


//************************
// ��Կ�����㷨
//************************
void ECRYPT_keysetup(  // ����Կ���˲��������Ӧ�Ľṹ��Ա��
  ECRYPT_ctx* ctx, 
  uint8_t* key) 
{
    uint8_t i;
    for(i=0;i<20;i++)
        ctx->filter[i] = cFilter[i];
        
    for(i=0; i<10; i++)
    {
        ctx->state[i] = key[9-i];
        ctx->init[i]=ctx->state[i]; 
    }
    for(;i<20;i++)
        ctx->state[i] = 0x00;
}
//************************
// ��ʼֵIV�����㷨
//************************
void ECRYPT_ivsetup(ECRYPT_ctx* ctx,uint8_t* iv)
{
    uint8_t i=0;//,tmp;
    uint8_t S[20];
    
    for(i=0; i<10; i++)
    {
        ctx->state[i] = ctx->init[i];
    }
    for(; i<20; i++)
    {
        ctx->state[i] = iv[19-i];
    }
    for(i=0; i<20; i++)
    {
        ctx->carry[i] = 0x00;
    }
    for(i=0; i<20; i++)
    {
        ECRYPT_clock(ctx);
        S[i]=ECRYPT_filter(ctx);
    }
    for(i=0; i<20; i++)
    {
        ctx->state[i] = S[i];
        ctx->carry[i] = 0x00;
    }
    for( i=0; i<162 ; i++)
    {
        ECRYPT_clock(ctx);
    }
        
}
//************************
// ������һ����Կ��
//************************
void ECRYPT_process_keys(ECRYPT_ctx* ctx,uint8_t* keys,uint32_t msglen)/* Message length in bytes. */ 
{
    uint32_t i;//,ch1,ch2;
    for(i=0; i<msglen; i++)
    {
        ECRYPT_clock(ctx);
        keys[i] = ECRYPT_filter(ctx);
    }
}

//************************
// ����Կ����/��������
//************************
void ECRYPT_Encrypt(uint8_t *keys,uint8_t* en_data,uint32_t msglen)/* Message length in bytes. */ 
{
    uint32_t i;
    for(i=0; i<msglen; i++)
    {
        en_data[i] = en_data[i] ^ keys[i];
    }
}


/* Update the shift register and the carry register of the FCSR */
//************************
// FCSR��תһ��
//************************
void ECRYPT_clock(ECRYPT_ctx* ctx)
{
    uint8_t feedback,i;
    uint8_t buffer[20];
    
    /* expand the feedback bit */
    if(ctx->state[0] & 0x01)
        feedback = 0xFF;
    else
        feedback = 0x00;
    /* shift the state */
    for(i=0; i<19; i++)
    {
        ctx->state[i] = ctx->state[i] >> 1;    
        ctx->state[i] |= (ctx->state[i+1] & 0x01) << 7;
    }
    ctx->state[i] >>= 1;
    /* update the register */
    
    for(i=0; i<20; i++)
    {
        buffer[i] = ctx->state[i] ^ ctx->carry[i];
    }
    for(i=0; i<20; i++)
    {
        ctx->carry[i] &= ctx->state[i];
    }
    for(i=0; i<20; i++)
    {
        ctx->carry[i] ^= buffer[i] & (feedback & ctx->filter[i]);
    }
    for(i=0; i<20; i++)
        buffer[i] ^= (feedback & ctx->filter[i]);
    for(i=0; i<20; i++)
    {
        ctx->state[i] = buffer[i]; 
    }        
}

/* Produce one byte of keystream from the int32_ternal state of the register */
//************************
// ��FCSR�˲����һ�ֽ�
//************************
uint8_t ECRYPT_filter(ECRYPT_ctx* ctx)
{
    uint8_t i;
    uint8_t buffer[20];
    for(i=0; i<20; i++)
    {
        buffer[i] = ctx->filter[i] & ctx->state[i];
    }
    for(i=1; i<20; i++)
    {
        buffer[0] ^= buffer[i];
    }
    return buffer[0];
}


#define ENCRYP_KEYLEN    32        //��Կ������
#define MIN(x,y) ((x) < (y)?(x):(y))
uint8_t chIV[10];                 //���ܽ���ʹ�õ�IVֵ
ECRYPT_ctx  ctx;                    //���ڽ��ܵĽṹ��
uint8_t chKeys[ENCRYP_KEYLEN];        //��Կ��

int32_t decrypt_data(uint32_t addr,int32_t filelen,int32_t encrypt)
{
    int32_t i,j;
    uint8_t tempdat, len; 
    uint8_t *pUpdateBuf = (uint8_t*)addr;
    char *identify = "MHIMG_M4";
    for(i = 0; i < 10; i++) 
    {
        chIV[i] = *(pUpdateBuf + i);
    }

    ECRYPT_keysetup(&ctx, cKEY);
    ECRYPT_ivsetup(&ctx, chIV);

    i = 10;
    do 
    {
        //������Կ��
        ECRYPT_process_keys(&ctx, chKeys, ENCRYP_KEYLEN);
        //���ݻ�õ�VIֵ������ܽ��
        len = MIN(ENCRYP_KEYLEN, filelen - i);
        for(j = 0; j < len; j++) 
        {
            tempdat  = *(pUpdateBuf + i + j);
            *(pUpdateBuf + i + j) = tempdat ^ chKeys[j];
        }
        i += len;
        feed_watchdog();
    } while( i < filelen);

    if(!encrypt)
    {
        for(i = 0;i < 8;i ++)
        {
            if(pUpdateBuf[10 + i] != identify[i])
                return -1;
        }
        return 0;
    }
    return 0;
}


#ifdef __cplusplus
}
#endif
