namespace FileControlWindow
{
    partial class InputForm
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
			this.label_Input = new System.Windows.Forms.Label();
			this.textBox_Input = new System.Windows.Forms.TextBox();
			this.button_OK = new System.Windows.Forms.Button();
			this.button_Cancel = new System.Windows.Forms.Button();
			this.SuspendLayout();
			// 
			// label_Input
			// 
			this.label_Input.AutoSize = true;
			this.label_Input.Location = new System.Drawing.Point(13, 13);
			this.label_Input.Name = "label_Input";
			this.label_Input.Size = new System.Drawing.Size(30, 12);
			this.label_Input.TabIndex = 0;
			this.label_Input.Text = "Input";
			// 
			// textBox_Input
			// 
			this.textBox_Input.Location = new System.Drawing.Point(15, 30);
			this.textBox_Input.Name = "textBox_Input";
			this.textBox_Input.Size = new System.Drawing.Size(252, 19);
			this.textBox_Input.TabIndex = 1;
			// 
			// button_OK
			// 
			this.button_OK.Location = new System.Drawing.Point(64, 70);
			this.button_OK.Name = "button_OK";
			this.button_OK.Size = new System.Drawing.Size(75, 23);
			this.button_OK.TabIndex = 2;
			this.button_OK.Text = "OK";
			this.button_OK.UseVisualStyleBackColor = true;
			this.button_OK.Click += new System.EventHandler(this.button_OK_Click);
			// 
			// button_Cancel
			// 
			this.button_Cancel.Location = new System.Drawing.Point(145, 70);
			this.button_Cancel.Name = "button_Cancel";
			this.button_Cancel.Size = new System.Drawing.Size(75, 23);
			this.button_Cancel.TabIndex = 3;
			this.button_Cancel.Text = "Cancel";
			this.button_Cancel.UseVisualStyleBackColor = true;
			this.button_Cancel.Click += new System.EventHandler(this.button_Cancel_Click);
			// 
			// InputForm
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(279, 105);
			this.ControlBox = false;
			this.Controls.Add(this.button_Cancel);
			this.Controls.Add(this.button_OK);
			this.Controls.Add(this.textBox_Input);
			this.Controls.Add(this.label_Input);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
			this.Name = "InputForm";
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
			this.Load += new System.EventHandler(this.InputForm_Load);
			this.ResumeLayout(false);
			this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label_Input;
        private System.Windows.Forms.TextBox textBox_Input;
        private System.Windows.Forms.Button button_OK;
        private System.Windows.Forms.Button button_Cancel;
    }
}