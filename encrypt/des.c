#include "stdio.h"  
#include "memory.h"  
#include "time.h"  
#include "stdlib.h"  

//��ʼ�û���IP  
int IP_Table[64] = {57,49,41,33,25,17,9,1,  
                    59,51,43,35,27,19,11,3,  
                    61,53,45,37,29,21,13,5,  
                    63,55,47,39,31,23,15,7,  
                    56,48,40,32,24,16,8,0,  
                    58,50,42,34,26,18,10,2,  
                    60,52,44,36,28,20,12,4,  
                    62,54,46,38,30,22,14,6};
//���ʼ�û���IP^-1  
int IP_1_Table[64] = {  39,7,47,15,55,23,63,31,  
                        38,6,46,14,54,22,62,30,  
                        37,5,45,13,53,21,61,29,  
                        36,4,44,12,52,20,60,28,  
                        35,3,43,11,51,19,59,27,  
                        34,2,42,10,50,18,58,26,  
                        33,1,41,9,49,17,57,25,  
                        32,0,40,8,48,16,56,24};  
  
//�����û���E
int E_Table[48] = {31, 0, 1, 2, 3, 4,  
                    3,  4, 5, 6, 7, 8,  
                    7,  8,9,10,11,12,  
                    11,12,13,14,15,16,  
                    15,16,17,18,19,20,  
                    19,20,21,22,23,24,  
                    23,24,25,26,27,28,  
                    27,28,29,30,31, 0};  
  
//�û�����P  
int P_Table[32] = {15,6,19,20,28,11,27,16,  
                    0,14,22,25,4,17,30,9,  
                    1,7,23,13,31,26,2,8,  
                    18,12,29,5,21,10,3,24};  
  
//S��  
int S[8][4][16] =//S1  
            {{{14,4,13,1,2,15,11,8,3,10,6,12,5,9,0,7},  
              {0,15,7,4,14,2,13,1,10,6,12,11,9,5,3,8},  
                {4,1,14,8,13,6,2,11,15,12,9,7,3,10,5,0},  
                {15,12,8,2,4,9,1,7,5,11,3,14,10,0,6,13}},  
                //S2  
              {{15,1,8,14,6,11,3,4,9,7,2,13,12,0,5,10},  
              {3,13,4,7,15,2,8,14,12,0,1,10,6,9,11,5},  
              {0,14,7,11,10,4,13,1,5,8,12,6,9,3,2,15},  
              {13,8,10,1,3,15,4,2,11,6,7,12,0,5,14,9}},  
              //S3  
              {{10,0,9,14,6,3,15,5,1,13,12,7,11,4,2,8},  
              {13,7,0,9,3,4,6,10,2,8,5,14,12,11,15,1},  
                {13,6,4,9,8,15,3,0,11,1,2,12,5,10,14,7},  
              {1,10,13,0,6,9,8,7,4,15,14,3,11,5,2,12}},  
              //S4  
              {{7,13,14,3,0,6,9,10,1,2,8,5,11,12,4,15},  
              {13,8,11,5,6,15,0,3,4,7,2,12,1,10,14,9},  
              {10,6,9,0,12,11,7,13,15,1,3,14,5,2,8,4},  
              {3,15,0,6,10,1,13,8,9,4,5,11,12,7,2,14}},  
              //S5  
              {{2,12,4,1,7,10,11,6,8,5,3,15,13,0,14,9},  
              {14,11,2,12,4,7,13,1,5,0,15,10,3,9,8,6},  
              {4,2,1,11,10,13,7,8,15,9,12,5,6,3,0,14},  
              {11,8,12,7,1,14,2,13,6,15,0,9,10,4,5,3}},  
              //S6  
              {{12,1,10,15,9,2,6,8,0,13,3,4,14,7,5,11},  
              {10,15,4,2,7,12,9,5,6,1,13,14,0,11,3,8},  
              {9,14,15,5,2,8,12,3,7,0,4,10,1,13,11,6},  
              {4,3,2,12,9,5,15,10,11,14,1,7,6,0,8,13}},  
              //S7  
              {{4,11,2,14,15,0,8,13,3,12,9,7,5,10,6,1},  
              {13,0,11,7,4,9,1,10,14,3,5,12,2,15,8,6},  
              {1,4,11,13,12,3,7,14,10,15,6,8,0,5,9,2},  
              {6,11,13,8,1,4,10,7,9,5,0,15,14,2,3,12}},  
              //S8  
              {{13,2,8,4,6,15,11,1,10,9,3,14,5,0,12,7},  
              {1,15,13,8,10,3,7,4,12,5,6,11,0,14,9,2},  
              {7,11,4,1,9,12,14,2,0,6,10,13,15,3,5,8},  
              {2,1,14,7,4,10,8,13,15,12,9,0,3,5,6,11}}};  
