using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace boot_img
{
    public partial class Form1 : Form
    {
        List<TextBox> offsettb;
        List<TextBox> pathtb;
        byte[] filehead;
        byte[] imgdata;
        Int32 imglen;//img文件长度
        UInt32 filecrc;
        List<fileinfo> listfi;

        public Form1()
        {
            InitializeComponent();
        }

        void load_config(string filename)
        {
            System.IO.StreamReader sr = new System.IO.StreamReader(filename);
            while (true)
            {
                string str = sr.ReadLine();
                if (str == null)
                    break;
                string[] strarr = str.Split(new char[] { '=', ',' });
                if (strarr.Length >= 2)
                {
                    if (strarr[0].CompareTo("board") == 0)
                    {
                        boardcomboBox.Items.Add(strarr[1]);
                    }
                    else if (strarr[0].CompareTo("arch") == 0)
                    {
                        archcomboBox.Items.Add(strarr[1]);
                    }
                    else if (strarr[0].CompareTo("cpu") == 0)
                    {
                        cpucomboBox.Items.Add(strarr[1]);
                    }
                    else if (strarr[0].CompareTo("encryptkey") == 0)
                    {
                        keytextBox.Text = strarr[1];
                    }
                    else if (strarr[0].CompareTo("encrypttype") == 0)
                    {
                        for (int i = 0; i < encryptcomboBox.Items.Count; i++)
                        {
                            if (strarr[1].ToUpper().CompareTo(encryptcomboBox.Items[i].ToString().ToUpper()) == 0)
                            {
                                encryptcomboBox.SelectedIndex = i;
                            }
                        }
                    }
                    else if (strarr[0].CompareTo("outpath") == 0)
                    {
                        outpathtextBox.Text = strarr[1];
                    }
                    else
                    {
                        for (int i = 1; i < 6; i++)
                        {
                            if (strarr[0].CompareTo("path" + i.ToString()) == 0)
                            {
                                pathtb[i - 1].Text = strarr[1];
                            }
                            else if (strarr[0].CompareTo("position" + i.ToString()) == 0)
                            {
                                offsettb[i - 1].Text = strarr[1];
                            }
                        }
                    }
                }
            }
            sr.Close();
        }

        void save_config(string filename)
        {
            string strline = "";
            System.IO.StreamWriter sw = new System.IO.StreamWriter(filename);
            sw.WriteLine("#加密方式可选填：none,AES,DES和RC4中的一种，其中选择none表示不加密");
            sw.WriteLine("encrypttype="+encryptcomboBox.Text);
            sw.WriteLine("#加密密钥");
            sw.WriteLine("encryptkey=" + keytextBox.Text);
            sw.WriteLine();
            sw.WriteLine(strline);
            for (int i = 1; i < 6; i++)
            {
                sw.WriteLine("position" + i.ToString() + "=" + offsettb[i - 1].Text);
                sw.WriteLine("path" + i.ToString() + "=" + pathtb[i - 1].Text);
                sw.WriteLine();
            }
            sw.WriteLine("outpath="+outpathtextBox.Text);
            sw.WriteLine();
            sw.WriteLine("#在下面添加设备板卡信息：");
            for (int i = 0; i < boardcomboBox.Items.Count; i++)
            {
                sw.WriteLine("board="+boardcomboBox.Items[i].ToString());
            }
            sw.WriteLine();
            sw.WriteLine("#在下面添加CPU架构信息：");
            for (int i = 0; i < archcomboBox.Items.Count; i++)
            {
                sw.WriteLine("arch=" + archcomboBox.Items[i].ToString());
            }
            sw.WriteLine();
            sw.WriteLine("#在下面添加CPU型号信息：");
            for (int i = 0; i < cpucomboBox.Items.Count; i++)
            {
                sw.WriteLine("cpu=" + cpucomboBox.Items[i].ToString());
            }
            sw.Close();
        }

        void init_hardware_info()
        {
            load_config("config.txt");
            offsettextBox1.Text = "0x200";
            if (boardcomboBox.Items.Count > 0)
                boardcomboBox.SelectedIndex = 0;
            if (archcomboBox.Items.Count > 0)
                archcomboBox.SelectedIndex = 0;
            if (cpucomboBox.Items.Count > 0)
                cpucomboBox.SelectedIndex = 0;
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            offsettb = new List<TextBox>();
            offsettb.Add(offsettextBox1);
            offsettb.Add(offsettextBox2);
            offsettb.Add(offsettextBox3);
            offsettb.Add(offsettextBox4);
            offsettb.Add(offsettextBox5);
            pathtb = new List<TextBox>();
            pathtb.Add(pathtextBox1);
            pathtb.Add(pathtextBox2);
            pathtb.Add(pathtextBox3);
            pathtb.Add(pathtextBox4);
            pathtb.Add(pathtextBox5);
            filehead = new byte[512];
            info.ForeColor = Color.Red;
            info.Text = "";
            listfi = new List<fileinfo>();
            init_hardware_info();
            //encryptcomboBox.SelectedIndex = 0;
        }

        void set_path(int idx)
        {
            OpenFileDialog ofd = new OpenFileDialog();
            ofd.Title = "选择文件";
            if (ofd.ShowDialog() == System.Windows.Forms.DialogResult.OK)
            {
                pathtb[idx].Text = ofd.FileName;
            }
        }

        

        private void viewbutton1_Click(object sender, EventArgs e)
        {
            set_path(0);
        }

        private void viewbutton2_Click(object sender, EventArgs e)
        {
            set_path(1);
        }

        private void viewbutton3_Click(object sender, EventArgs e)
        {
            set_path(2);
        }

        private void viewbutton4_Click(object sender, EventArgs e)
        {
            set_path(3);
        }

        private void viewbutton5_Click(object sender, EventArgs e)
        {
            set_path(4);
        }

        private void outviewbutton_Click(object sender, EventArgs e)
        {
            string filename = "imgfile." + encryptcomboBox.Text + ".img";
            SaveFileDialog sfd = new SaveFileDialog();
            sfd.FileName = filename;
            sfd.Title = "保存位置";
            if (sfd.ShowDialog() == System.Windows.Forms.DialogResult.OK)
            {
                outpathtextBox.Text = sfd.FileName;
            }
        }
        private void loadcfgbutton_Click(object sender, EventArgs e)
        {
            OpenFileDialog ofd = new OpenFileDialog();
            if (ofd.ShowDialog() == System.Windows.Forms.DialogResult.OK)
            {
                load_config(ofd.FileName);
            }
        }

        private void savecfgbutton_Click(object sender, EventArgs e)
        {

            SaveFileDialog sfd = new SaveFileDialog();
            sfd.FileName = "config.txt";
            sfd.Title = "保存配置文件";
            if (sfd.ShowDialog() == System.Windows.Forms.DialogResult.OK)
            {
                save_config(sfd.FileName);
            }
        }

        private void keytextBox_MouseClick(object sender, MouseEventArgs e)
        {
            if (keytextBox.UseSystemPasswordChar)
                keytextBox.UseSystemPasswordChar = false;
            else
                keytextBox.UseSystemPasswordChar = true;
        }


        /*****************************************************************************************/
        void set_error(string errstr)
        {
            info.Text = errstr;
        }

        bool check_params()
        {
            if (boardcomboBox.SelectedIndex < 0)
            {
                set_error("请选择单板名称");
                return false;
            }
            if (archcomboBox.SelectedIndex < 0)
            {
                set_error("请选择CPU架构");
                return false;
            }
            if (cpucomboBox.SelectedIndex < 0)
            {
                set_error("请选择CPU型号");
                return false;
            }
            if (encryptcomboBox.SelectedIndex < 0)
            {
                set_error("请选择加密方式");
                return false;
            }
            if (softvertextBox.Text == "")
            {
                set_error("请输入软件版本");
                return false;
            }
            if ((encryptcomboBox.SelectedIndex != 0) && (keytextBox.Text == ""))
            {
                set_error("请输入加密密钥");
                return false;
            }
            if (pathtextBox1.Text == "")
            {
                set_error("第一个文件不能为空");
                return false;
            }
            if (outpathtextBox.Text  == "")
            {
                set_error("请选择输出路径");
                return false;
            }
           return true;
        }

        int fill_bytearr(byte[] arr, int idx, string str)
        {
            byte[] tag = System.Text.Encoding.Default.GetBytes(str);
            tag.CopyTo(filehead, idx);
            return tag.Length;
        }

        int fill_bytearr(byte[] arr, int idx, UInt32 value)
        {
            byte[] tag = System.BitConverter.GetBytes(value);
            tag.CopyTo(filehead, idx);
            return tag.Length;
        }

        void fill_file_head()
        {
            int idx = 0;
            Array.Clear(filehead, 0, filehead.Length);
            fill_bytearr(filehead, idx, "S_BOOT10");
            idx += 8;
            idx += fill_bytearr(filehead, idx, (UInt32)imglen);
            idx += fill_bytearr(filehead, idx, 512);
            idx += fill_bytearr(filehead, idx, 1);
            fill_bytearr(filehead, idx, softvertextBox.Text);
            idx += 16;
            idx += fill_bytearr(filehead, idx, filecrc);
            int encrypttype = encryptcomboBox.SelectedIndex;
            idx += fill_bytearr(filehead, idx, (UInt32)encrypttype);
            idx += 20;
            idx += fill_bytearr(filehead, idx, outpathtextBox.Text);
            idx += 64;
            fill_bytearr(filehead, idx, boardcomboBox.Text);
            idx += 32;
            fill_bytearr(filehead, idx, archcomboBox.Text);
            idx += 32;
            fill_bytearr(filehead, idx, cpucomboBox.Text);
            idx += 32;
            UInt32 crc = (UInt32)Crc32.calc_crc32(filehead, 0, 512 - 4);
            fill_bytearr(filehead, 512-4, crc);
        }

        private void generatebutton_Click(object sender, EventArgs e)
        {
            if (!check_params())
                return;
            set_error("");
            listfi.Clear();
            for (int i = 0; i < 5; i++)
            {
                fileinfo fi = new fileinfo();
                fi.Path = pathtb[i].Text;
                fi.Offset = Convert.ToInt32(offsettb[i].Text,16);
                fi.read_file();
                if(fi.Filelen > 0)
                    listfi.Add(fi);
            }
            int cnt = listfi.Count - 1;
            for (int i = 0; i < cnt; i++)
            {
                if (listfi[i].Offset + listfi[i].Filelen >= listfi[i + 1].Offset)
                {
                    set_error("文件"+listfi[i].Path+"的长度超出了填充范围！");
                    return;
                }
            }
            imglen = listfi[cnt].Offset + listfi[cnt].Filelen;
            imgdata = new byte[imglen];
            for (int i = 0; i < listfi.Count; i++)
            {
                Array.Copy(listfi[i].Data, 0, imgdata, listfi[i].Offset,listfi[i].Filelen);
            }
            fill_file_head();
            Array.Copy(filehead, imgdata, 512);

            //这里需添加加密功能


            filecrc = (UInt32)Crc32.calc_crc32(imgdata, 512, imglen - 512);
            System.IO.FileStream fs = new System.IO.FileStream(outpathtextBox.Text, System.IO.FileMode.Create);
            fs.Write(imgdata, 0, imglen);
            fs.Close();
        }
    }
}
