using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace boot_img
{
    class Crypt
    {
        public virtual void set_key(byte[] key, w_int32_t lenth)
        {
 
        }

        public virtual w_int32_t encrypt(byte[] data, w_int32_t datalen)
        {
            return datalen;
        }
        public virtual w_int32_t decrypt(byte[] data, w_int32_t datalen)
        {
            return datalen;
        }

    }
}
