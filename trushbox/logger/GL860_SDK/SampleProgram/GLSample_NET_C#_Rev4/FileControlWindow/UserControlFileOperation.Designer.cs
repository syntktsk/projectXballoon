namespace FileControlWindow
{
    partial class UserControlFileOperation
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
			this.button_CreateFolder = new System.Windows.Forms.Button();
			this.button_Delete = new System.Windows.Forms.Button();
			this.button_Download = new System.Windows.Forms.Button();
			this.SuspendLayout();
			// 
			// button_CreateFolder
			// 
			this.button_CreateFolder.Location = new System.Drawing.Point(88, 3);
			this.button_CreateFolder.Name = "button_CreateFolder";
			this.button_CreateFolder.Size = new System.Drawing.Size(124, 23);
			this.button_CreateFolder.TabIndex = 13;
			this.button_CreateFolder.Text = "Create Folder";
			this.button_CreateFolder.UseVisualStyleBackColor = true;
			this.button_CreateFolder.Click += new System.EventHandler(this.button_CreateFolder_Click);
			// 
			// button_Delete
			// 
			this.button_Delete.Location = new System.Drawing.Point(3, 3);
			this.button_Delete.Name = "button_Delete";
			this.button_Delete.Size = new System.Drawing.Size(79, 23);
			this.button_Delete.TabIndex = 12;
			this.button_Delete.Text = "Delete";
			this.button_Delete.UseVisualStyleBackColor = true;
			this.button_Delete.Click += new System.EventHandler(this.button_Delete_Click);
			// 
			// button_Download
			// 
			this.button_Download.Location = new System.Drawing.Point(218, 3);
			this.button_Download.Name = "button_Download";
			this.button_Download.Size = new System.Drawing.Size(79, 23);
			this.button_Download.TabIndex = 11;
			this.button_Download.Text = "Download";
			this.button_Download.UseVisualStyleBackColor = true;
			this.button_Download.Click += new System.EventHandler(this.button_Download_Click);
			// 
			// UserControlFileOperation
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.Controls.Add(this.button_CreateFolder);
			this.Controls.Add(this.button_Delete);
			this.Controls.Add(this.button_Download);
			this.Name = "UserControlFileOperation";
			this.Size = new System.Drawing.Size(479, 72);
			this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Button button_CreateFolder;
        private System.Windows.Forms.Button button_Delete;
        private System.Windows.Forms.Button button_Download;
    }
}
