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
#include "encrypt.h"
#include "boot_type.h"

#define BPOLY 0x1b 
#define BLOCKSIZE 16 

#define KEYBITS 128 
#define ROUNDS 10 
#define KEYLENGTH 16 


uint8_t  block1[256]; 
uint8_t  block2[256]; 

uint8_t  * powTbl; 
uint8_t  * logTbl; 
uint8_t  * sBox; 
uint8_t  * sBoxInv; 
uint8_t  * expandedKey; 

static uint8_t  aes_key[16] = ENCRYPT_KEY;


void cleardog(void)
{
}
static void mem_set(char *dest,char value,int len)
{
    int i;
    for(i = 0;i < len;i ++)
        dest[i] = value;
}

void CalcPowLog(uint8_t * powTbl, uint8_t * logTbl)
{
    uint8_t  i = 0;
    uint8_t  t = 1;

    do 
    {
        powTbl[i] = t;
        logTbl[t] = i;
        i++;
        t ^= (t << 1) ^ (t & 0x80 ? BPOLY : 0);
    } while(t != 1);
    powTbl[255] = powTbl[0]; 
}



void CalcSBox(uint8_t * sBox)
{
    uint8_t  i, rot;
    uint8_t  temp;
    uint8_t  result;

    i = 0;
    do 
    {
        if(i > 0) 
            temp = powTbl[255 - logTbl[i]];
        else 
            temp = 0;
        result = temp ^ 0x63; // Start with adding a vector in GF(2).
        for(rot = 0; rot < 4; rot++) 
        {
            temp = (temp<<1) | (temp>>7);
            result ^= temp;
        }
        sBox[i] = result;
    } while(++i != 0);
} 



void CalcSBoxInv(uint8_t * sBox, uint8_t * sBoxInv)
{
    uint8_t  i = 0;
    uint8_t  j = 0;

    do 
    {
        cleardog();
        do 
        {
            if(sBox[ j ] == i) 
            {
                sBoxInv[ i ] = j;
                j = 255;
            }
        } while(++j != 0);
    } while(++i != 0);
}



void CycleLeft(uint8_t * row)
{
    uint8_t  temp = row[0];
    row[0] = row[1];
    row[1] = row[2];
    row[2] = row[3];
    row[3] = temp;
}



void InvMixColumn(uint8_t * column)
{
    uint8_t  r0, r1, r2, r3;

    r0 = column[1] ^ column[2] ^ column[3];
    r1 = column[0] ^ column[2] ^ column[3];
    r2 = column[0] ^ column[1] ^ column[3];
    r3 = column[0] ^ column[1] ^ column[2];

    column[0] = (column[0] << 1) ^ (column[0] & 0x80 ? BPOLY : 0);
    column[1] = (column[1] << 1) ^ (column[1] & 0x80 ? BPOLY : 0);
    column[2] = (column[2] << 1) ^ (column[2] & 0x80 ? BPOLY : 0);
    column[3] = (column[3] << 1) ^ (column[3] & 0x80 ? BPOLY : 0);

    r0 ^= column[0] ^ column[1];
    r1 ^= column[1] ^ column[2];
    r2 ^= column[2] ^ column[3];
    r3 ^= column[0] ^ column[3];

    column[0] = (column[0] << 1) ^ (column[0] & 0x80 ? BPOLY : 0);
    column[1] = (column[1] << 1) ^ (column[1] & 0x80 ? BPOLY : 0);
    column[2] = (column[2] << 1) ^ (column[2] & 0x80 ? BPOLY : 0);
    column[3] = (column[3] << 1) ^ (column[3] & 0x80 ? BPOLY : 0);

    r0 ^= column[0] ^ column[2];
    r1 ^= column[1] ^ column[3];
    r2 ^= column[0] ^ column[2];
    r3 ^= column[1] ^ column[3];

    column[0] = (column[0] << 1) ^ (column[0] & 0x80 ? BPOLY : 0);
    column[1] = (column[1] << 1) ^ (column[1] & 0x80 ? BPOLY : 0);
    column[2] = (column[2] << 1) ^ (column[2] & 0x80 ? BPOLY : 0);
    column[3] = (column[3] << 1) ^ (column[3] & 0x80 ? BPOLY : 0);

    column[0] ^= column[1] ^ column[2] ^ column[3];
    r0 ^= column[0];
    r1 ^= column[0];
    r2 ^= column[0];
    r3 ^= column[0];

    column[0] = r0;
    column[1] = r1;
    column[2] = r2;
    column[3] = r3;
}

uint8_t Multiply(uint8_t num, uint8_t factor)
{
    uint8_t mask = 1;
    uint8_t result = 0;

    while(mask != 0) 
    {
        if(mask & factor) 
        {
            result ^= num;
        }
        mask <<= 1;
        num = (num << 1) ^ (num & 0x80 ? BPOLY : 0);
    }

    return result;
}


