#ifndef ENCRYPT_H__
#define ENCRYPT_H__
#include "boot_type.h"

#define ENCRYPT_KEY {0x12,0x25,0x38,0x54,0x85,0xa5,0x7c,0x96,0x34,0x57,0xf2,0x39,0x0a,0x83,0x98,0x65}

int AES_encrypt(uint8_t *data,int datalen);
int AES_decrypt(uint8_t *data,int datalen);

int RC4_crypt(uint8_t *data, int32_t datalen);

int DES_Encrypt(uint8_t *data,int datalen);
int DES_Decrypt(uint8_t *data,int datalen);

#endif 
