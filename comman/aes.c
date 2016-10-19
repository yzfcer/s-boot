#include <string.h>
#include "aes.h"


#define AES_BPOLY 0x1b //!< Lower 8 bits of (x^8+x^4+x^3+x+1), ie. (x^4+x^3+x+1).
#define AES_BLOCKSIZE 16 //!< Block size in number of uint8_ts.

#define AES_KEYBITS 128 //!< Use AES128.
#define AES_ROUNDS 10 //!< Number of rounds.
#define AES_KEYLENGTH 16 //!< Key length in number of uint8_ts.


static uint8_t  block1[256]; //!< Workspace 1.
static uint8_t  block2[256]; //!< Worksapce 2.

static uint8_t  * powTbl; //!< Final location of exponentiation lookup table.
static uint8_t  * logTbl; //!< Final location of logarithm lookup table.
static uint8_t  * sbox; //!< Final location of s-box.
static uint8_t  * sbox_inv; //!< Final location of inverse s-box.
static uint8_t  * expand_key; //!< Final location of expanded key.

void feed_watchdog(void)
{
}

void calc_pow_log(uint8_t * powTbl, uint8_t * logTbl)
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
        t ^= (t << 1) ^ (t & 0x80 ? AES_BPOLY : 0);
    } while(t != 1); // Cyclic properties ensure that i < 255.
    powTbl[255] = powTbl[0]; // 255 = '-0', 254 = -1, etc.
}



void calc_sbox(uint8_t * sBox)
{
    uint8_t  i, rot;
    uint8_t  temp;
    uint8_t  result;

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



void calc_sbox_inv(uint8_t * sBox, uint8_t * sbox_inv)
{
    uint8_t  i = 0;
    uint8_t  j = 0;

    // Iterate through all elements in sbox_inv using  i.
    do 
    {
        // Search through sBox using j.
        feed_watchdog();
        do 
        {
            // Check if current j is the inverse of current i.
            if(sBox[ j ] == i) 
            {
                // If so, set sBoxInc and indicate search finished.
                sbox_inv[ i ] = j;
                j = 255;
            }
        } while(++j != 0);
    } while(++i != 0);
}



void cycle_left(uint8_t * row)
{
    // Cycle 4 uint8_ts in an array left once.
    uint8_t  temp = row[0];
    row[0] = row[1];
    row[1] = row[2];
    row[2] = row[3];
    row[3] = temp;
}



void inv_mix_column(uint8_t * column)
{
    uint8_t  r0, r1, r2, r3;

    r0 = column[1] ^ column[2] ^ column[3];
    r1 = column[0] ^ column[2] ^ column[3];
    r2 = column[0] ^ column[1] ^ column[3];
    r3 = column[0] ^ column[1] ^ column[2];

    column[0] = (column[0] << 1) ^ (column[0] & 0x80 ? AES_BPOLY : 0);
    column[1] = (column[1] << 1) ^ (column[1] & 0x80 ? AES_BPOLY : 0);
    column[2] = (column[2] << 1) ^ (column[2] & 0x80 ? AES_BPOLY : 0);
    column[3] = (column[3] << 1) ^ (column[3] & 0x80 ? AES_BPOLY : 0);

    r0 ^= column[0] ^ column[1];
    r1 ^= column[1] ^ column[2];
    r2 ^= column[2] ^ column[3];
    r3 ^= column[0] ^ column[3];

    column[0] = (column[0] << 1) ^ (column[0] & 0x80 ? AES_BPOLY : 0);
    column[1] = (column[1] << 1) ^ (column[1] & 0x80 ? AES_BPOLY : 0);
    column[2] = (column[2] << 1) ^ (column[2] & 0x80 ? AES_BPOLY : 0);
    column[3] = (column[3] << 1) ^ (column[3] & 0x80 ? AES_BPOLY : 0);

    r0 ^= column[0] ^ column[2];
    r1 ^= column[1] ^ column[3];
    r2 ^= column[0] ^ column[2];
    r3 ^= column[1] ^ column[3];

    column[0] = (column[0] << 1) ^ (column[0] & 0x80 ? AES_BPOLY : 0);
    column[1] = (column[1] << 1) ^ (column[1] & 0x80 ? AES_BPOLY : 0);
    column[2] = (column[2] << 1) ^ (column[2] & 0x80 ? AES_BPOLY : 0);
    column[3] = (column[3] << 1) ^ (column[3] & 0x80 ? AES_BPOLY : 0);

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

uint8_t multiply(uint8_t num, uint8_t factor)
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
        num = (num << 1) ^ (num & 0x80 ? AES_BPOLY : 0);
    }

    return result;
}


