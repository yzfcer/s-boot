#ifndef AES_H
#define AES_H
#include "boot_type.h"
extern void aes_init( uint8_t * tempbuf );
extern void aes_decrypt(uint8_t *buffer, uint8_t *chainBlock);
extern void aes_encrypt( uint8_t * buffer, uint8_t * chainBlock );
extern uint8_t aes_block_decrypt(uint8_t Direct,uint8_t *ChiperDataBuf,uint8_t DataLen);

#endif // AES_H