uint8_t DotProduct(uint8_t * vector1, uint8_t * vector2)
{
    uint8_t result = 0;

    result ^= Multiply(*vector1++, *vector2++);
    result ^= Multiply(*vector1++, *vector2++);
    result ^= Multiply(*vector1++, *vector2++);
    result ^= Multiply(*vector1  , *vector2  );

    return result;
}

void MixColumn(uint8_t * column)
{
    uint8_t  row[8] = {0x02, 0x03, 0x01, 0x01, 0x02, 0x03, 0x01, 0x01};
    uint8_t  result[4];
     
    result[0] = DotProduct(row+0, column);
    result[1] = DotProduct(row+3, column);
    result[2] = DotProduct(row+2, column);
    result[3] = DotProduct(row+1, column);

    column[0] = result[0];
    column[1] = result[1];
    column[2] = result[2];
    column[3] = result[3];
}


void SubBytes(uint8_t * uint8_ts, uint8_t count)
{
    do 
    {
        *uint8_ts = sBox[ *uint8_ts ]; 
        uint8_ts++;
    } while(--count);
}



void InvSubBytesAndXOR(uint8_t * uint8_ts, uint8_t * key, uint8_t count)
{
    do 
    {
        *uint8_ts = block2[ *uint8_ts ] ^ *key; 
        uint8_ts++;
        key++;
    } while(--count);
}



void InvShiftRows(uint8_t * state)
{
    uint8_t temp;

    temp = state[ 1 + 3*4 ];
    state[ 1 + 3*4 ] = state[ 1 + 2*4 ];
    state[ 1 + 2*4 ] = state[ 1 + 1*4 ];
    state[ 1 + 1*4 ] = state[ 1 + 0*4 ];
    state[ 1 + 0*4 ] = temp;

    temp = state[ 2 + 0*4 ];
    state[ 2 + 0*4 ] = state[ 2 + 2*4 ];
    state[ 2 + 2*4 ] = temp;
    temp = state[ 2 + 1*4 ];
    state[ 2 + 1*4 ] = state[ 2 + 3*4 ];
    state[ 2 + 3*4 ] = temp;

    temp = state[ 3 + 0*4 ];
    state[ 3 + 0*4 ] = state[ 3 + 1*4 ];
    state[ 3 + 1*4 ] = state[ 3 + 2*4 ];
    state[ 3 + 2*4 ] = state[ 3 + 3*4 ];
    state[ 3 + 3*4 ] = temp;
}

void ShiftRows(uint8_t * state)
{
    uint8_t temp;
    temp = state[ 1 + 0*4 ];
    state[ 1 + 0*4 ] = state[ 1 + 1*4 ];
    state[ 1 + 1*4 ] = state[ 1 + 2*4 ];
    state[ 1 + 2*4 ] = state[ 1 + 3*4 ];
    state[ 1 + 3*4 ] = temp;

    temp = state[ 2 + 0*4 ];
    state[ 2 + 0*4 ] = state[ 2 + 2*4 ];
    state[ 2 + 2*4 ] = temp;
    temp = state[ 2 + 1*4 ];
    state[ 2 + 1*4 ] = state[ 2 + 3*4 ];
    state[ 2 + 3*4 ] = temp;

    temp = state[ 3 + 3*4 ];
    state[ 3 + 3*4 ] = state[ 3 + 2*4 ];
    state[ 3 + 2*4 ] = state[ 3 + 1*4 ];
    state[ 3 + 1*4 ] = state[ 3 + 0*4 ];
    state[ 3 + 0*4 ] = temp;
}


void InvMixColumns(uint8_t * state)
{
    InvMixColumn(state + 0*4);
    InvMixColumn(state + 1*4);
    InvMixColumn(state + 2*4);
    InvMixColumn(state + 3*4);
}

void MixColumns(uint8_t * state)
{
    MixColumn(state + 0*4);
    MixColumn(state + 1*4);
    MixColumn(state + 2*4);
    MixColumn(state + 3*4);
}


void XORBytes(uint8_t * uint8_ts1, uint8_t * uint8_ts2, uint8_t count)
{
    do 
    {
        *uint8_ts1 ^= *uint8_ts2; // Add in GF(2), ie. XOR.
        uint8_ts1++;
        uint8_ts2++;
    } while(--count);
}



void CopyBytes(uint8_t * to, uint8_t * from, uint8_t count)
{
    do 
    {
        *to = *from;
        to++;
        from++;
    } while(--count);
}



