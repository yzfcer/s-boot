using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace boot_img
{
    class Crc32
    {
        static protected ulong[] Crc32Table;
        //生成CRC32码表
        static public void make_table()
        {
            ulong Crc;
            Crc32Table = new ulong[256];
            int i, j;
            for (i = 0; i < 256; i++)
            {
                Crc = (ulong)i;
                for (j = 8; j > 0; j--)
                {
                    if ((Crc & 1) == 1)
                        Crc = (Crc >> 1) ^ 0xEDB88320;
                    else
                        Crc >>= 1;
                }
                Crc32Table[i] = Crc;
            }
        }

        //获取字符串的CRC32校验值
        static public ulong calc_crc32(byte[] buffer,int offset,int len)
        {
            //生成码表
            make_table();
            ulong value = 0xffffffff;
            for (int i = 0; i < len; i++)
            {
                value = (value >> 8) ^ Crc32Table[(value & 0xFF) ^ buffer[i]];
            }
            return value ^ 0xffffffff;
        }
    }
}