//�û�ѡ��1  
int PC_1[56] = {56,48,40,32,24,16,8,  
                0,57,49,41,33,25,17,  
                9,1,58,50,42,34,26,  
                18,10,2,59,51,43,35,  
                62,54,46,38,30,22,14,  
                6,61,53,45,37,29,21,  
                13,5,60,52,44,36,28,  
                20,12,4,27,19,11,3};  
  
//�û�ѡ��2  
int PC_2[48] = {13,16,10,23,0,4,2,27,  
                14,5,20,9,22,18,11,3,  
                25,7,15,6,26,19,12,1,  
                40,51,30,36,46,54,29,39,  
                50,44,32,46,43,48,38,55,  
                33,52,45,41,49,35,28,31};  
  
//�����ƴ����Ĺ涨  
int MOVE_TIMES[16] = {1,1,2,2,2,2,2,2,1,2,2,2,2,2,2,1};  

typedef char uint8_t;  
int ByteToBit(uint8_t ch,uint8_t bit[8]);  
int BitToByte(uint8_t bit[8],uint8_t *ch);  
int Char8ToBit64(uint8_t ch[8],uint8_t bit[64]);  
int Bit64ToChar8(uint8_t bit[64],uint8_t ch[8]);  
int DES_MakeSubKeys(uint8_t key[64],uint8_t subKeys[16][48]);  
int DES_PC1_Transform(uint8_t key[64], uint8_t tempbts[56]);  
int DES_PC2_Transform(uint8_t key[56], uint8_t tempbts[48]);  
int DES_ROL(uint8_t data[56], int time);  
int DES_IP_Transform(uint8_t data[64]);  
int DES_IP_1_Transform(uint8_t data[64]);  
int DES_E_Transform(uint8_t data[48]);  
int DES_P_Transform(uint8_t data[32]);  
int DES_SBOX(uint8_t data[48]);  
int DES_XOR(uint8_t R[48], uint8_t L[48],int count);  
int DES_Swap(uint8_t left[32],uint8_t right[32]);  
int DES_EncryptBlock(uint8_t plainBlock[8], uint8_t subKeys[16][48], uint8_t cipherBlock[8]);  
int DES_DecryptBlock(uint8_t cipherBlock[8], uint8_t subKeys[16][48], uint8_t plainBlock[8]);  

int DES_encrypt(uint8_t *data, char *keyStr,int lenth);
int DES_decrypt(uint8_t *data, char *keyStr,int lenth);

//�ֽ�ת���ɶ�����  
int ByteToBit(uint8_t ch, uint8_t bit[8]){  
    int cnt;  
    for(cnt = 0;cnt < 8; cnt++){  
        *(bit+cnt) = (ch>>cnt)&1;  
    }  
    return 0;  
}  
  
//������ת�����ֽ�  
int BitToByte(uint8_t bit[8],uint8_t *ch){  
    int cnt;  
    for(cnt = 0;cnt < 8; cnt++){  
        *ch |= *(bit + cnt);
    }  
    return 0;  
}  
  
//������Ϊ8���ַ���תΪ������λ��  
int Char8ToBit64(uint8_t ch[8],uint8_t bit[64])
{  
    int cnt;  
    for(cnt = 0; cnt < 8; cnt++){          
        ByteToBit(*(ch+cnt),bit+(cnt<<3));  
    }  
    return 0;  
}  
  
//��������λ��תΪ����Ϊ8���ַ���  
int Bit64ToChar8(uint8_t bit[64],uint8_t ch[8]){  
    int cnt;  
    memset(ch,0,8);  
    for(cnt = 0; cnt < 8; cnt++)
    {  
        BitToByte(bit+(cnt<<3),ch+cnt);  
    }  
    return 0;  
}  
  
//��������Կ  
int DES_MakeSubKeys(uint8_t key[64],uint8_t subKeys[16][48]){  
    uint8_t temp[56];  
    int cnt;  
    DES_PC1_Transform(key,temp);//PC1�û�  
    for(cnt = 0; cnt < 16; cnt++){//16�ֵ���������16������Կ  
        DES_ROL(temp,MOVE_TIMES[cnt]);//ѭ������  
        DES_PC2_Transform(temp,subKeys[cnt]);//PC2�û�����������Կ  
    }  
    return 0;  
}  
  
