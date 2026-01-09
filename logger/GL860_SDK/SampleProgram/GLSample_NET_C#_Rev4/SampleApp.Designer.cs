namespace DevIo.Net_SampleProg
{
	partial class SampleApp
	{
		/// <summary>
		/// 必要なデザイナー変数です。
		/// </summary>
		private System.ComponentModel.IContainer components = null;

		/// <summary>
		/// 使用中のリソースをすべてクリーンアップします。
		/// </summary>
		/// <param name="disposing">マネージ リソースが破棄される場合 true、破棄されない場合は false です。</param>
		protected override void Dispose(bool disposing)
		{
			if (disposing && (components != null))
			{
				components.Dispose();
			}
			base.Dispose(disposing);
		}

		#region Windows フォーム デザイナーで生成されたコード

		/// <summary>
		/// デザイナー サポートに必要なメソッドです。このメソッドの内容を
		/// コード エディターで変更しないでください。
		/// </summary>
		private void InitializeComponent()
		{
			this.richTextBox = new System.Windows.Forms.RichTextBox();
			this.button_Connect = new System.Windows.Forms.Button();
			this.button_Udp = new System.Windows.Forms.Button();
			this.button_Clr = new System.Windows.Forms.Button();
			this.comboBoxCommand = new System.Windows.Forms.ComboBox();
			this.buttonFile = new System.Windows.Forms.Button();
			this.SuspendLayout();
			// 
			// richTextBox
			// 
			this.richTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
			this.richTextBox.Font = new System.Drawing.Font("ＭＳ ゴシック", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
			this.richTextBox.Location = new System.Drawing.Point(11, 41);
			this.richTextBox.Name = "richTextBox";
			this.richTextBox.Size = new System.Drawing.Size(855, 417);
			this.richTextBox.TabIndex = 0;
			this.richTextBox.Text = "";
			this.richTextBox.KeyDown += new System.Windows.Forms.KeyEventHandler(this.richTextBox_KeyDown);
			this.richTextBox.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.richTextBox_KeyPress);
			// 
			// button_Connect
			// 
			this.button_Connect.Location = new System.Drawing.Point(12, 5);
			this.button_Connect.Name = "button_Connect";
			this.button_Connect.Size = new System.Drawing.Size(95, 31);
			this.button_Connect.TabIndex = 1;
			this.button_Connect.Text = "Connect";
			this.button_Connect.UseVisualStyleBackColor = true;
			this.button_Connect.Click += new System.EventHandler(this.button_Connect_Click);
			// 
			// button_Udp
			// 
			this.button_Udp.Location = new System.Drawing.Point(542, 4);
			this.button_Udp.Name = "button_Udp";
			this.button_Udp.Size = new System.Drawing.Size(140, 30);
			this.button_Udp.TabIndex = 3;
			this.button_Udp.Text = "GL Search by UDP BC";
			this.button_Udp.UseVisualStyleBackColor = true;
			this.button_Udp.Click += new System.EventHandler(this.button_Udp_Click);
			// 
			// button_Clr
			// 
			this.button_Clr.Location = new System.Drawing.Point(688, 4);
			this.button_Clr.Name = "button_Clr";
			this.button_Clr.Size = new System.Drawing.Size(69, 29);
			this.button_Clr.TabIndex = 4;
			this.button_Clr.Text = "Clr";
			this.button_Clr.UseVisualStyleBackColor = true;
			this.button_Clr.Click += new System.EventHandler(this.button_Clr_Click);
			// 
			// comboBoxCommand
			// 
			this.comboBoxCommand.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.comboBoxCommand.Font = new System.Drawing.Font("ＭＳ ゴシック", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
			this.comboBoxCommand.FormattingEnabled = true;
			this.comboBoxCommand.Location = new System.Drawing.Point(113, 10);
			this.comboBoxCommand.Name = "comboBoxCommand";
			this.comboBoxCommand.Size = new System.Drawing.Size(423, 21);
			this.comboBoxCommand.TabIndex = 5;
			this.comboBoxCommand.SelectedIndexChanged += new System.EventHandler(this.comboBoxCommand_SelectedIndexChanged);
			// 
			// buttonFile
			// 
			this.buttonFile.Location = new System.Drawing.Point(791, 5);
			this.buttonFile.Name = "buttonFile";
			this.buttonFile.Size = new System.Drawing.Size(75, 29);
			this.buttonFile.TabIndex = 6;
			this.buttonFile.Text = "File";
			this.buttonFile.UseVisualStyleBackColor = true;
			this.buttonFile.Click += new System.EventHandler(this.buttonFile_Click);
			// 
			// SampleApp
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(872, 467);
			this.Controls.Add(this.buttonFile);
			this.Controls.Add(this.comboBoxCommand);
			this.Controls.Add(this.button_Clr);
			this.Controls.Add(this.button_Udp);
			this.Controls.Add(this.button_Connect);
			this.Controls.Add(this.richTextBox);
			this.DoubleBuffered = true;
			this.MaximizeBox = false;
			this.Name = "SampleApp";
			this.Text = "GtcDevIoSampleApp";
			this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.SampleProg_FormClosed);
			this.Load += new System.EventHandler(this.SampleApp_Load);
			this.ResumeLayout(false);

		}

		#endregion

		private System.Windows.Forms.RichTextBox richTextBox;
		private System.Windows.Forms.Button button_Connect;
		private System.Windows.Forms.Button button_Udp;
		private System.Windows.Forms.Button button_Clr;
		public System.Windows.Forms.ComboBox comboBoxCommand;
		private System.Windows.Forms.Button buttonFile;
	}
}