uint8_t dot_product(uint8_t * vector1, uint8_t * vector2)
{
    uint8_t result = 0;

    result ^= multiply(*vector1++, *vector2++);
    result ^= multiply(*vector1++, *vector2++);
    result ^= multiply(*vector1++, *vector2++);
    result ^= multiply(*vector1  , *vector2  );

    return result;
}

void mix_column(uint8_t * column)
{
    // Prepare first row of matrix twice, to eliminate need for cycling.
    uint8_t  row[8] = 
    {0x02, 0x03, 0x01, 0x01,
    0x02, 0x03, 0x01, 0x01};

    uint8_t  result[4];
     
    // Take dot products of each matrix row and the column vector.
    result[0] = dot_product(row+0, column);
    result[1] = dot_product(row+3, column);
    result[2] = dot_product(row+2, column);
    result[3] = dot_product(row+1, column);

    // Copy temporary result to original column.
    column[0] = result[0];
    column[1] = result[1];
    column[2] = result[2];
    column[3] = result[3];
}


void sub_bytes(uint8_t * uint8_ts, uint8_t count)
{
    do 
    {
        // Substitute every uint8_t in state.
        *uint8_ts = sbox[ *uint8_ts ]; 
        uint8_ts++;
    } while(--count);
}



void inv_subbytes_and_xor(uint8_t * uint8_ts, uint8_t * key, uint8_t count)
{
    do 
    {
        //  *uint8_ts = sbox_inv[ *uint8_ts ] ^ *key; // Inverse substitute every uint8_t in state and add key.
        *uint8_ts = block2[ *uint8_ts ] ^ *key; // Use block2 directly. Increases speed.
        uint8_ts++;
        key++;
    } while(--count);
}



void inv_shift_rows(uint8_t * state)
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

void shift_rows(uint8_t * state)
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


void inv_mix_columns(uint8_t * state)
{
    inv_mix_column(state + 0*4);
    inv_mix_column(state + 1*4);
    inv_mix_column(state + 2*4);
    inv_mix_column(state + 3*4);
}

void mix_columns(uint8_t * state)
{
    mix_column(state + 0*4);
    mix_column(state + 1*4);
    mix_column(state + 2*4);
    mix_column(state + 3*4);
}


void xor_bytes(uint8_t * uint8_ts1, uint8_t * uint8_ts2, uint8_t count)
{
    do 
    {
        *uint8_ts1 ^= *uint8_ts2; // Add in GF(2), ie. XOR.
        uint8_ts1++;
        uint8_ts2++;
    } while(--count);
}



void copy_bytes(uint8_t * to, uint8_t * from, uint8_t count)
{
    do 
    {
        *to = *from;
        to++;
        from++;
    } while(--count);
}



void key_expansion(uint8_t * expand_key)
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
    i = AES_KEYLENGTH;
    do 
    {
        *expand_key = *key;
        expand_key++;
        key++;
    } while(--i);

    // Prepare last 4 uint8_ts of key in temp.
    expand_key -= 4;
    temp[0] = *(expand_key++);
    temp[1] = *(expand_key++);
    temp[2] = *(expand_key++);
    temp[3] = *(expand_key++);

    // Expand key.
    i = AES_KEYLENGTH;
    while(i < AES_BLOCKSIZE*(AES_ROUNDS+1)) 
    {
        // Are we at the start of a multiple of the key size?
        if((i % AES_KEYLENGTH) == 0) 
        {
            cycle_left(temp); // Cycle left once.
            sub_bytes(temp, 4); // Substitute each uint8_t.
            xor_bytes(temp, Rcon, 4); // Add constant in GF(2).
            *Rcon = (*Rcon << 1) ^ (*Rcon & 0x80 ? AES_BPOLY : 0);
        }

        // Keysize larger than 24 uint8_ts, ie. larger that 192 bits?
#if AES_KEYLENGTH > 24
        // Are we right past a block size?
        else if((i % AES_KEYLENGTH) == AES_BLOCKSIZE) 
        {
            sub_bytes(temp, 4); // Substitute each uint8_t.
        }
#endif

        // Add uint8_ts in GF(2) one AES_KEYLENGTH away.
        xor_bytes(temp, expand_key - AES_KEYLENGTH, 4);

        // Copy result to current 4 uint8_ts.
        *(expand_key++) = temp[ 0 ];
        *(expand_key++) = temp[ 1 ];
        *(expand_key++) = temp[ 2 ];
        *(expand_key++) = temp[ 3 ];
        i += 4; // Next 4 uint8_ts.
    } 
}



