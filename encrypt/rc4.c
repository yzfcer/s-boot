#include "boot_type.h"
#include "encrypt.h"
static uint8_t rc4_key[16] = ENCRYPT_KEY;
static unsigned char rc4_sbox[256] = { 0 };

static void rc4_init(uint8_t*sbox, uint8_t *key, int32_t Len)
{
    int32_t i = 0, j = 0;
    uint8_t k[256] = { 0 };
    uint8_t tmp = 0;
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
 
int RC4_crypt(uint8_t *data, int32_t len)
{
    int32_t i = 0, j = 0, t = 0;
    int32_t k = 0;
    uint8_t tmp;
    
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
 