//��Կ�û�1  
int DES_PC1_Transform(uint8_t key[64], uint8_t tempbts[56]){  
    int cnt;      
    for(cnt = 0; cnt < 56; cnt++){  
        tempbts[cnt] = key[PC_1[cnt]];  
    }  
    return 0;  
}  
  
//��Կ�û�2  
int DES_PC2_Transform(uint8_t key[56], uint8_t tempbts[48]){  
    int cnt;  
    for(cnt = 0; cnt < 48; cnt++){  
        tempbts[cnt] = key[PC_2[cnt]];  
    }  
    return 0;  
}  
  
//ѭ������  
int DES_ROL(uint8_t data[56], int time){
    uint8_t temp[56];  
  
    //���潫Ҫѭ���ƶ����ұߵ�λ  
    memcpy(temp,data,time);  
    memcpy(temp+time,data+28,time);
      
    //ǰ28λ�ƶ�  
    memcpy(data,data+time,28-time);  
    memcpy(data+28-time,temp,time);  
  
    //��28λ�ƶ�  
    memcpy(data+28,data+28+time,28-time);  
    memcpy(data+56-time,temp+time,time);      
  
    return 0;  
}  
  
//IP�û�  
int DES_IP_Transform(uint8_t data[64]){  
    int cnt;  
    uint8_t temp[64];  
    for(cnt = 0; cnt < 64; cnt++){  
        temp[cnt] = data[IP_Table[cnt]];  
    }  
    memcpy(data,temp,64);  
    return 0;  
}  
  
//IP���û�  
int DES_IP_1_Transform(uint8_t data[64]){  
    int cnt;  
    uint8_t temp[64];  
    for(cnt = 0; cnt < 64; cnt++){  
        temp[cnt] = data[IP_1_Table[cnt]];  
    }  
    memcpy(data,temp,64);  
    return 0;  
}  
  
//��չ�û�  
int DES_E_Transform(uint8_t data[48]){  
    int cnt;  
    uint8_t temp[48];  
    for(cnt = 0; cnt < 48; cnt++){  
        temp[cnt] = data[E_Table[cnt]];  
    }     
    memcpy(data,temp,48);  
    return 0;  
}  
  
//P�û�  
int DES_P_Transform(uint8_t data[32]){  
    int cnt;  
    uint8_t temp[32];  
    for(cnt = 0; cnt < 32; cnt++){  
        temp[cnt] = data[P_Table[cnt]];  
    }     
    memcpy(data,temp,32);  
    return 0;  
}  
  
//���  
int DES_XOR(uint8_t R[48], uint8_t L[48] ,int count){  
    int cnt;  
    for(cnt = 0; cnt < count; cnt++){  
        R[cnt] ^= L[cnt];  
    }  
    return 0;  
}  
  
//S���û�  
int DES_SBOX(uint8_t data[48]){  
    int cnt;  
    int line,row,output;  
    int cur1,cur2;  
    for(cnt = 0; cnt < 8; cnt++){  
        cur1 = cnt*6;  
        cur2 = cnt<<2;  
          
        //������S���е�������  
        line = (data[cur1]<<1) + data[cur1+5];  
        row = (data[cur1+1]<<3) + (data[cur1+2]<<2)  
            + (data[cur1+3]<<1) + data[cur1+4];  
        output = S[cnt][line][row];  
  
        //��Ϊ2����  
        data[cur2] = (output&0X08)>>3;  
        data[cur2+1] = (output&0X04)>>2;  
        data[cur2+2] = (output&0X02)>>1;  
        data[cur2+3] = output&0x01;  
    }     
    return 0;  
}  
  
//����  
int DES_Swap(uint8_t left[32], uint8_t right[32]){  
    uint8_t temp[32];  
    memcpy(temp,left,32);     
    memcpy(left,right,32);    
    memcpy(right,temp,32);  
    return 0;  
}  
  
