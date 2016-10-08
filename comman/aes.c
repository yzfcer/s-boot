#include <string.h>
#include "aes.h"

void cleardog(void)
{
}




#define BPOLY 0x1b //!< Lower 8 bits of (x^8+x^4+x^3+x+1), ie. (x^4+x^3+x+1).
#define BLOCKSIZE 16 //!< Block size in number of uint8_ts.



#define KEYBITS 128 //!< Use AES128.
#define ROUNDS 10 //!< Number of rounds.
#define KEYLENGTH 16 //!< Key length in number of uint8_ts.


uint8_t  block1[256]; //!< Workspace 1.
uint8_t  block2[256]; //!< Worksapce 2.

uint8_t  * powTbl; //!< Final location of exponentiation lookup table.
uint8_t  * logTbl; //!< Final location of logarithm lookup table.
uint8_t  * sBox; //!< Final location of s-box.
uint8_t  * sBoxInv; //!< Final location of inverse s-box.
uint8_t  * expandedKey; //!< Final location of expanded key.




void CalcPowLog(uint8_t * powTbl, uint8_t * logTbl)
{
    uint8_t  i = 0;
    uint8_t  t = 1;

    do 
    {
        // Use 0x03 as root for exponentiation and logarithms.
        powTbl[i] = t;
        logTbl[t] = i;
        i++;
        // Muliply t by 3 in GF(2^8).
        t ^= (t << 1) ^ (t & 0x80 ? BPOLY : 0);
    } while(t != 1); // Cyclic properties ensure that i < 255.
    powTbl[255] = powTbl[0]; // 255 = '-0', 254 = -1, etc.
}



void CalcSBox(uint8_t * sBox)
{
    uint8_t  i, rot;
    uint8_t  temp;
    uint8_t  result;

    // Fill all entries of sBox[].
    i = 0;
    do 
    {
        // Inverse in GF(2^8).
        if(i > 0) 
            temp = powTbl[ 255 - logTbl[i] ];
        else 
            temp = 0;

        // Affine transformation in GF(2).
        result = temp ^ 0x63; // Start with adding a vector in GF(2).
        for(rot = 0; rot < 4; rot++) 
        {
            // Rotate left.
            temp = (temp<<1) | (temp>>7);
            // Add rotated uint8_t in GF(2).
            result ^= temp;
        }
        // Put result in table.
        sBox[i] = result;
    } while(++i != 0);
} 



void CalcSBoxInv(uint8_t * sBox, uint8_t * sBoxInv)
{
    uint8_t  i = 0;
    uint8_t  j = 0;

    // Iterate through all elements in sBoxInv using  i.
    do 
    {
        // Search through sBox using j.
        cleardog();
        do 
        {
            // Check if current j is the inverse of current i.
            if(sBox[ j ] == i) 
            {
                // If so, set sBoxInc and indicate search finished.
                sBoxInv[ i ] = j;
                j = 255;
            }
        } while(++j != 0);
    } while(++i != 0);
}



void CycleLeft(uint8_t * row)
{
    // Cycle 4 uint8_ts in an array left once.
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
        // Check bit of factor given by mask.
        if(mask & factor) 
        {
            // Add current multiple of num in GF(2).
            result ^= num;
        }
        // Shift mask to indicate next bit.
        mask <<= 1;

        // Double num.
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
    // Prepare first row of matrix twice, to eliminate need for cycling.
    uint8_t  row[8] = 
    {0x02, 0x03, 0x01, 0x01,
    0x02, 0x03, 0x01, 0x01};

    uint8_t  result[4];
     
    // Take dot products of each matrix row and the column vector.
    result[0] = DotProduct(row+0, column);
    result[1] = DotProduct(row+3, column);
    result[2] = DotProduct(row+2, column);
    result[3] = DotProduct(row+1, column);

    // Copy temporary result to original column.
    column[0] = result[0];
    column[1] = result[1];
    column[2] = result[2];
    column[3] = result[3];
}


void SubBytes(uint8_t * uint8_ts, uint8_t count)
{
    do 
    {
        // Substitute every uint8_t in state.
        *uint8_ts = sBox[ *uint8_ts ]; 
        uint8_ts++;
    } while(--count);
}



