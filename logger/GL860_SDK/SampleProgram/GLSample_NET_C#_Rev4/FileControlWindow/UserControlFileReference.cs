using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace FileControlWindow
{
    public partial class UserControlFileReference : UserControl
    {
        public FileNameType? FileNameType { get; set; }
        public string FileName { get; set; }

        public UserControlFileReference()
        {
            InitializeComponent();
        }

		public void SetFileName(string filename)
		{
			FileName = filename;
			textBoxFileName.Text = FileName;
		}

        private void UserControlFileReference_Load(object sender, EventArgs e)
        {
            FileNameType = (Parent as FileControl)?.FileNameType;
            InitControl();
        }

        private void InitControl()
        {
            switch (FileNameType)
            {
                case FileControlWindow.FileNameType.Auto:
                    textBoxFileName.Visible = false;
                    buttonInputFile.Enabled = false;
                    break;
                case FileControlWindow.FileNameType.Manual:
                    textBoxFileName.Visible = true;
                    buttonInputFile.Enabled = true;
                    break;
            }

        }

        private void buttonCreateFolder_Click(object sender, EventArgs e)
        {
            using (var InputForm = new InputForm())
            {
                var ret = InputForm.ShowDialog();
                if (ret == DialogResult.OK)
                {
                    var filename = InputForm.FileName;
                    var parent = (FileControl)Parent;
                    parent.CreateDirectory(filename);
                }
            }
        }

        private void buttonInputFile_Click(object sender, EventArgs e)
        {
            using (var InputForm = new InputForm())
            {
                InputForm.FileExtType = (Parent as FileControl).FileExtType;
				InputForm.FileName = this.FileName;
				var ret = InputForm.ShowDialog();
                if (ret == DialogResult.OK)
                {
                    FileName = InputForm.FileName;
                    textBoxFileName.Text = FileName;
                }
            }
        }

        private void buttonDelete_Click(object sender, EventArgs e)
        {
            (Parent as FileControl)?.Delete();
        }

        private void buttonOk_Click(object sender, EventArgs e)
        {
            (Parent as FileControl)?.SelectPath(FileName);
        }
    }
}
