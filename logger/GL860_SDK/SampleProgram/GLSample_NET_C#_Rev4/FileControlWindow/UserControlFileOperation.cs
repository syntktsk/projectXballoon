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
    public partial class UserControlFileOperation : UserControl
    {
        public UserControlFileOperation(bool isLoad = false)
        {
            InitializeComponent();
            if (isLoad) button_Download.Text = "Load";
        }

        private void button_Download_Click(object sender, EventArgs e)
        {
            ((FileControl)this.Parent).Download();
        }

        private void button_Delete_Click(object sender, EventArgs e)
        {
            ((FileControl)this.Parent).Delete();
        }

        private void button_CreateFolder_Click(object sender, EventArgs e)
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
    }
}
