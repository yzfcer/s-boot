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
            this.hardvertextBox = new System.Windows.Forms.TextBox();
            this.label7 = new System.Windows.Forms.Label();
            this.encryptcomboBox = new System.Windows.Forms.ComboBox();
            this.label20 = new System.Windows.Forms.Label();
            this.softvertextBox = new System.Windows.Forms.TextBox();
            this.label4 = new System.Windows.Forms.Label();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.removebutton = new System.Windows.Forms.Button();
            this.insertbutton = new System.Windows.Forms.Button();
            this.dataGridView1 = new System.Windows.Forms.DataGridView();
            this.offset = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.path = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.outviewbutton = new System.Windows.Forms.Button();
            this.outpathtextBox = new System.Windows.Forms.TextBox();
            this.label8 = new System.Windows.Forms.Label();
            this.viewbutton = new System.Windows.Forms.Button();
            this.label18 = new System.Windows.Forms.Label();
            this.srcpathtextBox = new System.Windows.Forms.TextBox();
            this.offsettextBox = new System.Windows.Forms.TextBox();
            this.label19 = new System.Windows.Forms.Label();
            this.generatebutton = new System.Windows.Forms.Button();
            this.loadcfgbutton = new System.Windows.Forms.Button();
            this.savecfgbutton = new System.Windows.Forms.Button();
            this.statusStrip = new System.Windows.Forms.StatusStrip();
            this.info = new System.Windows.Forms.ToolStripStatusLabel();
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.dataGridView1)).BeginInit();
            this.statusStrip.SuspendLayout();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(26, 55);
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
            this.label3.Location = new System.Drawing.Point(29, 26);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(47, 12);
            this.label3.TabIndex = 2;
            this.label3.Text = "CPU架构";
            // 
            // boardcomboBox
            // 
            this.boardcomboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.boardcomboBox.FormattingEnabled = true;
            this.boardcomboBox.Location = new System.Drawing.Point(96, 51);
            this.boardcomboBox.Name = "boardcomboBox";
            this.boardcomboBox.Size = new System.Drawing.Size(210, 20);
            this.boardcomboBox.TabIndex = 3;
            // 
            // cpucomboBox
            // 
            this.cpucomboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cpucomboBox.FormattingEnabled = true;
            this.cpucomboBox.Location = new System.Drawing.Point(419, 22);
            this.cpucomboBox.Name = "cpucomboBox";
            this.cpucomboBox.Size = new System.Drawing.Size(210, 20);
            this.cpucomboBox.TabIndex = 4;
            // 
            // archcomboBox
            // 
            this.archcomboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.archcomboBox.FormattingEnabled = true;
            this.archcomboBox.Location = new System.Drawing.Point(96, 22);
            this.archcomboBox.Name = "archcomboBox";
            this.archcomboBox.Size = new System.Drawing.Size(210, 20);
            this.archcomboBox.TabIndex = 5;
            // 
            // groupBox1
            // 
            this.groupBox1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBox1.Controls.Add(this.hardvertextBox);
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
            // hardvertextBox
            // 
            this.hardvertextBox.ForeColor = System.Drawing.SystemColors.WindowText;
            this.hardvertextBox.Location = new System.Drawing.Point(419, 81);
            this.hardvertextBox.Name = "hardvertextBox";
            this.hardvertextBox.Size = new System.Drawing.Size(210, 21);
            this.hardvertextBox.TabIndex = 11;
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(349, 84);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(53, 12);
            this.label7.TabIndex = 10;
            this.label7.Text = "硬件版本";
            // 
            // encryptcomboBox
            // 
            this.encryptcomboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
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
            this.groupBox2.Controls.Add(this.removebutton);
            this.groupBox2.Controls.Add(this.insertbutton);
            this.groupBox2.Controls.Add(this.dataGridView1);
            this.groupBox2.Controls.Add(this.outviewbutton);
            this.groupBox2.Controls.Add(this.outpathtextBox);
            this.groupBox2.Controls.Add(this.label8);
            this.groupBox2.Controls.Add(this.viewbutton);
            this.groupBox2.Controls.Add(this.label18);
            this.groupBox2.Controls.Add(this.srcpathtextBox);
            this.groupBox2.Controls.Add(this.offsettextBox);
            this.groupBox2.Controls.Add(this.label19);
            this.groupBox2.Location = new System.Drawing.Point(12, 131);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(656, 285);
            this.groupBox2.TabIndex = 7;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "文件列表";
            // 
            // removebutton
            // 
            this.removebutton.Location = new System.Drawing.Point(563, 147);
            this.removebutton.Name = "removebutton";
            this.removebutton.Size = new System.Drawing.Size(75, 23);
            this.removebutton.TabIndex = 35;
            this.removebutton.Text = "删除";
            this.removebutton.UseVisualStyleBackColor = true;
            this.removebutton.Click += new System.EventHandler(this.removebutton_Click);
            // 
            // insertbutton
            // 
            this.insertbutton.Location = new System.Drawing.Point(563, 106);
            this.insertbutton.Name = "insertbutton";
            this.insertbutton.Size = new System.Drawing.Size(75, 23);
            this.insertbutton.TabIndex = 34;
            this.insertbutton.Text = "添加";
            this.insertbutton.UseVisualStyleBackColor = true;
            this.insertbutton.Click += new System.EventHandler(this.insertbutton_Click);
            // 
            // dataGridView1
            // 
            this.dataGridView1.AllowUserToAddRows = false;
            this.dataGridView1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.dataGridView1.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.dataGridView1.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.offset,
            this.path});
            this.dataGridView1.Location = new System.Drawing.Point(9, 20);
            this.dataGridView1.Name = "dataGridView1";
            this.dataGridView1.RowHeadersVisible = false;
            this.dataGridView1.RowTemplate.Height = 23;
            this.dataGridView1.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.dataGridView1.SelectionMode = System.Windows.Forms.DataGridViewSelectionMode.FullRowSelect;
            this.dataGridView1.Size = new System.Drawing.Size(541, 150);
            this.dataGridView1.TabIndex = 33;
            // 
            // offset
            // 
            this.offset.HeaderText = "偏移位置";
            this.offset.Name = "offset";
            this.offset.ReadOnly = true;
            this.offset.Width = 80;
            // 
            // path
            // 
            this.path.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.Fill;
            this.path.HeaderText = "文件路径";
            this.path.Name = "path";
            this.path.ReadOnly = true;
            // 
            // outviewbutton
            // 
            this.outviewbutton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.outviewbutton.Location = new System.Drawing.Point(563, 232);
            this.outviewbutton.Name = "outviewbutton";
            this.outviewbutton.Size = new System.Drawing.Size(75, 23);
            this.outviewbutton.TabIndex = 32;
            this.outviewbutton.Text = "浏览";
            this.outviewbutton.UseVisualStyleBackColor = true;
            // 
            // outpathtextBox
            // 
            this.outpathtextBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.outpathtextBox.BackColor = System.Drawing.SystemColors.Window;
            this.outpathtextBox.Location = new System.Drawing.Point(96, 235);
            this.outpathtextBox.Name = "outpathtextBox";
            this.outpathtextBox.ReadOnly = true;
            this.outpathtextBox.Size = new System.Drawing.Size(452, 21);
            this.outpathtextBox.TabIndex = 31;
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
            // viewbutton
            // 
            this.viewbutton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.viewbutton.Location = new System.Drawing.Point(565, 193);
            this.viewbutton.Name = "viewbutton";
            this.viewbutton.Size = new System.Drawing.Size(75, 23);
            this.viewbutton.TabIndex = 28;
            this.viewbutton.Text = "浏览";
            this.viewbutton.UseVisualStyleBackColor = true;
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
            // srcpathtextBox
            // 
            this.srcpathtextBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.srcpathtextBox.BackColor = System.Drawing.SystemColors.Window;
            this.srcpathtextBox.Location = new System.Drawing.Point(231, 195);
            this.srcpathtextBox.Name = "srcpathtextBox";
            this.srcpathtextBox.ReadOnly = true;
            this.srcpathtextBox.Size = new System.Drawing.Size(319, 21);
            this.srcpathtextBox.TabIndex = 23;
            // 
            // offsettextBox
            // 
            this.offsettextBox.Location = new System.Drawing.Point(96, 195);
            this.offsettextBox.Name = "offsettextBox";
            this.offsettextBox.Size = new System.Drawing.Size(94, 21);
            this.offsettextBox.TabIndex = 22;
            // 
            // label19
            // 
            this.label19.AutoSize = true;
            this.label19.Location = new System.Drawing.Point(7, 199);
            this.label19.Name = "label19";
            this.label19.Size = new System.Drawing.Size(53, 12);
            this.label19.TabIndex = 21;
            this.label19.Text = "偏移位置";
            // 
            // generatebutton
            // 
            this.generatebutton.Location = new System.Drawing.Point(454, 429);
            this.generatebutton.Name = "generatebutton";
            this.generatebutton.Size = new System.Drawing.Size(106, 23);
            this.generatebutton.TabIndex = 8;
            this.generatebutton.Text = "生成映像文件";
            this.generatebutton.UseVisualStyleBackColor = true;
            this.generatebutton.Click += new System.EventHandler(this.generatebutton_Click);
            // 
            // loadcfgbutton
            // 
            this.loadcfgbutton.Location = new System.Drawing.Point(108, 429);
            this.loadcfgbutton.Name = "loadcfgbutton";
            this.loadcfgbutton.Size = new System.Drawing.Size(104, 23);
            this.loadcfgbutton.TabIndex = 9;
            this.loadcfgbutton.Text = "导入配置文件";
            this.loadcfgbutton.UseVisualStyleBackColor = true;
            this.loadcfgbutton.Click += new System.EventHandler(this.loadcfgbutton_Click);
            // 
            // savecfgbutton
            // 
            this.savecfgbutton.Location = new System.Drawing.Point(281, 429);
            this.savecfgbutton.Name = "savecfgbutton";
            this.savecfgbutton.Size = new System.Drawing.Size(104, 23);
            this.savecfgbutton.TabIndex = 10;
            this.savecfgbutton.Text = "保存配置文件";
            this.savecfgbutton.UseVisualStyleBackColor = true;
            this.savecfgbutton.Click += new System.EventHandler(this.savecfgbutton_Click);
            // 
            // statusStrip
            // 
            this.statusStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.info});
            this.statusStrip.Location = new System.Drawing.Point(0, 462);
            this.statusStrip.Name = "statusStrip";
            this.statusStrip.Size = new System.Drawing.Size(680, 22);
            this.statusStrip.TabIndex = 11;
            this.statusStrip.Text = "statusStrip";
            // 
            // info
            // 
            this.info.Name = "info";
            this.info.Size = new System.Drawing.Size(56, 17);
            this.info.Text = "操作消息";
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(680, 484);
            this.Controls.Add(this.statusStrip);
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
            ((System.ComponentModel.ISupportInitialize)(this.dataGridView1)).EndInit();
            this.statusStrip.ResumeLayout(false);
            this.statusStrip.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

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
        private System.Windows.Forms.TextBox srcpathtextBox;
        private System.Windows.Forms.TextBox offsettextBox;
        private System.Windows.Forms.Label label19;
        private System.Windows.Forms.Button generatebutton;
        private System.Windows.Forms.ComboBox encryptcomboBox;
        private System.Windows.Forms.Label label20;
        private System.Windows.Forms.Button viewbutton;
        private System.Windows.Forms.TextBox hardvertextBox;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Button outviewbutton;
        private System.Windows.Forms.TextBox outpathtextBox;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.Button loadcfgbutton;
        private System.Windows.Forms.Button savecfgbutton;
        private System.Windows.Forms.StatusStrip statusStrip;
        private System.Windows.Forms.ToolStripStatusLabel info;
        private System.Windows.Forms.DataGridView dataGridView1;
        private System.Windows.Forms.Button removebutton;
        private System.Windows.Forms.Button insertbutton;
        private System.Windows.Forms.DataGridViewTextBoxColumn offset;
        private System.Windows.Forms.DataGridViewTextBoxColumn path;
    }
}

