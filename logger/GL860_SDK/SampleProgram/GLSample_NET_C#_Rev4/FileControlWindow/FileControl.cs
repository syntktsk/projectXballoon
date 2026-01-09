using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace FileControlWindow
{
    public enum FileFormType
    {
        Opeartion,
        Reference,
    }

    public enum FileNameType
    {
        Auto,
        Manual,
    }

    public enum FileExtType
    {
        GBD,
        CSV,
        CND,
    }

    public partial class FileControl : Form
    {
        public class Builder
        {
            internal GtcDevIo.DevIo _DevIo;
            internal FileFormType _fileFormType;
            internal UserControl _userControl;
            internal FileNameType _fileNameType;
            internal FileExtType _fileExtType;
			internal string _fileName = "DEFAULT";

            private Builder()
            {

            }

            public Builder SetDevIo(GtcDevIo.DevIo DevIo) { _DevIo = DevIo; return this; }
            public Builder SetFormType(FileFormType type)
            {
                _fileFormType = type;
                switch (type)
                {
                    case FileFormType.Opeartion:
                        _userControl = new UserControlFileOperation();
                        break;
                    case FileFormType.Reference:
                        _userControl = new UserControlFileReference();
                        break;
                }
                _userControl.Location = new System.Drawing.Point(228, 416); //(228, 406);
				_userControl.Name = "userControlFileOperation";
                _userControl.Size = new System.Drawing.Size(479, 62);   //(479, 72)
				_userControl.TabIndex = 10;
                return this;
            }

			public Builder SetFileNameString(string name)
			{
				_fileName = name;
				return this;
			}

			public Builder SetFileNameType(FileNameType type)
            {
                _fileNameType = type;
                return this;
            }

            public Builder SetFileExtType(FileExtType type)
            {
                _fileExtType = type;
                return this;
            }

            public static Builder Instance
            {
                get { return new Builder(); }
            }

            public FileControl Build()
            {
                return new FileControl(this);
            }

            public CNDFileControl BuildCND()
            {
                return new CNDFileControl(this);
            }
        }

        public string FileNameString { get; set; }

        public FileNameType FileNameType { get; set; }
        public FileExtType FileExtType { get; set; }

        public UserControl UserControl { get; set; }
        public GtcDevIo.DevIo DevIo { get; set; }
        DriveInformation DriveInformation { get; set; }
        DirectoryInformation DirectoryInformation { get; set; }
        FileInformation FileInformation { get; set; }
        protected FileAction FileDownload { get; set; }
        FileAction FileDelete { get; set; }
        FolderDelete FolderDelete { get; set; }

        SelectFilePath SelectFilePath { get; set; }
        protected SaveCND SaveCND { get; set; }
        protected LoadCND LoadCND { get; set; }

        CreateFolder CreateFolder { get; set; }

        protected string CurrentFullPath { get; set; }

        public string SelectedFilePath { get; set; } = "";

		public int FileNumber { get; set; } = 0;
		public int SelectedFileNumber { get; set; } = 0;

        public FileControl()
        {
            InitializeComponent();
        }

        protected FileControl(Builder builder)
        {
            InitializeComponent();
            EnableDoubleBuffering(listView_Filelist);
            if (builder._userControl != null)
            {
                UserControl = builder._userControl;
                this.Controls.Add(UserControl);
            }
			FileNameString = builder._fileName;
			if (builder._fileFormType == FileFormType.Reference)
			{
				((UserControlFileReference)UserControl).SetFileName($"{FileNameString}." + GetFileExtension(builder._fileExtType));
			}
            FileNameType = builder._fileNameType;
            FileExtType = builder._fileExtType;
            this.DevIo = builder._DevIo;
			var b = CheckConnection();

            DriveInformation = new DriveInformationGL(DevIo);
            DirectoryInformation = new DirectoryInformationGL(DevIo);
            FileInformation = new FileInformationGL(DevIo);
            FileDownload = new FileDownload(DevIo);
            FileDelete = new FileDelete(DevIo);
            FolderDelete = new FolderDeleteGL(DevIo);
            CreateFolder = new CreateFolderGL(DevIo);
            SelectFilePath = new SelectFilePathGL(DevIo);
            SaveCND = new SaveCNDGL(DevIo);
			LoadCND = new LoadCNDGL(DevIo);
        }

        public string GetFileExtension(FileExtType extType)
        {
            switch (extType)
            {
                case FileExtType.CND:
                    return "CND";
                case FileExtType.GBD:
                    return "GBD";
                case FileExtType.CSV:
                    return "CSV";
                default:
                    return "";
            }
        }

        private bool CheckConnection()
		{
			var s = DevIo.SendQuery("*IDN?");
			if (s == "")
			{
				MessageBox.Show("Connect Error!!", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
				return false;
			}

			return true;
		}

        /// <summary>
        /// Returns the file size converted to units.
        /// </summary>
        /// <param name="fileSize"></param>
        /// <returns></returns>
        public static String getFileSize(long fileSize)
        {
            String ret = fileSize + " Byte";
            if (fileSize > (1024f * 1024f * 1024f))
            {
                ret = Math.Round((fileSize / 1024f / 1024f / 1024f), 2).ToString() + " GB";
            }
            else if (fileSize > (1024f * 1024f))
            {
                ret = Math.Round((fileSize / 1024f / 1024f), 2).ToString() + " MB";
            }
            else if (fileSize > 1024f)
            {
                ret = Math.Round((fileSize / 1024f), 2).ToString() + " kB";
            }

            return ret;
        }

		public void ShowFileNumberStr()
		{
			var str = "";
			if (SelectedFileNumber > 0)
			{
				switch (FileNumber)
				{
					case 0: break;
					case 1: str = $"{FileNumber} item / Select {SelectedFileNumber} item"; break;
					default:
						{
							str = $"{FileNumber} items / Select {SelectedFileNumber} " + ((SelectedFileNumber > 1)? "items" : "item");
							break;
						}
				}
			}
			else
			{
				switch (FileNumber)
				{
					case 0: break;
					case 1: str = $"{FileNumber} item"; break;
					default: str = $"{FileNumber} items"; break; ;
				}
			}

			if (str != "") labelFileStatus.Text = str;
		}

        /// <summary>
        /// Set list view items.
        /// </summary>
        private void setListItem(String filePath)
        {
			Cursor = Cursors.WaitCursor;

			listView_Filelist.BeginUpdate();

            // Set list view header
            listView_Filelist.View = View.Details;
            listView_Filelist.Clear();
            listView_Filelist.Columns.Add("File Name");
            listView_Filelist.Columns.Add("Date");
            listView_Filelist.Columns.Add("Size");

            try
            {
				// File list
				var count = 0;
                foreach (var fi in FileInformation.GetFiles(filePath))
                {
                    ListViewItem item = new ListViewItem(fi.Name);
                    item.SubItems.Add(String.Format("{0:yyyy/MM/dd HH:mm:ss}", fi.LastAccessTime));
                    item.SubItems.Add(getFileSize(fi.Length));
                    item.SubItems[2].Tag = fi.Length;
                    listView_Filelist.Items.Add(item);
					count++;
                }
				FileNumber = count;
				SelectedFileNumber = 0;
				ShowFileNumberStr();
            }
            catch (IOException ie)
            {
                MessageBox.Show(ie.Message, "Selection error");
            }

            // Automatically adjust column width
            listView_Filelist.AutoResizeColumns(ColumnHeaderAutoResizeStyle.HeaderSize);

            listView_Filelist.EndUpdate();

			Cursor = Cursors.Default;
		}



        private void ButtonRead_Click(object sender, EventArgs e)
        {
			Cursor = Cursors.WaitCursor;

            // Scan drive list and add to tree
            foreach (String drive in DriveInformation.GetRootDrive())
            {
                // Create new node
                // Add an empty node to display the plus button
                TreeNode node = new TreeNode(drive);
                node.Nodes.Add(new TreeNode());
                treeView_File.Nodes.Add(node);
            }
            // Display the contents of the initially selected drive
            try { setListItem(DriveInformation.GetRootDrive().First()); }
            catch { }

			Cursor = Cursors.Default;
        }

        /// <summary>
        /// Event handler when expanding tree view items.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void treeView_File_BeforeExpand(object sender, TreeViewCancelEventArgs e)
        {
            TreeNode node = e.Node;
            TreeViewItem(node);
        }

        private void TreeViewItem(TreeNode node)
        {
            if (node == null) return;

            String path = node.FullPath;
            CurrentFullPath = path;
            node.Nodes.Clear();

            try
            {
                foreach (var di in DirectoryInformation.GetDirectories(path))
                {
                    TreeNode child = new TreeNode(di.Name);
                    child.Nodes.Add(new TreeNode());
                    node.Nodes.Add(child);
                }
            }
            catch (IOException ie)
            {
                MessageBox.Show(ie.Message, "Selection error");
            }
        }

        /// Event handler when selecting a tree view item.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void treeView_File_NodeMouseClick(object sender, TreeNodeMouseClickEventArgs e)
        {
            TreeNodeExpand(e.Node);
        }

        private void TreeNodeExpand(TreeNode node)
        {
            if (node == null) return;
            node.Expand();
            setListItem(node.FullPath);
            CurrentFullPath = node.FullPath;
        }

        public virtual void Download()
        {
            var lvs = listView_Filelist.SelectedItems;   // Selected ListView item

            if (lvs.Count == 0)
            {
                MessageBox.Show("Selection error");
                return;
            }

            //var tvs = treeView.Nodes.SelectedNode;    // Selected TreeView node

            // Download file collection
            FileDownload.ClearActionList();
            for (var i = 0; i < listView_Filelist.SelectedItems.Count; i++)
            {
                var name = listView_Filelist.SelectedItems[i].Text;
                var size = (Int64)listView_Filelist.SelectedItems[i].SubItems[2].Tag;
                var filepath = Path.Combine(CurrentFullPath, name);
                FileDownload.AddActionList(name, filepath, size);
            }

            // Confirm destination path
            var destpath = FolderBrowserDialog();
            if (destpath.Length == 0)
            {
                MessageBox.Show("Save destination not specified");
                return;
            }
            using (var form = new ProgressForm())
            {
                form.FileAction = this.FileDownload;
                form.DestinationPath = destpath;
                form.ShowDialog();
            }
        }

        private void FileControl_Load(object sender, EventArgs e)
        {
            UserControl.Parent = this;
            ButtonRead_Click(sender, e);
        }

        public void Delete()
        {
            var lvs = listView_Filelist.SelectedItems;   // Selected ListView item

            if (lvs.Count == 0)
            {
                DeleteDirectory();
                return;
            }

            DeleteListView();
        }

        private void DeleteListView()
        {
			var result = MessageBox.Show($"{SelectedFileNumber} files are selected.\r\nDo you want to delete it?", "Delete file", MessageBoxButtons.OKCancel);
			if (result == DialogResult.Cancel) return;

            FileDelete.ClearActionList();
            for (var i = 0; i < listView_Filelist.SelectedItems.Count; i++)
            {
                var name = listView_Filelist.SelectedItems[i].Text;
                var filepath = Path.Combine(CurrentFullPath, name);
                FileDelete.AddActionList(name, filepath);
            }

            FileDelete.Exec();
            setListItem(CurrentFullPath);
        }

        private void DeleteDirectory()
        {
            if (CurrentFullPath == null)
            {
                MessageBox.Show("Selection error");
                return;
            }
            FolderDelete.Exec(CurrentFullPath);
            var node = treeView_File.SelectedNode.Parent;
            TreeViewItem(node);
        }

        /// <summary>
        /// Set the control's DoubleBuffered property to True
        /// </summary>
        /// <param name="control">Target control</param>
        public static void EnableDoubleBuffering(Control control)
        {
            control.GetType().InvokeMember(
               "DoubleBuffered",
               BindingFlags.NonPublic | BindingFlags.Instance | BindingFlags.SetProperty,
               null,
               control,
               new object[] { true });
        }

        public static string FolderBrowserDialog()
        {
            FolderBrowserDialog fbDialog = new FolderBrowserDialog();

            // Specify the description of the dialog
            fbDialog.Description = "Dialog";

            // Specify default folder
            fbDialog.SelectedPath = System.IO.Directory.GetCurrentDirectory();

            // Show "Create new folder" button
            fbDialog.ShowNewFolderButton = true;

            // Displays a dialog for selecting a folder
            string path = "";
            if (fbDialog.ShowDialog() == DialogResult.OK)
            {
                path = fbDialog.SelectedPath;
            }

            fbDialog.Dispose(); // Destroy an object

            return path;
        }

        private void button_Close_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        public void CreateDirectory(string filename)
        {
            if (CurrentFullPath == null)
            {
                MessageBox.Show("Selection error");
                return;
            }
            var filepath = Path.Combine(CurrentFullPath, filename);
            CreateFolder.Exec(filepath);
            var node = treeView_File.SelectedNode;
            TreeViewItem(node);
            TreeNodeExpand(node);
        }

        public virtual void SelectPath(string filename)
        {
            if (CurrentFullPath == null)
            {
                MessageBox.Show("Selection error");
                return;
            }

            if (FileNameType == FileNameType.Manual)
            {
                if (filename == null || filename.Length == 0)
                {
                    MessageBox.Show("File name error");
                    return;
                }
            }

            var filepath = (FileNameType == FileNameType.Manual) ? Path.Combine(CurrentFullPath, filename) : $"{CurrentFullPath}\\";
            var ret = SelectFilePath.Exec(filepath);

            if (ret == false)
            {
                ShowCheckConnectionMsg();
                return;
            }

            SelectedFilePath = filepath;
            this.Close();
        }

        public void ShowCheckConnectionMsg()
        {
            MessageBox.Show("Value Change!!!\r\n Check Connection!!!!", "Value Change", MessageBoxButtons.OK, MessageBoxIcon.Information);
        }

		private void listView_Filelist_SelectedIndexChanged(object sender, EventArgs e)
		{
			SelectedFileNumber = listView_Filelist.SelectedItems.Count;
			ShowFileNumberStr();
		}

		private void listView_Filelist_KeyDown(object sender, KeyEventArgs e)
		{
			if (e.KeyCode == Keys.A && e.Control)
			{
				listView_Filelist.MultiSelect = true;
				foreach (ListViewItem item in listView_Filelist.Items)
				{
					item.Selected = true;
				}
			}
			else if (e.KeyCode == Keys.Escape)
			{
				listView_Filelist.SelectedIndices.Clear();
			}
			else if (e.KeyCode == Keys.Delete)
			{
				Delete();
			}
		}

		private void buttonReload_Click(object sender, EventArgs e)
		{
			treeView_File.Nodes.Clear();
			ButtonRead_Click(sender, e);
		}
	}

    public class CNDFileControl : FileControl
    {
        public CNDFileControl() : base()
        {
        }

        public CNDFileControl(Builder builder) : base(builder)
        {
        }
        public override void SelectPath(string filename)
        {
        }

        public override void Download()
        {
        }
    }
}
