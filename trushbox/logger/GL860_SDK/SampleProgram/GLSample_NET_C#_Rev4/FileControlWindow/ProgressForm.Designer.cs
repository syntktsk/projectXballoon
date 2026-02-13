namespace FileControlWindow
{
    partial class ProgressForm
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
			this.label_AllProgress = new System.Windows.Forms.Label();
			this.label_AllProgressAggregate = new System.Windows.Forms.Label();
			this.label_RemainingTimeLabel = new System.Windows.Forms.Label();
			this.label_RemainTime = new System.Windows.Forms.Label();
			this.progressBar_All = new System.Windows.Forms.ProgressBar();
			this.progressBar_Indivisual = new System.Windows.Forms.ProgressBar();
			this.label_RemainTime2 = new System.Windows.Forms.Label();
			this.label_RemainingTimeLabel2 = new System.Windows.Forms.Label();
			this.label_IndivisualProgress = new System.Windows.Forms.Label();
			this.label_IndivisualProgressLabel = new System.Windows.Forms.Label();
			this.button_Abort = new System.Windows.Forms.Button();
			this.labelResult = new System.Windows.Forms.Label();
			this.SuspendLayout();
			// 
			// label_AllProgress
			// 
			this.label_AllProgress.AutoSize = true;
			this.label_AllProgress.Location = new System.Drawing.Point(12, 19);
			this.label_AllProgress.Name = "label_AllProgress";
			this.label_AllProgress.Size = new System.Drawing.Size(89, 12);
			this.label_AllProgress.TabIndex = 0;
			this.label_AllProgress.Text = "Overall progress";
			// 
			// label_AllProgressAggregate
			// 
			this.label_AllProgressAggregate.AutoSize = true;
			this.label_AllProgressAggregate.Location = new System.Drawing.Point(117, 19);
			this.label_AllProgressAggregate.Name = "label_AllProgressAggregate";
			this.label_AllProgressAggregate.Size = new System.Drawing.Size(25, 12);
			this.label_AllProgressAggregate.TabIndex = 1;
			this.label_AllProgressAggregate.Text = "n/N";
			// 
			// label_RemainingTimeLabel
			// 
			this.label_RemainingTimeLabel.AutoSize = true;
			this.label_RemainingTimeLabel.Location = new System.Drawing.Point(12, 34);
			this.label_RemainingTimeLabel.Name = "label_RemainingTimeLabel";
			this.label_RemainingTimeLabel.Size = new System.Drawing.Size(84, 12);
			this.label_RemainingTimeLabel.TabIndex = 2;
			this.label_RemainingTimeLabel.Text = "Remaining time";
			// 
			// label_RemainTime
			// 
			this.label_RemainTime.AutoSize = true;
			this.label_RemainTime.Location = new System.Drawing.Point(117, 34);
			this.label_RemainTime.Name = "label_RemainTime";
			this.label_RemainTime.Size = new System.Drawing.Size(92, 12);
			this.label_RemainTime.TabIndex = 3;
			this.label_RemainTime.Text = "00:00:00 (*** /S)";
			// 
			// progressBar_All
			// 
			this.progressBar_All.Location = new System.Drawing.Point(14, 49);
			this.progressBar_All.Name = "progressBar_All";
			this.progressBar_All.Size = new System.Drawing.Size(343, 23);
			this.progressBar_All.TabIndex = 4;
			// 
			// progressBar_Indivisual
			// 
			this.progressBar_Indivisual.Location = new System.Drawing.Point(14, 117);
			this.progressBar_Indivisual.Name = "progressBar_Indivisual";
			this.progressBar_Indivisual.Size = new System.Drawing.Size(343, 23);
			this.progressBar_Indivisual.TabIndex = 9;
			// 
			// label_RemainTime2
			// 
			this.label_RemainTime2.AutoSize = true;
			this.label_RemainTime2.Location = new System.Drawing.Point(117, 102);
			this.label_RemainTime2.Name = "label_RemainTime2";
			this.label_RemainTime2.Size = new System.Drawing.Size(92, 12);
			this.label_RemainTime2.TabIndex = 8;
			this.label_RemainTime2.Text = "00:00:00 (*** /S)";
			// 
			// label_RemainingTimeLabel2
			// 
			this.label_RemainingTimeLabel2.AutoSize = true;
			this.label_RemainingTimeLabel2.Location = new System.Drawing.Point(12, 102);
			this.label_RemainingTimeLabel2.Name = "label_RemainingTimeLabel2";
			this.label_RemainingTimeLabel2.Size = new System.Drawing.Size(84, 12);
			this.label_RemainingTimeLabel2.TabIndex = 7;
			this.label_RemainingTimeLabel2.Text = "Remaining time";
			// 
			// label_IndivisualProgress
			// 
			this.label_IndivisualProgress.AutoSize = true;
			this.label_IndivisualProgress.Location = new System.Drawing.Point(117, 87);
			this.label_IndivisualProgress.Name = "label_IndivisualProgress";
			this.label_IndivisualProgress.Size = new System.Drawing.Size(33, 12);
			this.label_IndivisualProgress.TabIndex = 6;
			this.label_IndivisualProgress.Text = "*** %";
			// 
			// label_IndivisualProgressLabel
			// 
			this.label_IndivisualProgressLabel.AutoSize = true;
			this.label_IndivisualProgressLabel.Location = new System.Drawing.Point(12, 87);
			this.label_IndivisualProgressLabel.Name = "label_IndivisualProgressLabel";
			this.label_IndivisualProgressLabel.Size = new System.Drawing.Size(72, 12);
			this.label_IndivisualProgressLabel.TabIndex = 5;
			this.label_IndivisualProgressLabel.Text = "File progress";
			// 
			// button_Abort
			// 
			this.button_Abort.Location = new System.Drawing.Point(152, 177);
			this.button_Abort.Name = "button_Abort";
			this.button_Abort.Size = new System.Drawing.Size(75, 23);
			this.button_Abort.TabIndex = 10;
			this.button_Abort.Text = "Abort";
			this.button_Abort.UseVisualStyleBackColor = true;
			this.button_Abort.Click += new System.EventHandler(this.button_Abort_Click);
			// 
			// labelResult
			// 
			this.labelResult.Location = new System.Drawing.Point(100, 151);
			this.labelResult.Name = "labelResult";
			this.labelResult.Size = new System.Drawing.Size(182, 23);
			this.labelResult.TabIndex = 11;
			this.labelResult.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
			// 
			// ProgressForm
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(374, 212);
			this.ControlBox = false;
			this.Controls.Add(this.labelResult);
			this.Controls.Add(this.button_Abort);
			this.Controls.Add(this.progressBar_Indivisual);
			this.Controls.Add(this.label_RemainTime2);
			this.Controls.Add(this.label_RemainingTimeLabel2);
			this.Controls.Add(this.label_IndivisualProgress);
			this.Controls.Add(this.label_IndivisualProgressLabel);
			this.Controls.Add(this.progressBar_All);
			this.Controls.Add(this.label_RemainTime);
			this.Controls.Add(this.label_RemainingTimeLabel);
			this.Controls.Add(this.label_AllProgressAggregate);
			this.Controls.Add(this.label_AllProgress);
			this.Name = "ProgressForm";
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
			this.Text = "Progress";
			this.Load += new System.EventHandler(this.ProgressForm_Load);
			this.ResumeLayout(false);
			this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label_AllProgress;
        private System.Windows.Forms.Label label_AllProgressAggregate;
        private System.Windows.Forms.Label label_RemainingTimeLabel;
        private System.Windows.Forms.Label label_RemainTime;
        private System.Windows.Forms.ProgressBar progressBar_All;
        private System.Windows.Forms.ProgressBar progressBar_Indivisual;
        private System.Windows.Forms.Label label_RemainTime2;
        private System.Windows.Forms.Label label_RemainingTimeLabel2;
        private System.Windows.Forms.Label label_IndivisualProgress;
        private System.Windows.Forms.Label label_IndivisualProgressLabel;
        private System.Windows.Forms.Button button_Abort;
		private System.Windows.Forms.Label labelResult;
	}
}