/*********************************************************************************
  *Copyright(C),2016-2018,yzfcer@163.com
  *FileName:  
  *Author:      Jason Zhou
  *Version:     1.0
  *Date:        2016/11/04
  *Description:  
  *Others:  
  *History:  
     1.Date:
       Author:
       Modification:
**********************************************************************************/
#include "wind_type.h"
#include "encrypt.h"
static w_uint8_t rc4_key[16] = ENCRYPT_KEY;
static unsigned char rc4_sbox[256] = { 0 };

static void rc4_init(w_uint8_t*sbox, w_uint8_t *key, w_int32_t Len)
{
    w_int32_t i = 0, j = 0;
    w_uint8_t k[256] = { 0 };
    w_uint8_t tmp = 0;
    for (i = 0; i<256; i++)
    {
        sbox[i] = i;
        k[i] = key[i%Len];
    }
    for (i = 0; i<256; i++)
    {
        j = (j + sbox[i] + k[i]) % 256;
        tmp = sbox[i];
        sbox[i] = sbox[j];
        sbox[j] = tmp;
    }
}
 
w_int32_t RC4_crypt(w_uint8_t *data, w_int32_t len)
{
    w_int32_t i = 0, j = 0, t = 0;
    w_int32_t k = 0;
    w_uint8_t tmp;
    
    rc4_init(rc4_sbox, (unsigned char*)rc4_key, 16);
    for (k = 0; k<len; k++)
    {
        i = (i + 1) % 256;
        j = (j + rc4_sbox[i]) % 256;
        tmp = rc4_sbox[i];
        rc4_sbox[i] = rc4_sbox[j];
        rc4_sbox[j] = tmp;
        t = (rc4_sbox[i] + rc4_sbox[j]) % 256;
        data[k] ^= rc4_sbox[t];
    }
    return len;
}
 