void KeyExpansion(uint8_t * expandedKey)
{
    uint8_t  temp[4];
    uint8_t i;
    uint8_t  Rcon[4] = { 0x01, 0x00, 0x00, 0x00 }; 
     
    uint8_t  *key;
    key = aes_key;
    i = KEYLENGTH;
    do 
    {
        *expandedKey = *key;
        expandedKey++;
        key++;
    } while(--i);

    expandedKey -= 4;
    temp[0] = *(expandedKey++);
    temp[1] = *(expandedKey++);
    temp[2] = *(expandedKey++);
    temp[3] = *(expandedKey++);

    i = KEYLENGTH;
    while(i < BLOCKSIZE*(ROUNDS+1)) 
    {
        if((i % KEYLENGTH) == 0) 
        {
            CycleLeft(temp); 
            SubBytes(temp, 4); 
            XORBytes(temp, Rcon, 4); 
            *Rcon = (*Rcon << 1) ^ (*Rcon & 0x80 ? BPOLY : 0);
        }

#if KEYLENGTH > 24
        else if((i % KEYLENGTH) == BLOCKSIZE) 
        {
            SubBytes(temp, 4); // Substitute each uint8_t.
        }
#endif

        XORBytes(temp, expandedKey - KEYLENGTH, 4);

        *(expandedKey++) = temp[ 0 ];
        *(expandedKey++) = temp[ 1 ];
        *(expandedKey++) = temp[ 2 ];
        *(expandedKey++) = temp[ 3 ];
        i += 4; 
    } 
}



void InvCipher(uint8_t * block, uint8_t * expandedKey)
{
    uint8_t round = ROUNDS-1;
    expandedKey += BLOCKSIZE * ROUNDS;

    XORBytes(block, expandedKey, 16);
    expandedKey -= BLOCKSIZE;
    do 
    {
        InvShiftRows(block);
        InvSubBytesAndXOR(block, expandedKey, 16);
        expandedKey -= BLOCKSIZE;
        InvMixColumns(block);
    } while(--round);

    InvShiftRows(block);
    InvSubBytesAndXOR(block, expandedKey, 16);
}

//完成一个块(16字节，128bit)的加密
void Cipher(uint8_t * block, uint8_t * expandedKey) 
{
    uint8_t round = ROUNDS-1;

    XORBytes(block, expandedKey, 16);
    expandedKey += BLOCKSIZE;

    do 
    {
        SubBytes(block, 16);
        ShiftRows(block);
        MixColumns(block);
        XORBytes(block, expandedKey, 16);
        expandedKey += BLOCKSIZE;
    } while(--round);

    SubBytes(block, 16);
    ShiftRows(block);
    XORBytes(block, expandedKey, 16);
}

void aesInit(uint8_t * tempbuf)
{
    powTbl = block1;
    logTbl = block2;
    CalcPowLog(powTbl, logTbl);

    sBox = tempbuf;
    CalcSBox(sBox);

    expandedKey = block1;  //至此block1用来存贮密码表
    KeyExpansion(expandedKey);
     
    sBoxInv = block2; //block2至此开始只用来存贮SBOXINV
    CalcSBoxInv(sBox, sBoxInv);
} 


void aesDecryptBlock(uint8_t * buffer, uint8_t * chainBlock)
{
    CopyBytes(buffer,chainBlock,BLOCKSIZE);
    InvCipher(buffer, expandedKey);
    CopyBytes(chainBlock, buffer, BLOCKSIZE);
}

void aesEncryptBlock(uint8_t * buffer, uint8_t * chainBlock)
{
    CopyBytes(buffer, chainBlock, BLOCKSIZE);
    Cipher(buffer, expandedKey);
    CopyBytes(chainBlock, buffer, BLOCKSIZE);
}

int AES_encrypt(uint8_t *data,int datalen)
{
    int  i,idx;
    int blocks;
    int encrypt_len;
    uint8_t  tempbuf[16];
    uint8_t  sBoxbuf[256];

    cleardog();
    aesInit(sBoxbuf);

    encrypt_len = (((datalen+15) >> 4) << 4);
    for(i = datalen;i < encrypt_len;i ++)
        data[i] = 0;
    blocks = ((datalen+15) >> 4); 
    for(i = 0;i < blocks;i ++)
    {
        idx = (i << 4);
        cleardog();
        aesEncryptBlock(tempbuf,&data[idx]);
    }
    
    cleardog();
    return encrypt_len;
}


int AES_decrypt(uint8_t *data,int datalen)
{
    int  i,idx;
    int blocks;
    uint8_t  tempbuf[16];
    uint8_t  sBoxbuf[256];
    if(datalen&0x0f)
        return -1;
    
    cleardog();
    aesInit(sBoxbuf);

    blocks = (datalen>>4);
    for(i=0;i<blocks;i++)
    {
        cleardog();
        idx = (i << 4);
        aesDecryptBlock(tempbuf,&data[idx]);
    }
    cleardog();
    return datalen;

}