void inv_cipher(uint8_t * block, uint8_t * expand_key)
{
    uint8_t round = AES_ROUNDS-1;
    expand_key += AES_BLOCKSIZE * AES_ROUNDS;

    xor_bytes(block, expand_key, 16);
    expand_key -= AES_BLOCKSIZE;
    do 
    {
        inv_shift_rows(block);
        inv_subbytes_and_xor(block, expand_key, 16);
        expand_key -= AES_BLOCKSIZE;
        inv_mix_columns(block);
    } while(--round);

    inv_shift_rows(block);
    inv_subbytes_and_xor(block, expand_key, 16);
}

void cipher(uint8_t * block, uint8_t * expand_key)    //完成一个块(16字节，128bit)的加密
{
    uint8_t round = AES_ROUNDS-1;

    xor_bytes(block, expand_key, 16);
    expand_key += AES_BLOCKSIZE;
    do 
    {
        sub_bytes(block, 16);
        shift_rows(block);
        mix_columns(block);
        xor_bytes(block, expand_key, 16);
        expand_key += AES_BLOCKSIZE;
    } while(--round);

    sub_bytes(block, 16);
    shift_rows(block);
    xor_bytes(block, expand_key, 16);
}

void aes_init(uint8_t * tempbuf)
{
    powTbl = block1;
    logTbl = block2;
    calc_pow_log(powTbl, logTbl);

    sbox = tempbuf;
    calc_sbox(sbox);
    expand_key = block1;  //至此block1用来存贮密码表
    key_expansion(expand_key);
    sbox_inv = block2; // Must be block2. block2至此开始只用来存贮SBOXINV
    calc_sbox_inv(sbox, sbox_inv);
} 


//对一个16字节块解密,参数buffer是解密密缓存，chainBlock是要解密的块
void aes_decrypt(uint8_t * buffer, uint8_t * chainBlock)
{
    copy_bytes(buffer,chainBlock,AES_BLOCKSIZE);
    inv_cipher(buffer, expand_key);
    //xor_bytes(buffer, chainBlock, AES_BLOCKSIZE);
    copy_bytes(chainBlock, buffer, AES_BLOCKSIZE);
}

//对一个16字节块完成加密，参数buffer是加密缓存，chainBlock是要加密的块
void aes_encrypt(uint8_t * buffer, uint8_t * chainBlock)
{
    copy_bytes(buffer, chainBlock, AES_BLOCKSIZE);
    //xor_bytes(buffer, chainBlock, AES_BLOCKSIZE);
    cipher(buffer, expand_key);
    copy_bytes(chainBlock, buffer, AES_BLOCKSIZE);
}

//加解密函数，参数为加解密标志，要加解密的数据缓存起始指针，要加解密的数据长度（如果解密运算，必须是16的整数倍。）
uint8_t aes_block_decrypt(uint8_t direct,uint8_t *ChiperDataBuf,uint8_t DataLen)
{
    uint8_t  i;
    uint8_t  block_cnt;
    uint8_t  sbox_buf[256];
    uint8_t  tempbuf[16];
    unsigned long int  orign_len=0; //未加密数据的原始长度

    if(direct==0)
    {
        *((uint8_t *)&orign_len+3)=ChiperDataBuf[0];
        *((uint8_t *)&orign_len+2)=ChiperDataBuf[1];
        *((uint8_t *)&orign_len+1)=ChiperDataBuf[2];
        *((uint8_t *)&orign_len)=ChiperDataBuf[3];
        DataLen=DataLen-4;
    }
    else
    {
        memmove(ChiperDataBuf+4,ChiperDataBuf,DataLen);
        orign_len=DataLen;
        ChiperDataBuf[0]=orign_len;
        ChiperDataBuf[1]=orign_len>>8;
        ChiperDataBuf[2]=orign_len>>16;
        ChiperDataBuf[3]=orign_len>>24;
    }
    feed_watchdog();
    aes_init(sbox_buf);   
    if(direct==0)    
    {
        block_cnt=DataLen/16;
        for(i=0;i<block_cnt;i++)
        {
            feed_watchdog();
            aes_decrypt(tempbuf,ChiperDataBuf+4+16*i);
        }
        memmove(ChiperDataBuf,ChiperDataBuf+4,orign_len);
        feed_watchdog();
        return(orign_len);
    }
    else    //加密
    {
        if(DataLen%16!=0)
        {  
            block_cnt=DataLen/16+1;
        }
        else
        {
            block_cnt=DataLen/16;
        }

        for(i=0;i<block_cnt;i++)
        {
            feed_watchdog();
            aes_encrypt(tempbuf,ChiperDataBuf+4+16*i);
        }
        feed_watchdog();
        return(block_cnt*16+4);
    }
}

//#endif