//���ܵ�������  
int DES_EncryptBlock(uint8_t plainBlock[8], uint8_t subKeys[16][48], uint8_t cipherBlock[8]){  
    uint8_t plainBits[64];  
    uint8_t copyRight[48];  
    int cnt;  
  
    Char8ToBit64(plainBlock,plainBits);       
    //��ʼ�û���IP�û���  
    DES_IP_Transform(plainBits);  
  
    //16�ֵ���  
    for(cnt = 0; cnt < 16; cnt++){         
        memcpy(copyRight,plainBits+32,32);  
        //���Ұ벿�ֽ�����չ�û�����32λ��չ��48λ  
        DES_E_Transform(copyRight);  
        //���Ұ벿��������Կ����������  
        DES_XOR(copyRight,subKeys[cnt],48);   
        //���������S�У����32λ���  
        DES_SBOX(copyRight);  
        //P�û�  
        DES_P_Transform(copyRight);  
        //��������벿�����Ұ벿�ֽ������  
        DES_XOR(plainBits,copyRight,32);  
        if(cnt != 15){  
            //����������Ҳ��Ľ���  
            DES_Swap(plainBits,plainBits+32);  
        }  
    }  
    //���ʼ�û���IP^1�û���  
    DES_IP_1_Transform(plainBits);  
    Bit64ToChar8(plainBits,cipherBlock);  
    return 0;  
}  
  
//���ܵ�������  
int DES_DecryptBlock(uint8_t cipherBlock[8], uint8_t subKeys[16][48],uint8_t plainBlock[8]){  
    uint8_t cipherBits[64];  
    uint8_t copyRight[48];  
    int cnt;  
  
    Char8ToBit64(cipherBlock,cipherBits);         
    //��ʼ�û���IP�û���  
    DES_IP_Transform(cipherBits);  
      
    //16�ֵ���  
    for(cnt = 15; cnt >= 0; cnt--){        
        memcpy(copyRight,cipherBits+32,32);  
        //���Ұ벿�ֽ�����չ�û�����32λ��չ��48λ  
        DES_E_Transform(copyRight);  
        //���Ұ벿��������Կ����������  
        DES_XOR(copyRight,subKeys[cnt],48);       
        //���������S�У����32λ���  
        DES_SBOX(copyRight);  
        //P�û�  
        DES_P_Transform(copyRight);       
        //��������벿�����Ұ벿�ֽ������  
        DES_XOR(cipherBits,copyRight,32);  
        if(cnt != 0){  
            //����������Ҳ��Ľ���  
            DES_Swap(cipherBits,cipherBits+32);  
        }  
    }  
    //���ʼ�û���IP^1�û���  
    DES_IP_1_Transform(cipherBits);  
    Bit64ToChar8(cipherBits,plainBlock);  
    return 0;  
}  


//��������
int DES_encrypt(uint8_t *data, char *keyStr,int lenth)
{  
    uint8_t cipherBlock[8],keyBlock[8];  
    uint8_t bKey[64];  
    uint8_t subKeys[16][48];  
    int i,idx,block_cnt;

    memcpy(keyBlock,keyStr,8);  
    Char8ToBit64(keyBlock,bKey);
    DES_MakeSubKeys(bKey,subKeys);  
    block_cnt = (lenth+7)/8;
    for(i = 0;i < block_cnt;i ++)
    {
        idx = (i << 3);
        if(lenth - idx < 8)
        {
            memset(&data[lenth],0,8 + idx - lenth);
        }
        DES_EncryptBlock(&data[idx],subKeys,cipherBlock); 
        memcpy(&data[idx],cipherBlock,8);
    }
    idx = (block_cnt << 3);
    for(i = 0;i < 4;i ++)
    {
        data[idx++] = (uint8_t)(lenth >> (8*i));
    }
    return idx + 4;
}  

//�����ļ�  
int DES_decrypt(uint8_t *data, char *keyStr,int lenth)
{  
    int len;
    int i,idx,block_cnt;
    uint8_t plainBlock[8],keyBlock[8];  
    uint8_t bKey[64];
    uint8_t subKeys[16][48];
  
    memcpy(keyBlock,keyStr,8);  
    Char8ToBit64(keyBlock,bKey);  
    DES_MakeSubKeys(bKey,subKeys);
    len = 0;
    idx  = lenth - 4;
    for(i = 3;i >= 0;i --)
    {
        lenth = lenth * 256 + data[idx+i];
    }
    block_cnt = lenth / 8;
  
    for(i = 0;i < block_cnt;i ++)
    {
        idx = (i << 3);
        DES_DecryptBlock(&data[idx],subKeys,plainBlock); 
        memcpy(&data[idx],plainBlock,8);
    }
    return len;  
}  


