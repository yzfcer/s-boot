/*********************************************************************************
  *Copyright(C),2016-2018,yzfcer@163.com
  *FileName:  
  *Author:      Zhou Jiangcun
  *Version:     1.0
  *Date:        2016/11/04
  *Description:  
  *Others:  
  *History:  
     1.Date:
       Author:
       Modification:
**********************************************************************************/
#include "boot_type.h"
typedef int (*Encrypt)(uint8_t *data,int lenth);
typedef int (*Decrypt)(uint8_t *data,int lenth);
typedef struct
{
    Encrypt encrypt;
    Decrypt decrypt;
}crypt_port_s;


int RC4_crypt(uint8_t *data, int32_t datalen);
static int none_crypt(uint8_t *data,int datalen)
{
	return datalen;
}

crypt_port_s crypt_port[] = 
{
    {none_crypt,none_crypt},
    {RC4_crypt,RC4_crypt},
};

int encrypt_data(int crypttype,uint8_t *data,int lenth)
{
    crypt_port_s *port;
    int count = sizeof(crypt_port)/sizeof(crypt_port_s);
    if(crypttype >= count)
        return lenth;
    port = &crypt_port[crypttype];
    if(!port->encrypt)
        return lenth;
    return port->encrypt(data,lenth);
}

int decrypt_data(int crypttype,uint8_t *data,int lenth)
{
    crypt_port_s *port;
    int count = sizeof(crypt_port)/sizeof(crypt_port_s);
    if(crypttype >= count)
        return lenth;
    port = &crypt_port[crypttype];
    if(!port->decrypt)
        return lenth;
    return port->decrypt(data,lenth);
}


