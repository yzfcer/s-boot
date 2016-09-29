#ifndef AES_H
#define AES_H
#include "boot_type.h"
extern void aes_init( uint8_t * tempbuf );
extern void aesDecrypt(uint8_t *buffer, uint8_t *chainBlock);
extern void aesEncrypt( uint8_t * buffer, uint8_t * chainBlock );
extern uint8_t aesBlockDecrypt(uint8_t Direct,uint8_t *ChiperDataBuf,uint8_t DataLen);

#endif // AES_H