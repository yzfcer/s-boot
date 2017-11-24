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
#include "wind_type.h"
typedef w_int32_t (*Encrypt)(w_uint8_t *data,w_int32_t lenth);
typedef w_int32_t (*Decrypt)(w_uint8_t *data,w_int32_t lenth);
typedef struct
{
    Encrypt encrypt;
    Decrypt decrypt;
}crypt_port_s;


w_int32_t RC4_crypt(w_uint8_t *data, w_int32_t datalen);
static w_int32_t none_crypt(w_uint8_t *data,w_int32_t datalen)
{
	return datalen;
}

crypt_port_s crypt_port[] = 
{
    {none_crypt,none_crypt},
    {RC4_crypt,RC4_crypt},
};

w_int32_t encrypt_data(w_int32_t crypttype,w_uint8_t *data,w_int32_t lenth)
{
    crypt_port_s *port;
    w_int32_t count = sizeof(crypt_port)/sizeof(crypt_port_s);
    if(crypttype >= count)
        return lenth;
    port = &crypt_port[crypttype];
    if(!port->encrypt)
        return lenth;
    return port->encrypt(data,lenth);
}

w_int32_t decrypt_data(w_int32_t crypttype,w_uint8_t *data,w_int32_t lenth)
{
    crypt_port_s *port;
    w_int32_t count = sizeof(crypt_port)/sizeof(crypt_port_s);
    if(crypttype >= count)
        return lenth;
    port = &crypt_port[crypttype];
    if(!port->decrypt)
        return lenth;
    return port->decrypt(data,lenth);
}


