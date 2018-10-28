using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace boot_img
{
    class Crypt
    {
        public virtual void set_key(byte[] key, int lenth)
        {

 
        }

        public virtual int encrypt(byte[] data, int datalen)
        {
            return datalen;
        }
        public virtual int decrypt(byte[] data, int datalen)
        {
            return datalen;
        }

    }
}
