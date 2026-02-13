namespace DevIo.Net_SampleProg
{
	partial class ConnectForm
	{
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.IContainer components = null;

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		/// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
		protected override void Dispose(bool disposing)
		{
			if (disposing && (components != null))
			{
				components.Dispose();
			}
			base.Dispose(disposing);
		}

		#region Windows Form Designer generated code

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.groupBox_ConnectSelect = new System.Windows.Forms.GroupBox();
			this.radioButton_TCP = new System.Windows.Forms.RadioButton();
			this.radioButton_Udp = new System.Windows.Forms.RadioButton();
			this.radioButton_Com = new System.Windows.Forms.RadioButton();
			this.radioButton_Usb = new System.Windows.Forms.RadioButton();
			this.comboBox_DeviceSelect = new System.Windows.Forms.ComboBox();
			this.textBox_Port = new System.Windows.Forms.TextBox();
			this.label_Port = new System.Windows.Forms.Label();
			this.label_IP = new System.Windows.Forms.Label();
			this.textBox_IP1 = new System.Windows.Forms.TextBox();
			this.textBox_IP2 = new System.Windows.Forms.TextBox();
			this.textBox_IP3 = new System.Windows.Forms.TextBox();
			this.textBox_IP4 = new System.Windows.Forms.TextBox();
			this.button_Connect = new System.Windows.Forms.Button();
			this.label_Title = new System.Windows.Forms.Label();
			this.label1 = new System.Windows.Forms.Label();
			this.label_Dot1 = new System.Windows.Forms.Label();
			this.label_Dot2 = new System.Windows.Forms.Label();
			this.label_Dot3 = new System.Windows.Forms.Label();
			this.button_search = new System.Windows.Forms.Button();
			this.groupBox_ConnectSelect.SuspendLayout();
			this.SuspendLayout();
			// 
			// groupBox_ConnectSelect
			// 
			this.groupBox_ConnectSelect.Controls.Add(this.radioButton_TCP);
			this.groupBox_ConnectSelect.Controls.Add(this.radioButton_Udp);
			this.groupBox_ConnectSelect.Controls.Add(this.radioButton_Com);
			this.groupBox_ConnectSelect.Controls.Add(this.radioButton_Usb);
			this.groupBox_ConnectSelect.Location = new System.Drawing.Point(17, 26);
			this.groupBox_ConnectSelect.Name = "groupBox_ConnectSelect";
			this.groupBox_ConnectSelect.Size = new System.Drawing.Size(76, 166);
			this.groupBox_ConnectSelect.TabIndex = 0;
			this.groupBox_ConnectSelect.TabStop = false;
			this.groupBox_ConnectSelect.Text = "Connection Selection";
			// 
			// radioButton_TCP
			// 
			this.radioButton_TCP.AutoSize = true;
			this.radioButton_TCP.Location = new System.Drawing.Point(12, 67);
			this.radioButton_TCP.Name = "radioButton_TCP";
			this.radioButton_TCP.Size = new System.Drawing.Size(45, 16);
			this.radioButton_TCP.TabIndex = 3;
			this.radioButton_TCP.TabStop = true;
			this.radioButton_TCP.Text = "TCP";
			this.radioButton_TCP.UseVisualStyleBackColor = true;
			this.radioButton_TCP.CheckedChanged += new System.EventHandler(this.radioButton_TCP_CheckedChanged);
			// 
			// radioButton_Udp
			// 
			this.radioButton_Udp.AutoSize = true;
			this.radioButton_Udp.Location = new System.Drawing.Point(12, 135);
			this.radioButton_Udp.Name = "radioButton_Udp";
			this.radioButton_Udp.Size = new System.Drawing.Size(46, 16);
			this.radioButton_Udp.TabIndex = 2;
			this.radioButton_Udp.TabStop = true;
			this.radioButton_Udp.Text = "UDP";
			this.radioButton_Udp.UseVisualStyleBackColor = true;
			this.radioButton_Udp.Visible = false;
			this.radioButton_Udp.CheckedChanged += new System.EventHandler(this.radioButton_Udp_CheckedChanged);
			// 
			// radioButton_Com
			// 
			this.radioButton_Com.AutoSize = true;
			this.radioButton_Com.Location = new System.Drawing.Point(12, 101);
			this.radioButton_Com.Name = "radioButton_Com";
			this.radioButton_Com.Size = new System.Drawing.Size(48, 16);
			this.radioButton_Com.TabIndex = 1;
			this.radioButton_Com.TabStop = true;
			this.radioButton_Com.Text = "COM";
			this.radioButton_Com.UseVisualStyleBackColor = true;
			this.radioButton_Com.CheckedChanged += new System.EventHandler(this.radioButton_Com_CheckedChanged);
			// 
			// radioButton_Usb
			// 
			this.radioButton_Usb.AutoSize = true;
			this.radioButton_Usb.Location = new System.Drawing.Point(12, 33);
			this.radioButton_Usb.Name = "radioButton_Usb";
			this.radioButton_Usb.Size = new System.Drawing.Size(46, 16);
			this.radioButton_Usb.TabIndex = 0;
			this.radioButton_Usb.TabStop = true;
			this.radioButton_Usb.Text = "USB";
			this.radioButton_Usb.UseVisualStyleBackColor = true;
			this.radioButton_Usb.CheckedChanged += new System.EventHandler(this.radioButton_Usb_CheckedChanged);
			// 
			// comboBox_DeviceSelect
			// 
			this.comboBox_DeviceSelect.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.comboBox_DeviceSelect.FormattingEnabled = true;
			this.comboBox_DeviceSelect.Location = new System.Drawing.Point(142, 58);
			this.comboBox_DeviceSelect.Name = "comboBox_DeviceSelect";
			this.comboBox_DeviceSelect.Size = new System.Drawing.Size(213, 20);
			this.comboBox_DeviceSelect.TabIndex = 1;
			this.comboBox_DeviceSelect.SelectedIndexChanged += new System.EventHandler(this.comboBox_DeviceSelect_SelectedIndexChanged);
			// 
			// textBox_Port
			// 
			this.textBox_Port.ImeMode = System.Windows.Forms.ImeMode.Off;
			this.textBox_Port.Location = new System.Drawing.Point(143, 124);
			this.textBox_Port.Name = "textBox_Port";
			this.textBox_Port.Size = new System.Drawing.Size(68, 19);
			this.textBox_Port.TabIndex = 2;
			this.textBox_Port.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.textBox_Port_KeyPress);
			// 
			// label_Port
			// 
			this.label_Port.AutoSize = true;
			this.label_Port.Location = new System.Drawing.Point(99, 128);
			this.label_Port.Name = "label_Port";
			this.label_Port.Size = new System.Drawing.Size(26, 12);
			this.label_Port.TabIndex = 3;
			this.label_Port.Text = "Port";
			// 
			// label_IP
			// 
			this.label_IP.AutoSize = true;
			this.label_IP.Location = new System.Drawing.Point(99, 165);
			this.label_IP.Name = "label_IP";
			this.label_IP.Size = new System.Drawing.Size(15, 12);
			this.label_IP.TabIndex = 4;
			this.label_IP.Text = "IP";
			// 
			// textBox_IP1
			// 
			this.textBox_IP1.Location = new System.Drawing.Point(143, 165);
			this.textBox_IP1.Name = "textBox_IP1";
			this.textBox_IP1.Size = new System.Drawing.Size(46, 19);
			this.textBox_IP1.TabIndex = 5;
			this.textBox_IP1.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.textBox_IP1_KeyPress);
			// 
			// textBox_IP2
			// 
			this.textBox_IP2.Location = new System.Drawing.Point(199, 165);
			this.textBox_IP2.Name = "textBox_IP2";
			this.textBox_IP2.Size = new System.Drawing.Size(46, 19);
			this.textBox_IP2.TabIndex = 6;
			this.textBox_IP2.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.textBox_IP2_KeyPress);
			// 
			// textBox_IP3
			// 
			this.textBox_IP3.Location = new System.Drawing.Point(254, 165);
			this.textBox_IP3.Name = "textBox_IP3";
			this.textBox_IP3.Size = new System.Drawing.Size(46, 19);
			this.textBox_IP3.TabIndex = 7;
			this.textBox_IP3.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.textBox_IP3_KeyPress);
			// 
			// textBox_IP4
			// 
			this.textBox_IP4.Location = new System.Drawing.Point(309, 165);
			this.textBox_IP4.Name = "textBox_IP4";
			this.textBox_IP4.Size = new System.Drawing.Size(46, 19);
			this.textBox_IP4.TabIndex = 8;
			this.textBox_IP4.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.textBox_IP4_KeyPress);
			// 
			// button_Connect
			// 
			this.button_Connect.Location = new System.Drawing.Point(378, 124);
			this.button_Connect.Name = "button_Connect";
			this.button_Connect.Size = new System.Drawing.Size(95, 59);
			this.button_Connect.TabIndex = 9;
			this.button_Connect.Text = "Connect";
			this.button_Connect.UseVisualStyleBackColor = true;
			this.button_Connect.Click += new System.EventHandler(this.button_Connect_Click);
			// 
			// label_Title
			// 
			this.label_Title.AutoSize = true;
			this.label_Title.Font = new System.Drawing.Font("MS UI Gothic", 20F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
			this.label_Title.Location = new System.Drawing.Point(130, 9);
			this.label_Title.Name = "label_Title";
			this.label_Title.Size = new System.Drawing.Size(285, 27);
			this.label_Title.TabIndex = 10;
			this.label_Title.Text = "Device connection form";
			// 
			// label1
			// 
			this.label1.AutoSize = true;
			this.label1.Location = new System.Drawing.Point(99, 61);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(37, 12);
			this.label1.TabIndex = 11;
			this.label1.Text = "Select";
			// 
			// label_Dot1
			// 
			this.label_Dot1.AutoSize = true;
			this.label_Dot1.Location = new System.Drawing.Point(189, 172);
			this.label_Dot1.Name = "label_Dot1";
			this.label_Dot1.Size = new System.Drawing.Size(7, 12);
			this.label_Dot1.TabIndex = 12;
			this.label_Dot1.Text = ".";
			// 
			// label_Dot2
			// 
			this.label_Dot2.AutoSize = true;
			this.label_Dot2.Location = new System.Drawing.Point(244, 172);
			this.label_Dot2.Name = "label_Dot2";
			this.label_Dot2.Size = new System.Drawing.Size(7, 12);
			this.label_Dot2.TabIndex = 13;
			this.label_Dot2.Text = ".";
			// 
			// label_Dot3
			// 
			this.label_Dot3.AutoSize = true;
			this.label_Dot3.Location = new System.Drawing.Point(299, 172);
			this.label_Dot3.Name = "label_Dot3";
			this.label_Dot3.Size = new System.Drawing.Size(7, 12);
			this.label_Dot3.TabIndex = 14;
			this.label_Dot3.Text = ".";
			// 
			// button_search
			// 
			this.button_search.Location = new System.Drawing.Point(378, 57);
			this.button_search.Name = "button_search";
			this.button_search.Size = new System.Drawing.Size(59, 20);
			this.button_search.TabIndex = 15;
			this.button_search.Text = "Search";
			this.button_search.UseVisualStyleBackColor = true;
			this.button_search.Click += new System.EventHandler(this.button_search_Click);
			// 
			// ConnectForm
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(495, 209);
			this.Controls.Add(this.button_search);
			this.Controls.Add(this.textBox_IP4);
			this.Controls.Add(this.textBox_IP3);
			this.Controls.Add(this.textBox_IP2);
			this.Controls.Add(this.textBox_IP1);
			this.Controls.Add(this.label_Dot3);
			this.Controls.Add(this.label_Dot2);
			this.Controls.Add(this.label_Dot1);
			this.Controls.Add(this.label1);
			this.Controls.Add(this.label_Title);
			this.Controls.Add(this.button_Connect);
			this.Controls.Add(this.label_IP);
			this.Controls.Add(this.label_Port);
			this.Controls.Add(this.textBox_Port);
			this.Controls.Add(this.comboBox_DeviceSelect);
			this.Controls.Add(this.groupBox_ConnectSelect);
			this.MaximizeBox = false;
			this.Name = "ConnectForm";
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
			this.Text = "ConnectForm";
			this.TopMost = true;
			this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.ConnectForm_FormClosed);
			this.Load += new System.EventHandler(this.ConnectForm_Load);
			this.groupBox_ConnectSelect.ResumeLayout(false);
			this.groupBox_ConnectSelect.PerformLayout();
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.GroupBox groupBox_ConnectSelect;
		private System.Windows.Forms.RadioButton radioButton_TCP;
		private System.Windows.Forms.RadioButton radioButton_Udp;
		private System.Windows.Forms.RadioButton radioButton_Com;
		private System.Windows.Forms.RadioButton radioButton_Usb;
		private System.Windows.Forms.ComboBox comboBox_DeviceSelect;
		private System.Windows.Forms.TextBox textBox_Port;
		private System.Windows.Forms.Label label_Port;
		private System.Windows.Forms.Label label_IP;
		private System.Windows.Forms.TextBox textBox_IP1;
		private System.Windows.Forms.TextBox textBox_IP2;
		private System.Windows.Forms.TextBox textBox_IP3;
		private System.Windows.Forms.TextBox textBox_IP4;
		private System.Windows.Forms.Button button_Connect;
		private System.Windows.Forms.Label label_Title;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.Label label_Dot1;
		private System.Windows.Forms.Label label_Dot2;
		private System.Windows.Forms.Label label_Dot3;
		private System.Windows.Forms.Button button_search;
	}
}