void InvSubBytesAndXOR(uint8_t * uint8_ts, uint8_t * key, uint8_t count)
{
    do 
    {
        //  *uint8_ts = sBoxInv[ *uint8_ts ] ^ *key; // Inverse substitute every uint8_t in state and add key.
        *uint8_ts = block2[ *uint8_ts ] ^ *key; // Use block2 directly. Increases speed.
        uint8_ts++;
        key++;
    } while(--count);
}



void InvShiftRows(uint8_t * state)
{
    uint8_t temp;

    // Note: State is arranged column by column.

    // Cycle second row right one time.
    temp = state[ 1 + 3*4 ];
    state[ 1 + 3*4 ] = state[ 1 + 2*4 ];
    state[ 1 + 2*4 ] = state[ 1 + 1*4 ];
    state[ 1 + 1*4 ] = state[ 1 + 0*4 ];
    state[ 1 + 0*4 ] = temp;

    // Cycle third row right two times.
    temp = state[ 2 + 0*4 ];
    state[ 2 + 0*4 ] = state[ 2 + 2*4 ];
    state[ 2 + 2*4 ] = temp;
    temp = state[ 2 + 1*4 ];
    state[ 2 + 1*4 ] = state[ 2 + 3*4 ];
    state[ 2 + 3*4 ] = temp;

    // Cycle fourth row right three times, ie. left once.
    temp = state[ 3 + 0*4 ];
    state[ 3 + 0*4 ] = state[ 3 + 1*4 ];
    state[ 3 + 1*4 ] = state[ 3 + 2*4 ];
    state[ 3 + 2*4 ] = state[ 3 + 3*4 ];
    state[ 3 + 3*4 ] = temp;
}