int main()  
{     
    clock_t a,b;  
    a = clock();  
    DES_encrypt("1.txt","key.txt","2.txt");  
    b = clock();  
    printf("��������%d����\n",b-a);
      
    system("pause");  
    a = clock();  
    DES_decrypt("2.txt","key.txt","3.txt");
    b = clock();  
    printf("��������%d����\n",b-a);  
    getchar();  
    return 0;  
}  
=======
#include "boot_type.h"
#include "encrypt.h"
//��ʼ�û���IP  
int IP_Table[64] = {  57,49,41,33,25,17,9,1,  
                                 59,51,43,35,27,19,11,3,  
                                 61,53,45,37,29,21,13,5,  
                                 63,55,47,39,31,23,15,7,  
                                 56,48,40,32,24,16,8,0,  
                                 58,50,42,34,26,18,10,2,  
                                 60,52,44,36,28,20,12,4,  
                                 62,54,46,38,30,22,14,6};   
//���ʼ�û���IP^-1  
int IP_1_Table[64] = {39,7,47,15,55,23,63,31,  
           38,6,46,14,54,22,62,30,  
           37,5,45,13,53,21,61,29,  
           36,4,44,12,52,20,60,28,  
           35,3,43,11,51,19,59,27,  
           34,2,42,10,50,18,58,26,  
           33,1,41,9,49,17,57,25,  
           32,0,40,8,48,16,56,24};  
  
//�����û���E  
int E_Table[48] = {31, 0, 1, 2, 3, 4,  
                  3,  4, 5, 6, 7, 8,  
                  7,  8,9,10,11,12,  
                  11,12,13,14,15,16,  
                  15,16,17,18,19,20,  
                  19,20,21,22,23,24,  
                  23,24,25,26,27,28,  
                  27,28,29,30,31, 0};  
  
//�û�����P  
int P_Table[32] = {15,6,19,20,28,11,27,16,  
                  0,14,22,25,4,17,30,9,  
                  1,7,23,13,31,26,2,8,  
                  18,12,29,5,21,10,3,24};  
  
//S��  
int S[8][4][16] =//S1  
            {{{14,4,13,1,2,15,11,8,3,10,6,12,5,9,0,7},  
              {0,15,7,4,14,2,13,1,10,6,12,11,9,5,3,8},  
                {4,1,14,8,13,6,2,11,15,12,9,7,3,10,5,0},  
                {15,12,8,2,4,9,1,7,5,11,3,14,10,0,6,13}},  
                //S2  
              {{15,1,8,14,6,11,3,4,9,7,2,13,12,0,5,10},  
              {3,13,4,7,15,2,8,14,12,0,1,10,6,9,11,5},  
              {0,14,7,11,10,4,13,1,5,8,12,6,9,3,2,15},  
              {13,8,10,1,3,15,4,2,11,6,7,12,0,5,14,9}},  
              //S3  
              {{10,0,9,14,6,3,15,5,1,13,12,7,11,4,2,8},  
              {13,7,0,9,3,4,6,10,2,8,5,14,12,11,15,1},  
                {13,6,4,9,8,15,3,0,11,1,2,12,5,10,14,7},  
              {1,10,13,0,6,9,8,7,4,15,14,3,11,5,2,12}},  
              //S4  
              {{7,13,14,3,0,6,9,10,1,2,8,5,11,12,4,15},  
              {13,8,11,5,6,15,0,3,4,7,2,12,1,10,14,9},  
              {10,6,9,0,12,11,7,13,15,1,3,14,5,2,8,4},  
              {3,15,0,6,10,1,13,8,9,4,5,11,12,7,2,14}},  
              //S5  
              {{2,12,4,1,7,10,11,6,8,5,3,15,13,0,14,9},  
              {14,11,2,12,4,7,13,1,5,0,15,10,3,9,8,6},  
              {4,2,1,11,10,13,7,8,15,9,12,5,6,3,0,14},  
              {11,8,12,7,1,14,2,13,6,15,0,9,10,4,5,3}},  
              //S6  
              {{12,1,10,15,9,2,6,8,0,13,3,4,14,7,5,11},  
              {10,15,4,2,7,12,9,5,6,1,13,14,0,11,3,8},  
              {9,14,15,5,2,8,12,3,7,0,4,10,1,13,11,6},  
              {4,3,2,12,9,5,15,10,11,14,1,7,6,0,8,13}},  
              //S7  
              {{4,11,2,14,15,0,8,13,3,12,9,7,5,10,6,1},  
              {13,0,11,7,4,9,1,10,14,3,5,12,2,15,8,6},  
              {1,4,11,13,12,3,7,14,10,15,6,8,0,5,9,2},  
              {6,11,13,8,1,4,10,7,9,5,0,15,14,2,3,12}},  
              //S8  
              {{13,2,8,4,6,15,11,1,10,9,3,14,5,0,12,7},  
              {1,15,13,8,10,3,7,4,12,5,6,11,0,14,9,2},  
              {7,11,4,1,9,12,14,2,0,6,10,13,15,3,5,8},  
              {2,1,14,7,4,10,8,13,15,12,9,0,3,5,6,11}}};  
