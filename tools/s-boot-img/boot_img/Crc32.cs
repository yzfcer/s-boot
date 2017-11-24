using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace boot_img
{
    class Crc32
    {
        //static protected ulong[] Crc32Table;
        static UInt32  POLYNOMIAL = 0xEDB88320;
        static bool have_table = false;
        static UInt32[] table = new UInt32[256];

        //生成CRC32码表
        static void make_table()
        {
            UInt32 i, j;
            have_table = true;
            for (i = 0; i < 256; i++)
                for (j = 0, table[i] = i; j < 8; j++)
                    table[i] = (table[i] >> 1) ^ (((table[i] & 1) != 0)? POLYNOMIAL : 0);
        }

        //获取字符串的CRC32校验值
        public static UInt32 calc_crc32(byte[] buff, w_int32_t offset,w_int32_t len, UInt32 crc)
        {
            w_int32_t i;
            if (!have_table)
                make_table();
            //boot_debug("calculate CRC base 0x%x,len %d",buff,len);
            crc = ~crc;
            for (i = offset; i < offset + len; i++)
                crc = (crc >> 8) ^ table[(crc ^ buff[i]) & 0xff];
            return ~crc;
        }

        
        
    }
}
