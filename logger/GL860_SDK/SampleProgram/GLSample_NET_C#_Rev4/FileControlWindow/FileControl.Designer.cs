namespace FileControlWindow
{
    partial class FileControl
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

        #region Windows フォーム デザイナーで生成されたコード

        /// <summary>
        /// デザイナー サポートに必要なメソッドです。このメソッドの内容を
        /// コード エディターで変更しないでください。
        /// </summary>
        private void InitializeComponent()
        {
			this.treeView_File = new System.Windows.Forms.TreeView();
			this.button_Close = new System.Windows.Forms.Button();
			this.listView_Filelist = new System.Windows.Forms.ListView();
			this.splitContainer1 = new System.Windows.Forms.SplitContainer();
			this.labelFileStatus = new System.Windows.Forms.Label();
			this.buttonReload = new System.Windows.Forms.Button();
			((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).BeginInit();
			this.splitContainer1.Panel1.SuspendLayout();
			this.splitContainer1.Panel2.SuspendLayout();
			this.splitContainer1.SuspendLayout();
			this.SuspendLayout();
			// 
			// treeView_File
			// 
			this.treeView_File.Dock = System.Windows.Forms.DockStyle.Fill;
			this.treeView_File.Location = new System.Drawing.Point(0, 0);
			this.treeView_File.Name = "treeView_File";
			this.treeView_File.Size = new System.Drawing.Size(212, 388);
			this.treeView_File.TabIndex = 0;
			this.treeView_File.BeforeExpand += new System.Windows.Forms.TreeViewCancelEventHandler(this.treeView_File_BeforeExpand);
			this.treeView_File.NodeMouseClick += new System.Windows.Forms.TreeNodeMouseClickEventHandler(this.treeView_File_NodeMouseClick);
			// 
			// button_Close
			// 
			this.button_Close.Location = new System.Drawing.Point(713, 449);
			this.button_Close.Name = "button_Close";
			this.button_Close.Size = new System.Drawing.Size(75, 23);
			this.button_Close.TabIndex = 2;
			this.button_Close.Text = "Close";
			this.button_Close.UseVisualStyleBackColor = true;
			this.button_Close.Click += new System.EventHandler(this.button_Close_Click);
			// 
			// listView_Filelist
			// 
			this.listView_Filelist.Dock = System.Windows.Forms.DockStyle.Fill;
			this.listView_Filelist.HideSelection = false;
			this.listView_Filelist.Location = new System.Drawing.Point(0, 0);
			this.listView_Filelist.Name = "listView_Filelist";
			this.listView_Filelist.Size = new System.Drawing.Size(560, 388);
			this.listView_Filelist.TabIndex = 3;
			this.listView_Filelist.UseCompatibleStateImageBehavior = false;
			this.listView_Filelist.SelectedIndexChanged += new System.EventHandler(this.listView_Filelist_SelectedIndexChanged);
			this.listView_Filelist.KeyDown += new System.Windows.Forms.KeyEventHandler(this.listView_Filelist_KeyDown);
			// 
			// splitContainer1
			// 
			this.splitContainer1.Location = new System.Drawing.Point(12, 12);
			this.splitContainer1.Name = "splitContainer1";
			// 
			// splitContainer1.Panel1
			// 
			this.splitContainer1.Panel1.Controls.Add(this.treeView_File);
			// 
			// splitContainer1.Panel2
			// 
			this.splitContainer1.Panel2.Controls.Add(this.listView_Filelist);
			this.splitContainer1.Size = new System.Drawing.Size(776, 388);
			this.splitContainer1.SplitterDistance = 212;
			this.splitContainer1.TabIndex = 6;
			// 
			// labelFileStatus
			// 
			this.labelFileStatus.AutoSize = true;
			this.labelFileStatus.Location = new System.Drawing.Point(226, 403);
			this.labelFileStatus.Name = "labelFileStatus";
			this.labelFileStatus.Size = new System.Drawing.Size(0, 12);
			this.labelFileStatus.TabIndex = 7;
			// 
			// buttonReload
			// 
			this.buttonReload.Location = new System.Drawing.Point(12, 449);
			this.buttonReload.Name = "buttonReload";
			this.buttonReload.Size = new System.Drawing.Size(75, 23);
			this.buttonReload.TabIndex = 8;
			this.buttonReload.Text = "Reload";
			this.buttonReload.UseVisualStyleBackColor = true;
			this.buttonReload.Click += new System.EventHandler(this.buttonReload_Click);
			// 
			// FileControl
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(800, 484);
			this.Controls.Add(this.buttonReload);
			this.Controls.Add(this.labelFileStatus);
			this.Controls.Add(this.splitContainer1);
			this.Controls.Add(this.button_Close);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
			this.Name = "FileControl";
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
			this.Text = "File Form";
			this.Load += new System.EventHandler(this.FileControl_Load);
			this.splitContainer1.Panel1.ResumeLayout(false);
			this.splitContainer1.Panel2.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).EndInit();
			this.splitContainer1.ResumeLayout(false);
			this.ResumeLayout(false);
			this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TreeView treeView_File;
        private System.Windows.Forms.Button button_Close;
        private System.Windows.Forms.ListView listView_Filelist;
		private System.Windows.Forms.SplitContainer splitContainer1;
		private System.Windows.Forms.Label labelFileStatus;
		private System.Windows.Forms.Button buttonReload;
        //public ListView ListView_Filelist { get => listView_Filelist; set => listView_Filelist = value; }
    }
}