//�û�ѡ��1  
int PC_1[56] = {56,48,40,32,24,16,8,  
              0,57,49,41,33,25,17,  
              9,1,58,50,42,34,26,  
              18,10,2,59,51,43,35,  
              62,54,46,38,30,22,14,  
              6,61,53,45,37,29,21,  
              13,5,60,52,44,36,28,  
              20,12,4,27,19,11,3};  
  
//�û�ѡ��2  
int PC_2[48] = {13,16,10,23,0,4,2,27,  
              14,5,20,9,22,18,11,3,  
              25,7,15,6,26,19,12,1,  
              40,51,30,36,46,54,29,39,  
              50,44,32,46,43,48,38,55,  
              33,52,45,41,49,35,28,31};  
  
//�����ƴ����Ĺ涨  
int MOVE_TIMES[16] = {1,1,2,2,2,2,2,2,1,2,2,2,2,2,2,1};  

int ByteToBit(uint8_t ch,uint8_t bit[8]);  
int BitToByte(uint8_t bit[8],uint8_t *ch);  
int Char8ToBit64(uint8_t ch[8],uint8_t bit[64]);  
int Bit64ToChar8(uint8_t bit[64],uint8_t ch[8]);  
int DES_MakeSubKeys(uint8_t key[64],uint8_t subKeys[16][48]);  
int DES_PC1_Transform(uint8_t key[64], uint8_t tempbts[56]);  
int DES_PC2_Transform(uint8_t key[56], uint8_t tempbts[48]);  
int DES_ROL(uint8_t data[56], int time);  
int DES_IP_Transform(uint8_t data[64]);  
int DES_IP_1_Transform(uint8_t data[64]);  
int DES_E_Transform(uint8_t data[48]);  
int DES_P_Transform(uint8_t data[32]);  
int DES_SBOX(uint8_t data[48]);  
int DES_XOR(uint8_t R[48], uint8_t L[48],int count);  
int DES_Swap(uint8_t left[32],uint8_t right[32]);  
int DES_EncryptBlock(uint8_t plainBlock[8], uint8_t subKeys[16][48], uint8_t cipherBlock[8]);  
int DES_DecryptBlock(uint8_t cipherBlock[8], uint8_t subKeys[16][48], uint8_t plainBlock[8]);  

static void mem_cpy(uint8_t *dest,uint8_t *src,int len)
{
    int i;
    for(i = 0;i < len;i ++)
        dest[i] = src[i];
}

static void mem_set(uint8_t *dest,uint8_t value,int len)
{
    int i;
    for(i = 0;i < len;i ++)
        dest[i] = value;
}

//�ֽ�ת���ɶ�����  
int ByteToBit(uint8_t ch, uint8_t bit[8]){  
    int cnt;  
    for(cnt = 0;cnt < 8; cnt++){  
        *(bit+cnt) = (ch>>cnt)&1;  
    }  
    return 0;  
}  
  
//������ת�����ֽ�  
int BitToByte(uint8_t bit[8],uint8_t *ch){  
    int cnt;  
    for(cnt = 0;cnt < 8; cnt++){  
        *ch |= *(bit + cnt)<<cnt;  
    }  
    return 0;  
}  
  
//������Ϊ8���ַ���תΪ������λ��  
int Char8ToBit64(uint8_t ch[8],uint8_t bit[64]){  
    int cnt;  
    for(cnt = 0; cnt < 8; cnt++){          
        ByteToBit(*(ch+cnt),bit+(cnt<<3));  
    }  
    return 0;  
}  
  
//��������λ��תΪ����Ϊ8���ַ���  
int Bit64ToChar8(uint8_t bit[64],uint8_t ch[8]){  
    int cnt;  
    mem_set(ch,0,8);  
    for(cnt = 0; cnt < 8; cnt++){  
        BitToByte(bit+(cnt<<3),ch+cnt);  
    }  
    return 0;  
}  
  
