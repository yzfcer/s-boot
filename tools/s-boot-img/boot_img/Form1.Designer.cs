namespace boot_img
{
    partial class Form1
    {
        /// <summary>
        /// 必需的设计器变量。
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 清理所有正在使用的资源。
        /// </summary>
        /// <param name="disposing">如果应释放托管资源，为 true；否则为 false。</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows 窗体设计器生成的代码

        /// <summary>
        /// 设计器支持所需的方法 - 不要
        /// 使用代码编辑器修改此方法的内容。
        /// </summary>
        private void InitializeComponent()
        {
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.boardcomboBox = new System.Windows.Forms.ComboBox();
            this.cpucomboBox = new System.Windows.Forms.ComboBox();
            this.archcomboBox = new System.Windows.Forms.ComboBox();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.keytextBox = new System.Windows.Forms.TextBox();
            this.label7 = new System.Windows.Forms.Label();
            this.encryptcomboBox = new System.Windows.Forms.ComboBox();
            this.label20 = new System.Windows.Forms.Label();
            this.softvertextBox = new System.Windows.Forms.TextBox();
            this.label4 = new System.Windows.Forms.Label();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.viewbutton2 = new System.Windows.Forms.Button();
            this.viewbutton5 = new System.Windows.Forms.Button();
            this.viewbutton4 = new System.Windows.Forms.Button();
            this.viewbutton3 = new System.Windows.Forms.Button();
            this.viewbutton1 = new System.Windows.Forms.Button();
            this.label18 = new System.Windows.Forms.Label();
            this.pathtextBox5 = new System.Windows.Forms.TextBox();
            this.offsettextBox5 = new System.Windows.Forms.TextBox();
            this.label19 = new System.Windows.Forms.Label();
            this.label15 = new System.Windows.Forms.Label();
            this.pathtextBox4 = new System.Windows.Forms.TextBox();
            this.offsettextBox4 = new System.Windows.Forms.TextBox();
            this.label16 = new System.Windows.Forms.Label();
            this.label12 = new System.Windows.Forms.Label();
            this.pathtextBox3 = new System.Windows.Forms.TextBox();
            this.offsettextBox3 = new System.Windows.Forms.TextBox();
            this.label13 = new System.Windows.Forms.Label();
            this.label9 = new System.Windows.Forms.Label();
            this.pathtextBox2 = new System.Windows.Forms.TextBox();
            this.offsettextBox2 = new System.Windows.Forms.TextBox();
            this.label10 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.pathtextBox1 = new System.Windows.Forms.TextBox();
            this.offsettextBox1 = new System.Windows.Forms.TextBox();
            this.label5 = new System.Windows.Forms.Label();
            this.generatebutton = new System.Windows.Forms.Button();
            this.label8 = new System.Windows.Forms.Label();
            this.outviewbutton = new System.Windows.Forms.Button();
            this.outpathtextBox = new System.Windows.Forms.TextBox();
            this.loadcfgbutton = new System.Windows.Forms.Button();
            this.savecfgbutton = new System.Windows.Forms.Button();
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(25, 24);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(53, 12);
            this.label1.TabIndex = 0;
            this.label1.Text = "单板名称";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(349, 25);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(47, 12);
            this.label2.TabIndex = 1;
            this.label2.Text = "CPU型号";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(25, 53);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(47, 12);
            this.label3.TabIndex = 2;
            this.label3.Text = "CPU架构";
            // 
            // boardcomboBox
            // 
            this.boardcomboBox.FormattingEnabled = true;
            this.boardcomboBox.Location = new System.Drawing.Point(95, 21);
            this.boardcomboBox.Name = "boardcomboBox";
            this.boardcomboBox.Size = new System.Drawing.Size(210, 20);
            this.boardcomboBox.TabIndex = 3;
            // 
            // cpucomboBox
            // 
            this.cpucomboBox.FormattingEnabled = true;
            this.cpucomboBox.Location = new System.Drawing.Point(419, 22);
            this.cpucomboBox.Name = "cpucomboBox";
            this.cpucomboBox.Size = new System.Drawing.Size(210, 20);
            this.cpucomboBox.TabIndex = 4;
            // 
            // archcomboBox
            // 
            this.archcomboBox.FormattingEnabled = true;
            this.archcomboBox.Location = new System.Drawing.Point(95, 50);
            this.archcomboBox.Name = "archcomboBox";
            this.archcomboBox.Size = new System.Drawing.Size(210, 20);
            this.archcomboBox.TabIndex = 5;
            // 
            // groupBox1
            // 
            this.groupBox1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBox1.Controls.Add(this.keytextBox);
            this.groupBox1.Controls.Add(this.label7);
            this.groupBox1.Controls.Add(this.encryptcomboBox);
            this.groupBox1.Controls.Add(this.label20);
            this.groupBox1.Controls.Add(this.softvertextBox);
            this.groupBox1.Controls.Add(this.label4);
            this.groupBox1.Controls.Add(this.cpucomboBox);
            this.groupBox1.Controls.Add(this.archcomboBox);
            this.groupBox1.Controls.Add(this.label1);
            this.groupBox1.Controls.Add(this.label2);
            this.groupBox1.Controls.Add(this.boardcomboBox);
            this.groupBox1.Controls.Add(this.label3);
            this.groupBox1.Location = new System.Drawing.Point(12, 13);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(656, 112);
            this.groupBox1.TabIndex = 6;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "产品信息";
            // 
            // keytextBox
            // 
            this.keytextBox.ForeColor = System.Drawing.SystemColors.WindowText;
            this.keytextBox.Location = new System.Drawing.Point(419, 81);
            this.keytextBox.Name = "keytextBox";
            this.keytextBox.Size = new System.Drawing.Size(210, 21);
            this.keytextBox.TabIndex = 11;
            this.keytextBox.UseSystemPasswordChar = true;
            this.keytextBox.MouseClick += new System.Windows.Forms.MouseEventHandler(this.keytextBox_MouseClick);
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(349, 84);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(29, 12);
            this.label7.TabIndex = 10;
            this.label7.Text = "密钥";
            // 
            // encryptcomboBox
            // 
            this.encryptcomboBox.FormattingEnabled = true;
            this.encryptcomboBox.Items.AddRange(new object[] {
            "none",
            "AES",
            "DES",
            "RC4"});
            this.encryptcomboBox.Location = new System.Drawing.Point(96, 77);
            this.encryptcomboBox.Name = "encryptcomboBox";
            this.encryptcomboBox.Size = new System.Drawing.Size(210, 20);
            this.encryptcomboBox.TabIndex = 9;
            // 
            // label20
            // 
            this.label20.AutoSize = true;
            this.label20.Location = new System.Drawing.Point(26, 80);
            this.label20.Name = "label20";
            this.label20.Size = new System.Drawing.Size(53, 12);
            this.label20.TabIndex = 8;
            this.label20.Text = "加密方式";
            // 
            // softvertextBox
            // 
            this.softvertextBox.ForeColor = System.Drawing.SystemColors.WindowText;
            this.softvertextBox.Location = new System.Drawing.Point(419, 51);
            this.softvertextBox.Name = "softvertextBox";
            this.softvertextBox.Size = new System.Drawing.Size(210, 21);
            this.softvertextBox.TabIndex = 7;
            this.softvertextBox.Text = "1.0.0";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(349, 54);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(53, 12);
            this.label4.TabIndex = 6;
            this.label4.Text = "软件版本";
            // 
            // groupBox2
            // 
            this.groupBox2.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBox2.Controls.Add(this.outviewbutton);
            this.groupBox2.Controls.Add(this.outpathtextBox);
            this.groupBox2.Controls.Add(this.label8);
            this.groupBox2.Controls.Add(this.viewbutton2);
            this.groupBox2.Controls.Add(this.viewbutton5);
            this.groupBox2.Controls.Add(this.viewbutton4);
            this.groupBox2.Controls.Add(this.viewbutton3);
            this.groupBox2.Controls.Add(this.viewbutton1);
            this.groupBox2.Controls.Add(this.label18);
            this.groupBox2.Controls.Add(this.pathtextBox5);
            this.groupBox2.Controls.Add(this.offsettextBox5);
            this.groupBox2.Controls.Add(this.label19);
            this.groupBox2.Controls.Add(this.label15);
            this.groupBox2.Controls.Add(this.pathtextBox4);
            this.groupBox2.Controls.Add(this.offsettextBox4);
            this.groupBox2.Controls.Add(this.label16);
            this.groupBox2.Controls.Add(this.label12);
            this.groupBox2.Controls.Add(this.pathtextBox3);
            this.groupBox2.Controls.Add(this.offsettextBox3);
            this.groupBox2.Controls.Add(this.label13);
            this.groupBox2.Controls.Add(this.label9);
            this.groupBox2.Controls.Add(this.pathtextBox2);
            this.groupBox2.Controls.Add(this.offsettextBox2);
            this.groupBox2.Controls.Add(this.label10);
            this.groupBox2.Controls.Add(this.label6);
            this.groupBox2.Controls.Add(this.pathtextBox1);
            this.groupBox2.Controls.Add(this.offsettextBox1);
            this.groupBox2.Controls.Add(this.label5);
            this.groupBox2.Location = new System.Drawing.Point(12, 131);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(656, 285);
            this.groupBox2.TabIndex = 7;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "文件列表";
            // 
            // viewbutton2
            // 
            this.viewbutton2.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.viewbutton2.Location = new System.Drawing.Point(556, 76);
            this.viewbutton2.Name = "viewbutton2";
            this.viewbutton2.Size = new System.Drawing.Size(75, 23);
            this.viewbutton2.TabIndex = 29;
            this.viewbutton2.Text = "浏览";
            this.viewbutton2.UseVisualStyleBackColor = true;
            this.viewbutton2.Click += new System.EventHandler(this.viewbutton2_Click);
            // 
            // viewbutton5
            // 
            this.viewbutton5.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.viewbutton5.Location = new System.Drawing.Point(556, 193);
            this.viewbutton5.Name = "viewbutton5";
            this.viewbutton5.Size = new System.Drawing.Size(75, 23);
            this.viewbutton5.TabIndex = 28;
            this.viewbutton5.Text = "浏览";
            this.viewbutton5.UseVisualStyleBackColor = true;
            this.viewbutton5.Click += new System.EventHandler(this.viewbutton5_Click);
            // 
            // viewbutton4
            // 
            this.viewbutton4.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.viewbutton4.Location = new System.Drawing.Point(556, 154);
            this.viewbutton4.Name = "viewbutton4";
            this.viewbutton4.Size = new System.Drawing.Size(75, 23);
            this.viewbutton4.TabIndex = 27;
            this.viewbutton4.Text = "浏览";
            this.viewbutton4.UseVisualStyleBackColor = true;
            this.viewbutton4.Click += new System.EventHandler(this.viewbutton4_Click);
            // 
            // viewbutton3
            // 
            this.viewbutton3.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.viewbutton3.Location = new System.Drawing.Point(556, 115);
            this.viewbutton3.Name = "viewbutton3";
            this.viewbutton3.Size = new System.Drawing.Size(75, 23);
            this.viewbutton3.TabIndex = 26;
            this.viewbutton3.Text = "浏览";
            this.viewbutton3.UseVisualStyleBackColor = true;
            this.viewbutton3.Click += new System.EventHandler(this.viewbutton3_Click);
            // 
            // viewbutton1
            // 
            this.viewbutton1.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.viewbutton1.Location = new System.Drawing.Point(556, 37);
            this.viewbutton1.Name = "viewbutton1";
            this.viewbutton1.Size = new System.Drawing.Size(75, 23);
            this.viewbutton1.TabIndex = 25;
            this.viewbutton1.Text = "浏览";
            this.viewbutton1.UseVisualStyleBackColor = true;
            this.viewbutton1.Click += new System.EventHandler(this.viewbutton1_Click);
            // 
            // label18
            // 
            this.label18.AutoSize = true;
            this.label18.Location = new System.Drawing.Point(196, 199);
            this.label18.Name = "label18";
            this.label18.Size = new System.Drawing.Size(29, 12);
            this.label18.TabIndex = 24;
            this.label18.Text = "路径";
            // 
            // pathtextBox5
            // 
            this.pathtextBox5.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.pathtextBox5.Location = new System.Drawing.Point(231, 195);
            this.pathtextBox5.Name = "pathtextBox5";
            this.pathtextBox5.Size = new System.Drawing.Size(319, 21);
            this.pathtextBox5.TabIndex = 23;
            // 
            // offsettextBox5
            // 
            this.offsettextBox5.Location = new System.Drawing.Point(96, 195);
            this.offsettextBox5.Name = "offsettextBox5";
            this.offsettextBox5.Size = new System.Drawing.Size(94, 21);
            this.offsettextBox5.TabIndex = 22;
            // 
            // label19
            // 
            this.label19.AutoSize = true;
            this.label19.Location = new System.Drawing.Point(7, 199);
            this.label19.Name = "label19";
            this.label19.Size = new System.Drawing.Size(83, 12);
            this.label19.TabIndex = 21;
            this.label19.Text = "文件1偏移位置";
            // 
            // label15
            // 
            this.label15.AutoSize = true;
            this.label15.Location = new System.Drawing.Point(196, 159);
            this.label15.Name = "label15";
            this.label15.Size = new System.Drawing.Size(29, 12);
            this.label15.TabIndex = 19;
            this.label15.Text = "路径";
            // 
            // pathtextBox4
            // 
            this.pathtextBox4.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.pathtextBox4.Location = new System.Drawing.Point(231, 155);
            this.pathtextBox4.Name = "pathtextBox4";
            this.pathtextBox4.Size = new System.Drawing.Size(319, 21);
            this.pathtextBox4.TabIndex = 18;
            // 
            // offsettextBox4
            // 
            this.offsettextBox4.Location = new System.Drawing.Point(96, 155);
            this.offsettextBox4.Name = "offsettextBox4";
            this.offsettextBox4.Size = new System.Drawing.Size(94, 21);
            this.offsettextBox4.TabIndex = 17;
            // 
            // label16
            // 
            this.label16.AutoSize = true;
            this.label16.Location = new System.Drawing.Point(7, 159);
            this.label16.Name = "label16";
            this.label16.Size = new System.Drawing.Size(83, 12);
            this.label16.TabIndex = 16;
            this.label16.Text = "文件1偏移位置";
            // 
            // label12
            // 
            this.label12.AutoSize = true;
            this.label12.Location = new System.Drawing.Point(196, 119);
            this.label12.Name = "label12";
            this.label12.Size = new System.Drawing.Size(29, 12);
            this.label12.TabIndex = 14;
            this.label12.Text = "路径";
            // 
            // pathtextBox3
            // 
            this.pathtextBox3.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.pathtextBox3.Location = new System.Drawing.Point(231, 115);
            this.pathtextBox3.Name = "pathtextBox3";
            this.pathtextBox3.Size = new System.Drawing.Size(319, 21);
            this.pathtextBox3.TabIndex = 13;
            // 
            // offsettextBox3
            // 
            this.offsettextBox3.Location = new System.Drawing.Point(96, 115);
            this.offsettextBox3.Name = "offsettextBox3";
            this.offsettextBox3.Size = new System.Drawing.Size(94, 21);
            this.offsettextBox3.TabIndex = 12;
            // 
            // label13
            // 
            this.label13.AutoSize = true;
            this.label13.Location = new System.Drawing.Point(7, 119);
            this.label13.Name = "label13";
            this.label13.Size = new System.Drawing.Size(83, 12);
            this.label13.TabIndex = 11;
            this.label13.Text = "文件1偏移位置";
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(196, 79);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(29, 12);
            this.label9.TabIndex = 9;
            this.label9.Text = "路径";
            // 
            // pathtextBox2
            // 
            this.pathtextBox2.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.pathtextBox2.Location = new System.Drawing.Point(231, 75);
            this.pathtextBox2.Name = "pathtextBox2";
            this.pathtextBox2.Size = new System.Drawing.Size(319, 21);
            this.pathtextBox2.TabIndex = 8;
            // 
            // offsettextBox2
            // 
            this.offsettextBox2.Location = new System.Drawing.Point(96, 75);
            this.offsettextBox2.Name = "offsettextBox2";
            this.offsettextBox2.Size = new System.Drawing.Size(94, 21);
            this.offsettextBox2.TabIndex = 7;
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Location = new System.Drawing.Point(7, 79);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(83, 12);
            this.label10.TabIndex = 6;
            this.label10.Text = "文件1偏移位置";
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(196, 39);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(29, 12);
            this.label6.TabIndex = 4;
            this.label6.Text = "路径";
            // 
            // pathtextBox1
            // 
            this.pathtextBox1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.pathtextBox1.Location = new System.Drawing.Point(231, 35);
            this.pathtextBox1.Name = "pathtextBox1";
            this.pathtextBox1.Size = new System.Drawing.Size(319, 21);
            this.pathtextBox1.TabIndex = 3;
            // 
            // offsettextBox1
            // 
            this.offsettextBox1.Location = new System.Drawing.Point(96, 35);
            this.offsettextBox1.Name = "offsettextBox1";
            this.offsettextBox1.Size = new System.Drawing.Size(94, 21);
            this.offsettextBox1.TabIndex = 2;
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(7, 39);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(83, 12);
            this.label5.TabIndex = 0;
            this.label5.Text = "文件1偏移位置";
            // 
            // generatebutton
            // 
            this.generatebutton.Location = new System.Drawing.Point(454, 439);
            this.generatebutton.Name = "generatebutton";
            this.generatebutton.Size = new System.Drawing.Size(106, 23);
            this.generatebutton.TabIndex = 8;
            this.generatebutton.Text = "生成映像文件";
            this.generatebutton.UseVisualStyleBackColor = true;
            this.generatebutton.Click += new System.EventHandler(this.generatebutton_Click);
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(6, 236);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(77, 12);
            this.label8.TabIndex = 30;
            this.label8.Text = "文件输出路径";
            // 
            // outviewbutton
            // 
            this.outviewbutton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.outviewbutton.Location = new System.Drawing.Point(554, 232);
            this.outviewbutton.Name = "outviewbutton";
            this.outviewbutton.Size = new System.Drawing.Size(75, 23);
            this.outviewbutton.TabIndex = 32;
            this.outviewbutton.Text = "浏览";
            this.outviewbutton.UseVisualStyleBackColor = true;
            this.outviewbutton.Click += new System.EventHandler(this.outviewbutton_Click);
            // 
            // outpathtextBox
            // 
            this.outpathtextBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.outpathtextBox.Location = new System.Drawing.Point(96, 235);
            this.outpathtextBox.Name = "outpathtextBox";
            this.outpathtextBox.Size = new System.Drawing.Size(452, 21);
            this.outpathtextBox.TabIndex = 31;
            // 
            // loadcfgbutton
            // 
            this.loadcfgbutton.Location = new System.Drawing.Point(108, 439);
            this.loadcfgbutton.Name = "loadcfgbutton";
            this.loadcfgbutton.Size = new System.Drawing.Size(104, 23);
            this.loadcfgbutton.TabIndex = 9;
            this.loadcfgbutton.Text = "导入配置文件";
            this.loadcfgbutton.UseVisualStyleBackColor = true;
            this.loadcfgbutton.Click += new System.EventHandler(this.loadcfgbutton_Click);
            // 
            // savecfgbutton
            // 
            this.savecfgbutton.Location = new System.Drawing.Point(281, 439);
            this.savecfgbutton.Name = "savecfgbutton";
            this.savecfgbutton.Size = new System.Drawing.Size(104, 23);
            this.savecfgbutton.TabIndex = 10;
            this.savecfgbutton.Text = "保存配置文件";
            this.savecfgbutton.UseVisualStyleBackColor = true;
            this.savecfgbutton.Click += new System.EventHandler(this.savecfgbutton_Click);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(680, 484);
            this.Controls.Add(this.savecfgbutton);
            this.Controls.Add(this.loadcfgbutton);
            this.Controls.Add(this.generatebutton);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.groupBox1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.Fixed3D;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "Form1";
            this.Text = "s-boot映像文件生成工具";
            this.Load += new System.EventHandler(this.Form1_Load);
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.ComboBox boardcomboBox;
        private System.Windows.Forms.ComboBox cpucomboBox;
        private System.Windows.Forms.ComboBox archcomboBox;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.TextBox softvertextBox;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.Label label18;
        private System.Windows.Forms.TextBox pathtextBox5;
        private System.Windows.Forms.TextBox offsettextBox5;
        private System.Windows.Forms.Label label19;
        private System.Windows.Forms.Label label15;
        private System.Windows.Forms.TextBox pathtextBox4;
        private System.Windows.Forms.TextBox offsettextBox4;
        private System.Windows.Forms.Label label16;
        private System.Windows.Forms.Label label12;
        private System.Windows.Forms.TextBox pathtextBox3;
        private System.Windows.Forms.TextBox offsettextBox3;
        private System.Windows.Forms.Label label13;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.TextBox pathtextBox2;
        private System.Windows.Forms.TextBox offsettextBox2;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.TextBox pathtextBox1;
        private System.Windows.Forms.TextBox offsettextBox1;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Button generatebutton;
        private System.Windows.Forms.ComboBox encryptcomboBox;
        private System.Windows.Forms.Label label20;
        private System.Windows.Forms.Button viewbutton2;
        private System.Windows.Forms.Button viewbutton5;
        private System.Windows.Forms.Button viewbutton4;
        private System.Windows.Forms.Button viewbutton3;
        private System.Windows.Forms.Button viewbutton1;
        private System.Windows.Forms.TextBox keytextBox;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Button outviewbutton;
        private System.Windows.Forms.TextBox outpathtextBox;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.Button loadcfgbutton;
        private System.Windows.Forms.Button savecfgbutton;
    }
}

