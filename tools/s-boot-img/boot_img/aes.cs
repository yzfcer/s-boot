using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

//#define AES_BPOLY 0x1b 
//#define AES_BLKSIZE 16 

//#define AES_KEYBITS 128 
//#define AES_ROUNDS 10 
//#define AES_KEYLEN 16 
namespace boot_img
{
    class AES:Crypt
    {
        public const byte AES_BPOLY = 0x1b; 
        public const int AES_BLKSIZE = 16;
        public const int AES_KEYBITS = 128;
        public const int AES_ROUNDS = 10;
        public const int AES_KEYLEN = 16;

        byte[]  aes_block1; 
        byte[]  aes_block2; 

        byte[] powTbl; 
        byte[] logTbl; 
        byte[] sBox; 
        byte[] sBoxInv; 
        byte[] expandedKey;
        byte[] aes_key;

        public AES()
        {
            aes_block1 = new byte[256];
            aes_block2 = new byte[256];
            aes_key = new byte[16];
        }

        private void mem_set(byte[] dest,byte value,int len)
        {
            int i;
            for(i = 0;i < len;i ++)
                dest[i] = value;
        }
        private void CalcPowLog(byte[] powTbl, byte[] logTbl)
        {
            byte  i = 0;
            byte  t = 1;

            do 
            {
                powTbl[i] = t;
                logTbl[t] = i;
                i++;
                t ^= (byte)((t << 1) ^ ((t & 0x80) != 0 ? AES.AES_BPOLY : 0));
            } while(t != 1);
            powTbl[255] = powTbl[0]; 
        }
        private void CalcSBox(byte[] sBox)
        {
            byte i, rot;
            byte temp;
            byte result;

            i = 0;
            do
            {
                if (i > 0)
                    temp = powTbl[255 - logTbl[i]];
                else
                    temp = 0;
                result = (byte)(temp ^ 0x63); // Start with adding a vector in GF(2).
                for (rot = 0; rot < 4; rot++)
                {
                    temp = (byte)((temp << 1) | (temp >> 7));
                    result ^= temp;
                }
                sBox[i] = result;
            } while (++i != 0);
        }
        void CalcSBoxInv(byte[] sBox, byte[] sBoxInv)
        {
            byte i = 0;
            byte j = 0;

            do
            {
                do
                {
                    if (sBox[j] == i)
                    {
                        sBoxInv[i] = j;
                        j = 255;
                    }
                } while (++j != 0);
            } while (++i != 0);
        }
        void CycleLeft(byte[] row)
        {
            byte temp = row[0];
            row[0] = row[1];
            row[1] = row[2];
            row[2] = row[3];
            row[3] = temp;
        }
        void InvMixColumn(byte[] column,int idx)
        {
            byte r0, r1, r2, r3;

            r0 = (byte)(column[idx+1] ^ column[idx+2] ^ column[idx+3]);
            r1 = (byte)(column[idx+0] ^ column[idx+2] ^ column[idx+3]);
            r2 = (byte)(column[idx+0] ^ column[idx+1] ^ column[idx+3]);
            r3 = (byte)(column[idx+0] ^ column[idx+1] ^ column[idx+2]);

            column[idx+0] = (byte)((column[idx+0] << 1) ^ ((column[idx+0] & 0x80) != 0? AES_BPOLY : 0));
            column[idx+1] = (byte)((column[idx+1] << 1) ^ ((column[idx+1] & 0x80) != 0 ? AES_BPOLY : 0));
            column[idx+2] = (byte)((column[idx+2] << 1) ^ ((column[idx+2] & 0x80) != 0 ? AES_BPOLY : 0));
            column[idx+3] = (byte)((column[idx+3] << 1) ^ ((column[idx+3] & 0x80) != 0 ? AES_BPOLY : 0));

            r0 ^= (byte)(column[idx+0] ^ column[idx+1]);
            r1 ^= (byte)(column[idx+1] ^ column[idx+2]);
            r2 ^= (byte)(column[idx+2] ^ column[idx+3]);
            r3 ^= (byte)(column[idx+0] ^ column[idx+3]);

            column[idx+0] = (byte)((column[idx+0] << 1) ^ ((column[idx+0] & 0x80) != 0 ? AES_BPOLY : 0));
            column[idx+1] = (byte)((column[idx+1] << 1) ^ ((column[idx+1] & 0x80) != 0 ? AES_BPOLY : 0));
            column[idx+2] = (byte)((column[idx+2] << 1) ^ ((column[idx+2] & 0x80) != 0 ? AES_BPOLY : 0));
            column[idx+3] = (byte)((column[idx+3] << 1) ^ ((column[idx+3] & 0x80) != 0 ? AES_BPOLY : 0));

            r0 ^= (byte)(column[idx+0] ^ column[idx+2]);
            r1 ^= (byte)(column[idx+1] ^ column[idx+3]);
            r2 ^= (byte)(column[idx+0] ^ column[idx+2]);
            r3 ^= (byte)(column[idx+1] ^ column[idx+3]);

            column[idx+0] = (byte)((column[idx+0] << 1) ^ ((column[idx+0] & 0x80) != 0 ? AES_BPOLY : 0));
            column[idx+1] = (byte)((column[idx+1] << 1) ^ ((column[idx+1] & 0x80) != 0 ? AES_BPOLY : 0));
            column[idx+2] = (byte)((column[idx+2] << 1) ^ ((column[idx+2] & 0x80) != 0 ? AES_BPOLY : 0));
            column[idx+3] = (byte)((column[idx+3] << 1) ^ ((column[idx+3] & 0x80) != 0 ? AES_BPOLY : 0));

            column[idx+0] ^= (byte)(column[idx+1] ^ column[idx+2] ^ column[idx+3]);
            r0 ^= column[idx+0];
            r1 ^= column[idx+0];
            r2 ^= column[idx+0];
            r3 ^= column[idx+0];

            column[idx+0] = r0;
            column[idx+1] = r1;
            column[idx+2] = r2;
            column[idx+3] = r3;
        }
        byte Multiply(byte num, byte factor)
        {
            byte mask = 1;
            byte result = 0;

            while (mask != 0)
            {
                if ((mask & factor) != 0)
                {
                    result ^= num;
                }
                mask <<= 1;
                num = (byte)((num << 1) ^ ((num & 0x80) != 0 ? AES_BPOLY : 0));
            }

            return result;
        }
        byte DotProduct(byte[] vector1, int vidx,byte[] vector2)
        {
            byte result = 0;
            int idx = 0;
            result ^= Multiply(vector1[vidx+idx], vector2[idx]);
            idx++;
            result ^= Multiply(vector1[vidx + idx], vector2[idx]);
            idx++;
            result ^= Multiply(vector1[vidx + idx], vector2[idx]);
            idx++;
            result ^= Multiply(vector1[vidx + idx], vector2[idx]);
            return result;
        }
        void MixColumn(byte[] column,int idx)
        {
            byte[] row = new byte[8]{0x02, 0x03, 0x01, 0x01, 0x02, 0x03, 0x01, 0x01};
            byte[] result = new byte[4];
     
            result[0] = DotProduct(row,0, column);
            result[1] = DotProduct(row,3,column);
            result[2] = DotProduct(row,2, column);
            result[3] = DotProduct(row,1, column);

            column[idx+0] = result[0];
            column[idx+1] = result[1];
            column[idx+2] = result[2];
            column[idx+3] = result[3];
        }
        void SubBytes(byte[] s, byte count)
        {
            int idx = 0;
            for (int i = 0; i < count;i ++ )
            {
                s[idx] = sBox[s[idx]];
                idx++;
            }
        }
        void InvSubBytesAndXOR(byte[] s, byte[] key,int idx, byte count)
        {
            //int idx = 0;
            for (int i = 0; i < count; i++)
            {
                s[i] = (byte)(aes_block2[s[i]] ^ key[idx+i]);
            }
        }
        void InvShiftRows(byte[] state)
        {
            byte temp;

            temp = state[1 + 3 * 4];
            state[1 + 3 * 4] = state[1 + 2 * 4];
            state[1 + 2 * 4] = state[1 + 1 * 4];
            state[1 + 1 * 4] = state[1 + 0 * 4];
            state[1 + 0 * 4] = temp;

            temp = state[2 + 0 * 4];
            state[2 + 0 * 4] = state[2 + 2 * 4];
            state[2 + 2 * 4] = temp;
            temp = state[2 + 1 * 4];
            state[2 + 1 * 4] = state[2 + 3 * 4];
            state[2 + 3 * 4] = temp;

            temp = state[3 + 0 * 4];
            state[3 + 0 * 4] = state[3 + 1 * 4];
            state[3 + 1 * 4] = state[3 + 2 * 4];
            state[3 + 2 * 4] = state[3 + 3 * 4];
            state[3 + 3 * 4] = temp;
        }
        void ShiftRows(byte[] state)
        {
            byte temp;
            temp = state[1 + 0 * 4];
            state[1 + 0 * 4] = state[1 + 1 * 4];
            state[1 + 1 * 4] = state[1 + 2 * 4];
            state[1 + 2 * 4] = state[1 + 3 * 4];
            state[1 + 3 * 4] = temp;

            temp = state[2 + 0 * 4];
            state[2 + 0 * 4] = state[2 + 2 * 4];
            state[2 + 2 * 4] = temp;
            temp = state[2 + 1 * 4];
            state[2 + 1 * 4] = state[2 + 3 * 4];
            state[2 + 3 * 4] = temp;

            temp = state[3 + 3 * 4];
            state[3 + 3 * 4] = state[3 + 2 * 4];
            state[3 + 2 * 4] = state[3 + 1 * 4];
            state[3 + 1 * 4] = state[3 + 0 * 4];
            state[3 + 0 * 4] = temp;
        }
        void InvMixColumns(byte[] state)
        {
            InvMixColumn(state , 0 * 4);
            InvMixColumn(state , 1 * 4);
            InvMixColumn(state , 2 * 4);
            InvMixColumn(state , 3 * 4);
        }
        void MixColumns(byte[] state)
        {
            MixColumn(state , 0 * 4);
            MixColumn(state , 1 * 4);
            MixColumn(state , 2 * 4);
            MixColumn(state , 3 * 4);
        }
        int XORBytes(byte[] s1, byte[] s2, int s2idx,byte count)
        {
            int i;
            for (i = 0; i < count; i++)
            {
                s1[i] ^= s2[s2idx+i]; // Add in GF(2), ie. XOR.
            }
            return i;
        }
        void CopyBytes(byte[] to, byte[] from, byte count)
        {
            for (int i = 0; i < count; i++)
            {
                to[i] = from[i];
            } 
        }
        void KeyExpansion(byte[] expandedKey)
        {
            int keyidx = 0;
            int idx=0;
            byte[]  temp = new byte[4];
            byte[]  Rcon = new byte[4]{ 0x01, 0x00, 0x00, 0x00 }; 
     
            byte[] key = aes_key;
            
            for (keyidx = 0; keyidx < AES_KEYLEN; keyidx++)
            {
                expandedKey[keyidx] = key[keyidx];
            }
            //i = AES_KEYLEN;

            keyidx -= 4;
            temp[0] = (expandedKey[keyidx++]);
            temp[1] = (expandedKey[keyidx++]);
            temp[2] = (expandedKey[keyidx++]);
            temp[3] = (expandedKey[keyidx++]);

            keyidx = AES_KEYLEN;
            while(keyidx < AES_BLKSIZE*(AES_ROUNDS+1)) 
            {
                if((keyidx % AES_KEYLEN) == 0) 
                {
                    CycleLeft(temp); 
                    SubBytes(temp, 4); 
                    idx += XORBytes(temp, Rcon,0, 4); 
                    Rcon[idx] = (byte)((Rcon[idx] << 1) ^ ((Rcon[idx] & 0x80) != 0? AES_BPOLY : 0));
                }

                if((AES_KEYLEN > 24)&&((keyidx % AES_KEYLEN) == AES_BLKSIZE))
                {
                    SubBytes(temp, 4); // Substitute each byte.
                }

                XORBytes(temp, expandedKey,keyidx  - AES_KEYLEN, 4);

                (expandedKey[keyidx++]) = temp[0];
                (expandedKey[keyidx++]) = temp[1];
                (expandedKey[keyidx++]) = temp[2];
                (expandedKey[keyidx++]) = temp[3];
            } 
        }
        void InvCipher(byte[] block, byte[] expandedKey)
        {
            int keyidx = 0;
            byte round = AES_ROUNDS - 1;
            //expandedKey += AES_BLKSIZE * AES_ROUNDS;
            keyidx += AES_BLKSIZE * AES_ROUNDS;

            XORBytes(block, expandedKey, keyidx,16);
            keyidx -= AES_BLKSIZE;
            for (int i = 0; i < round; i++)
            {
                InvShiftRows(block);
                InvSubBytesAndXOR(block, expandedKey, keyidx,16);
                keyidx -= AES_BLKSIZE;
                InvMixColumns(block);
            } 

            InvShiftRows(block);
            InvSubBytesAndXOR(block, expandedKey, keyidx,16);
        }
        //完成一个块(16字节，128bit)的加密
        void Cipher(byte[] block, byte[] expandedKey)
        {
            int keyidx = 0;
            byte round = AES_ROUNDS - 1;

            XORBytes(block, expandedKey, 0,16);
            keyidx += AES_BLKSIZE;

            for (int i = 0; i < round; i++)
            {
                SubBytes(block, 16);
                ShiftRows(block);
                MixColumns(block);
                XORBytes(block, expandedKey,keyidx, 16);
                keyidx += AES_BLKSIZE;
            }

            SubBytes(block, 16);
            ShiftRows(block);
            XORBytes(block, expandedKey,keyidx, 16);
        }
        void aesInit(byte[] tempbuf)
        {
            powTbl = aes_block1;
            logTbl = aes_block2;
            CalcPowLog(powTbl, logTbl);

            sBox = tempbuf;
            CalcSBox(sBox);

            expandedKey = aes_block1;  //至此block1用来存贮密码表
            KeyExpansion(expandedKey);

            sBoxInv = aes_block2; //block2至此开始只用来存贮SBOXINV
            CalcSBoxInv(sBox, sBoxInv);
        }
        void aesDecryptBlock(byte[] buffer, byte[] chainBlock, int blkidx)
        {
            Array.Copy(chainBlock, blkidx, buffer, 0, AES_BLKSIZE);
            InvCipher(buffer, expandedKey);
            Array.Copy(buffer, 0, chainBlock, blkidx, AES_BLKSIZE);
        }
        void aesEncryptBlock(byte[] buffer, byte[] chainBlock,int blkidx)
        {
            Array.Copy(chainBlock, blkidx, buffer, 0, AES_BLKSIZE);
            Cipher(buffer, expandedKey);
            Array.Copy( buffer, 0,chainBlock, blkidx, AES_BLKSIZE);
        }
        public void set_key(byte[] key, int lenth)
        {
            Array.Clear(this.aes_key,0,this.aes_key.Length);
            int len = lenth < this.aes_key.Length ? lenth : this.aes_key.Length;
            Array.Copy(key, this.aes_key, len);
        }

        new public int encrypt(byte[] data, int datalen)
        {
            int  i,idx;
            int blocks;
            int encrypt_len;
            byte[]  tempbuf = new byte[16];
            byte[]  sBoxbuf = new byte[256];

            
            aesInit(sBoxbuf);

            encrypt_len = (((datalen+15) >> 4) << 4);
            for(i = datalen;i < encrypt_len;i ++)
                data[i] = 0;
            blocks = ((datalen+15) >> 4); 
            for(i = 0;i < blocks;i ++)
            {
                idx = (i << 4);
                aesEncryptBlock(tempbuf,data,idx);
            }
            return encrypt_len;
        }

        new public int decrypt(byte[] data, int datalen)
        {
            int  i,idx;
            int blocks;
            byte[] tempbuf = new byte[16];
            byte[] sBoxbuf = new byte[256];
            if ((datalen & 0x0f) != 0)
                return -1;
            aesInit(sBoxbuf);

            blocks = (datalen>>4);
            for(i=0;i<blocks;i++)
            {
               
                idx = (i << 4);
                aesDecryptBlock(tempbuf, data, idx);
            }
            return datalen;

        }

    }
}