//��������Կ  
int DES_MakeSubKeys(uint8_t key[64],uint8_t subKeys[16][48]){  
    uint8_t temp[56];  
    int cnt;  
    DES_PC1_Transform(key,temp);
    for(cnt = 0; cnt < 16; cnt++){
        DES_ROL(temp,MOVE_TIMES[cnt]);
        DES_PC2_Transform(temp,subKeys[cnt]);
    }  
    return 0;  
}  
  
//��Կ�û�1  
int DES_PC1_Transform(uint8_t key[64], uint8_t tempbts[56]){  
    int cnt;      
    for(cnt = 0; cnt < 56; cnt++){  
        tempbts[cnt] = key[PC_1[cnt]];  
    }  
    return 0;  
}  
  
//��Կ�û�2  
int DES_PC2_Transform(uint8_t key[56], uint8_t tempbts[48]){  
    int cnt;  
    for(cnt = 0; cnt < 48; cnt++){  
        tempbts[cnt] = key[PC_2[cnt]];  
    }  
    return 0;  
}  
  
//ѭ������  
int DES_ROL(uint8_t data[56], int time){     
    uint8_t temp[56];  
  
    //���潫Ҫѭ���ƶ����ұߵ�λ  
    mem_cpy(temp,data,time);  
    mem_cpy(temp+time,data+28,time);  
      
    //ǰ28λ�ƶ�  
    mem_cpy(data,data+time,28-time);  
    mem_cpy(data+28-time,temp,time);  
  
    //��28λ�ƶ�  
    mem_cpy(data+28,data+28+time,28-time);  
    mem_cpy(data+56-time,temp+time,time);      
  
    return 0;  
}  
  
//IP�û�  
int DES_IP_Transform(uint8_t data[64]){  
    int cnt;  
    uint8_t temp[64];  
    for(cnt = 0; cnt < 64; cnt++){  
        temp[cnt] = data[IP_Table[cnt]];  
    }  
    mem_cpy(data,temp,64);  
    return 0;  
}  
  
//IP���û�  
int DES_IP_1_Transform(uint8_t data[64]){  
    int cnt;  
    uint8_t temp[64];  
    for(cnt = 0; cnt < 64; cnt++){  
        temp[cnt] = data[IP_1_Table[cnt]];  
    }  
    mem_cpy(data,temp,64);  
    return 0;  
}  
  
//��չ�û�  
int DES_E_Transform(uint8_t data[48]){  
    int cnt;  
    uint8_t temp[48];  
    for(cnt = 0; cnt < 48; cnt++){  
        temp[cnt] = data[E_Table[cnt]];  
    }     
    mem_cpy(data,temp,48);  
    return 0;  
}  
  
//P�û�  
int DES_P_Transform(uint8_t data[32]){  
    int cnt;  
    uint8_t temp[32];  
    for(cnt = 0; cnt < 32; cnt++){  
        temp[cnt] = data[P_Table[cnt]];  
    }     
    mem_cpy(data,temp,32);  
    return 0;  
}  
  
//���  
int DES_XOR(uint8_t R[48], uint8_t L[48] ,int count){  
    int cnt;  
    for(cnt = 0; cnt < count; cnt++){  
        R[cnt] ^= L[cnt];  
    }  
    return 0;  
}  
  
//S���û�  
int DES_SBOX(uint8_t data[48]){  
    int cnt;  
    int line,row,output;  
    int cur1,cur2;  
    for(cnt = 0; cnt < 8; cnt++){  
        cur1 = cnt*6;  
        cur2 = cnt<<2;  
          
        //������S���е�������  
        line = (data[cur1]<<1) + data[cur1+5];  
        row = (data[cur1+1]<<3) + (data[cur1+2]<<2)  
            + (data[cur1+3]<<1) + data[cur1+4];  
        output = S[cnt][line][row];  
  
        //��Ϊ2����  
        data[cur2] = (output&0X08)>>3;  
        data[cur2+1] = (output&0X04)>>2;  
        data[cur2+2] = (output&0X02)>>1;  
        data[cur2+3] = output&0x01;  
    }     
    return 0;  
}  
  
//����  
int DES_Swap(uint8_t left[32], uint8_t right[32]){  
    uint8_t temp[32];  
    mem_cpy(temp,left,32);     
    mem_cpy(left,right,32);    
    mem_cpy(right,temp,32);  
    return 0;  
}  
  
