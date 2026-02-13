namespace FileControlWindow
{
    partial class UserControlFileReference
    {
        /// <summary> 
        /// 必要なデザイナー変数です。
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary> 
        /// 使用中のリソースをすべてクリーンアップします。
        /// </summary>
        /// <param name="disposing">マネージド リソースを破棄する場合は true を指定し、その他の場合は false を指定します。</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region コンポーネント デザイナーで生成されたコード

        /// <summary> 
        /// デザイナー サポートに必要なメソッドです。このメソッドの内容を 
        /// コード エディターで変更しないでください。
        /// </summary>
        private void InitializeComponent()
        {
			this.buttonOk = new System.Windows.Forms.Button();
			this.textBoxFileName = new System.Windows.Forms.TextBox();
			this.buttonCreateFolder = new System.Windows.Forms.Button();
			this.buttonDelete = new System.Windows.Forms.Button();
			this.buttonInputFile = new System.Windows.Forms.Button();
			this.SuspendLayout();
			// 
			// buttonOk
			// 
			this.buttonOk.Location = new System.Drawing.Point(401, 33);
			this.buttonOk.Name = "buttonOk";
			this.buttonOk.Size = new System.Drawing.Size(75, 23);
			this.buttonOk.TabIndex = 19;
			this.buttonOk.Text = "OK";
			this.buttonOk.UseVisualStyleBackColor = true;
			this.buttonOk.Click += new System.EventHandler(this.buttonOk_Click);
			// 
			// textBoxFileName
			// 
			this.textBoxFileName.Enabled = false;
			this.textBoxFileName.Location = new System.Drawing.Point(341, 5);
			this.textBoxFileName.Name = "textBoxFileName";
			this.textBoxFileName.Size = new System.Drawing.Size(135, 19);
			this.textBoxFileName.TabIndex = 18;
			// 
			// buttonCreateFolder
			// 
			this.buttonCreateFolder.Location = new System.Drawing.Point(81, 3);
			this.buttonCreateFolder.Name = "buttonCreateFolder";
			this.buttonCreateFolder.Size = new System.Drawing.Size(124, 23);
			this.buttonCreateFolder.TabIndex = 15;
			this.buttonCreateFolder.Text = "Create Folder";
			this.buttonCreateFolder.UseVisualStyleBackColor = true;
			this.buttonCreateFolder.Click += new System.EventHandler(this.buttonCreateFolder_Click);
			// 
			// buttonDelete
			// 
			this.buttonDelete.Location = new System.Drawing.Point(3, 3);
			this.buttonDelete.Name = "buttonDelete";
			this.buttonDelete.Size = new System.Drawing.Size(75, 23);
			this.buttonDelete.TabIndex = 16;
			this.buttonDelete.Text = "Delete";
			this.buttonDelete.Click += new System.EventHandler(this.buttonDelete_Click);
			// 
			// buttonInputFile
			// 
			this.buttonInputFile.Location = new System.Drawing.Point(211, 3);
			this.buttonInputFile.Name = "buttonInputFile";
			this.buttonInputFile.Size = new System.Drawing.Size(124, 23);
			this.buttonInputFile.TabIndex = 17;
			this.buttonInputFile.Text = "Input File";
			this.buttonInputFile.Click += new System.EventHandler(this.buttonInputFile_Click);
			// 
			// UserControlFileReference
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.Controls.Add(this.buttonOk);
			this.Controls.Add(this.textBoxFileName);
			this.Controls.Add(this.buttonCreateFolder);
			this.Controls.Add(this.buttonDelete);
			this.Controls.Add(this.buttonInputFile);
			this.Name = "UserControlFileReference";
			this.Size = new System.Drawing.Size(479, 72);
			this.Load += new System.EventHandler(this.UserControlFileReference_Load);
			this.ResumeLayout(false);
			this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button buttonOk;
        private System.Windows.Forms.TextBox textBoxFileName;
        private System.Windows.Forms.Button buttonCreateFolder;
        private System.Windows.Forms.Button buttonDelete;
        private System.Windows.Forms.Button buttonInputFile;
    }
}