void ShiftRows(uint8_t * state)
{
    uint8_t temp;

    // Note: State is arranged column by column.

    // Cycle second row left one time.
    temp = state[ 1 + 0*4 ];
    state[ 1 + 0*4 ] = state[ 1 + 1*4 ];
    state[ 1 + 1*4 ] = state[ 1 + 2*4 ];
    state[ 1 + 2*4 ] = state[ 1 + 3*4 ];
    state[ 1 + 3*4 ] = temp;

    // Cycle third row left two times.
    temp = state[ 2 + 0*4 ];
    state[ 2 + 0*4 ] = state[ 2 + 2*4 ];
    state[ 2 + 2*4 ] = temp;
    temp = state[ 2 + 1*4 ];
    state[ 2 + 1*4 ] = state[ 2 + 3*4 ];
    state[ 2 + 3*4 ] = temp;

    // Cycle fourth row left three times, ie. right once.
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
    uint8_t  Rcon[4] = { 0x01, 0x00, 0x00, 0x00 }; // Round constant.
     
    uint8_t  *key;
    uint8_t  a[16];
    key=a;
    key[0]=0x30;
    key[1]=0x30;
    key[2]=0x30;
    key[3]=0x30;
    key[4]=0x30;
    key[5]=0x30;
    key[6]=0x30;
    key[7]=0x30;
    key[8]=0x30;
    key[9]=0x30;
    key[10]=0x30;
    key[11]=0x30;
    key[12]=0x30;
    key[13]=0x30;
    key[14]=0x30;
    key[15]=0x30;
    ////////////////////////////////////////////

    // Copy key to start of expanded key.
    i = KEYLENGTH;
    do 
    {
        *expandedKey = *key;
        expandedKey++;
        key++;
    } while(--i);

    // Prepare last 4 uint8_ts of key in temp.
    expandedKey -= 4;
    temp[0] = *(expandedKey++);
    temp[1] = *(expandedKey++);
    temp[2] = *(expandedKey++);
    temp[3] = *(expandedKey++);

    // Expand key.
    i = KEYLENGTH;
    while(i < BLOCKSIZE*(ROUNDS+1)) 
    {
        // Are we at the start of a multiple of the key size?
        if((i % KEYLENGTH) == 0) 
        {
            CycleLeft(temp); // Cycle left once.
            SubBytes(temp, 4); // Substitute each uint8_t.
            XORBytes(temp, Rcon, 4); // Add constant in GF(2).
            *Rcon = (*Rcon << 1) ^ (*Rcon & 0x80 ? BPOLY : 0);
        }

        // Keysize larger than 24 uint8_ts, ie. larger that 192 bits?
#if KEYLENGTH > 24
        // Are we right past a block size?
        else if((i % KEYLENGTH) == BLOCKSIZE) 
        {
            SubBytes(temp, 4); // Substitute each uint8_t.
        }
#endif

        // Add uint8_ts in GF(2) one KEYLENGTH away.
        XORBytes(temp, expandedKey - KEYLENGTH, 4);

        // Copy result to current 4 uint8_ts.
        *(expandedKey++) = temp[ 0 ];
        *(expandedKey++) = temp[ 1 ];
        *(expandedKey++) = temp[ 2 ];
        *(expandedKey++) = temp[ 3 ];
        i += 4; // Next 4 uint8_ts.
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

void Cipher(uint8_t * block, uint8_t * expandedKey)    //完成一个块(16字节，128bit)的加密
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

void aes_init(uint8_t * tempbuf)
{
    powTbl = block1;
    logTbl = block2;
    CalcPowLog(powTbl, logTbl);

    sBox = tempbuf;
    CalcSBox(sBox);

    expandedKey = block1;  //至此block1用来存贮密码表
    KeyExpansion(expandedKey);
     
    sBoxInv = block2; // Must be block2. block2至此开始只用来存贮SBOXINV
    CalcSBoxInv(sBox, sBoxInv);
} 


//对一个16字节块解密,参数buffer是解密密缓存，chainBlock是要解密的块
void aesDecrypt(uint8_t * buffer, uint8_t * chainBlock)
{
    CopyBytes(buffer,chainBlock,BLOCKSIZE);
    InvCipher(buffer, expandedKey);
    //XORBytes(buffer, chainBlock, BLOCKSIZE);
    CopyBytes(chainBlock, buffer, BLOCKSIZE);
}

//对一个16字节块完成加密，参数buffer是加密缓存，chainBlock是要加密的块
void aesEncrypt(uint8_t * buffer, uint8_t * chainBlock)
{
    CopyBytes(buffer, chainBlock, BLOCKSIZE);
    //XORBytes(buffer, chainBlock, BLOCKSIZE);
    Cipher(buffer, expandedKey);
    CopyBytes(chainBlock, buffer, BLOCKSIZE);
}

//加解密函数，参数为加解密标志，要加解密的数据缓存起始指针，要加解密的数据长度（如果解密运算，必须是16的整数倍。）
uint8_t aesBlockDecrypt(uint8_t Direct,uint8_t *ChiperDataBuf,uint8_t DataLen)
{
    uint8_t  i;
    uint8_t  Blocks;
    uint8_t  sBoxbuf[256];
    uint8_t  tempbuf[16];
    unsigned long int  OrignLen=0; //未加密数据的原始长度

    if(Direct==0)
    {
        *((uint8_t *)&OrignLen+3)=ChiperDataBuf[0];
        *((uint8_t *)&OrignLen+2)=ChiperDataBuf[1];
        *((uint8_t *)&OrignLen+1)=ChiperDataBuf[2];
        *((uint8_t *)&OrignLen)=ChiperDataBuf[3];
        DataLen=DataLen-4;
    }
    else
    {
        memmove(ChiperDataBuf+4,ChiperDataBuf,DataLen);
        OrignLen=DataLen;
        ChiperDataBuf[0]=OrignLen;
        ChiperDataBuf[1]=OrignLen>>8;
        ChiperDataBuf[2]=OrignLen>>16;
        ChiperDataBuf[3]=OrignLen>>24;
    }
    cleardog();
    aes_init(sBoxbuf);   
    if(Direct==0)    
    {
        Blocks=DataLen/16;
        for(i=0;i<Blocks;i++)
        {
            cleardog();
            aesDecrypt(tempbuf,ChiperDataBuf+4+16*i);
        }
        memmove(ChiperDataBuf,ChiperDataBuf+4,OrignLen);
        cleardog();
        return(OrignLen);
    }
    else    //加密
    {
        if(DataLen%16!=0)
        {  
            Blocks=DataLen/16+1;
        }
        else
        {
            Blocks=DataLen/16;
        }

        for(i=0;i<Blocks;i++)
        {
            cleardog();
            aesEncrypt(tempbuf,ChiperDataBuf+4+16*i);
        }
        cleardog();
        return(Blocks*16+4);
    }
}

//#endif