//���ܵ�������  
int DES_EncryptBlock(uint8_t plainBlock[8], uint8_t subKeys[16][48], uint8_t cipherBlock[8]){  
    uint8_t plainBits[64];  
    uint8_t copyRight[48];  
    int cnt;  
  
    Char8ToBit64(plainBlock,plainBits);       
    //��ʼ�û���IP�û���  
    DES_IP_Transform(plainBits);  
  
    //16�ֵ���  
    for(cnt = 0; cnt < 16; cnt++){         
        mem_cpy(copyRight,plainBits+32,32);  
        //���Ұ벿�ֽ�����չ�û�����32λ��չ��48λ  
        DES_E_Transform(copyRight);  
        //���Ұ벿��������Կ����������  
        DES_XOR(copyRight,subKeys[cnt],48);   
        //���������S�У����32λ���  
        DES_SBOX(copyRight);  
        //P�û�  
        DES_P_Transform(copyRight);  
        //��������벿�����Ұ벿�ֽ������  
        DES_XOR(plainBits,copyRight,32);  
        if(cnt != 15){  
            //����������Ҳ��Ľ���  
            DES_Swap(plainBits,plainBits+32);  
        }  
    }  
    //���ʼ�û���IP^1�û���  
    DES_IP_1_Transform(plainBits);  
    Bit64ToChar8(plainBits,cipherBlock);  
    return 0;  
}  
  
//���ܵ�������  
int DES_DecryptBlock(uint8_t cipherBlock[8], uint8_t subKeys[16][48],uint8_t plainBlock[8])
{  
    uint8_t cipherBits[64];  
    uint8_t copyRight[48];  
    int cnt;  
  
    Char8ToBit64(cipherBlock,cipherBits);         
    //��ʼ�û���IP�û���  
    DES_IP_Transform(cipherBits);  
      
    //16�ֵ���  
    for(cnt = 15; cnt >= 0; cnt--){        
        mem_cpy(copyRight,cipherBits+32,32);  
        //���Ұ벿�ֽ�����չ�û�����32λ��չ��48λ  
        DES_E_Transform(copyRight);  
        //���Ұ벿��������Կ����������  
        DES_XOR(copyRight,subKeys[cnt],48);       
        //���������S�У����32λ���  
        DES_SBOX(copyRight);  
        //P�û�  
        DES_P_Transform(copyRight);       
        //��������벿�����Ұ벿�ֽ������  
        DES_XOR(cipherBits,copyRight,32);  
        if(cnt != 0){  
            //����������Ҳ��Ľ���  
            DES_Swap(cipherBits,cipherBits+32);  
        }  
    }  
    //���ʼ�û���IP^1�û���  
    DES_IP_1_Transform(cipherBits);  
    Bit64ToChar8(cipherBits,plainBlock);  
    return 0;  
}  

static uint8_t keyStr[] = ENCRYPT_KEY;
//�����ļ�  
int DES_Encrypt(uint8_t *data,int datalen)
{  
    int i,idx;
    int blocks;
    int encrypt_len;
    uint8_t cipherBlock[8],keyBlock[8];  
    uint8_t bKey[64];  
    uint8_t subKeys[16][48];  

    mem_cpy(keyBlock,keyStr,8);  
    Char8ToBit64(keyBlock,bKey);  
    DES_MakeSubKeys(bKey,subKeys);
    
    encrypt_len = (((datalen+7) >> 3) << 3);
    for(i = datalen;i < encrypt_len;i ++)
        data[i] = 0;
    blocks = ((datalen+7) >> 3); 
    for(i=0;i<blocks;i++)
    {
        //cleardog();
        idx = (i << 3);
        DES_EncryptBlock(&data[idx],subKeys,cipherBlock);  
        mem_cpy(&data[idx],cipherBlock,8);
    }
    
    return encrypt_len;  
}  

//�����ļ�  
int DES_Decrypt(uint8_t *data,int datalen)
{  
    int i,idx;
    int blocks;
    uint8_t plainBlock[8],keyBlock[8];  
    uint8_t bKey[64];  
    uint8_t subKeys[16][48];  
  
    if(datalen&0x07)
        return -1;
    
    mem_cpy(keyBlock,keyStr,8);  
    Char8ToBit64(keyBlock,bKey);  
    DES_MakeSubKeys(bKey,subKeys);  
  
    blocks = (datalen>>3);
    for(i=0;i<blocks;i++)
    {
        //cleardog();
        idx = (i << 3);
        DES_DecryptBlock(&data[idx],subKeys,plainBlock);                         
        mem_cpy(&data[idx],plainBlock,8);
    }
    return datalen;  
}  

