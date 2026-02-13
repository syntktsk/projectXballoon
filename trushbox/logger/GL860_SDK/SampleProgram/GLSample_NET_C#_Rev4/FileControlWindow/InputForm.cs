using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace FileControlWindow
{
    public partial class InputForm : Form
    {
		public string FileName { get; set; } = "";
        public FileExtType? FileExtType { get; set; }

        public InputForm()
        {
            InitializeComponent();
        }

        private void InputForm_Load(object sender, EventArgs e)
        {
			if (FileExtType == null) return;
			else if (FileName != "") textBox_Input.Text = FileName;
			else if (FileExtType == FileControlWindow.FileExtType.GBD) textBox_Input.Text = "DEFAULT.GBD";
			else if (FileExtType == FileControlWindow.FileExtType.CSV) textBox_Input.Text = "DEFAULT.CSV";
        }

        private void button_OK_Click(object sender, EventArgs e)
        {
            if (CheckFileNameCharactor(this.textBox_Input.Text) == false)
            {
                MessageBox.Show("You have entered characters that cannot be used.\r\n \\/:,;*?\"<>|");
                return;
            }
            DialogResult = DialogResult.OK;
            FileName = this.textBox_Input.Text;
            this.Close();

        }

        private void button_Cancel_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.Cancel;
            this.Close();
        }

        private bool CheckFileNameCharactor(string filename)
        {
            char[] ng = { '\\', '/', ':', ',', ';', '*', '?', '"', '<', '>', '|' };
            var ret = filename.IndexOfAny(ng);
            return (ret == -1 ? true : false);
        }

    }
}
