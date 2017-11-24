using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace boot_img
{
    class fileinfo
    {
        string path;
        w_int32_t offset;
        w_int32_t filelen;
        byte[] data;
        public string Path
        {
            get
            {
                return path;
            }
            set 
            {
                this.path = value;
            }
        }
        public w_int32_t  Offset
        {
            get
            {
                return offset;
            }
            set
            {
                this.offset = value;
            }
        }

        public w_int32_t Filelen
        {
            get
            {
                return filelen;
            }
            set
            {
                this.filelen = value;
            }
        }

        public byte[] Data
        {
            get
            {
                return data;
            }
        }

        public w_int32_t read_file()
        {
            try
            {
                System.IO.FileStream fs = new System.IO.FileStream(path, System.IO.FileMode.Open);
                data = new byte[fs.Length];
                filelen = fs.Read(data, 0, (w_int32_t)fs.Length);
                fs.Close();

            }
            catch (Exception ex)
            {
                filelen = -1; 
            }
            return filelen;
        }

    }
}
