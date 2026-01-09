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
    public partial class ProgressForm : Form
    {
        public FileAction FileAction { get; set; }
        public string DestinationPath { get; set; }

        public ProgressForm()
        {
            InitializeComponent();
        }

        private void ProgressForm_Load(object sender, EventArgs e)
        {
            Task.Run(() => FileAction?.Exec(DestinationPath, this.Update));
        }

        public class UpdateParam
        {
            public bool endFlg = false;
            public bool errorFlg = false;

            public int fileCount;
            public int totalFileCount;

            public Int64 transAllBytes;
            public Int64 totalAllBytes;

            public Int64 transFileBytes;
            public Int64 totalFileBytes;

            public double bps = 1;

            public DateTime startTime;
            public DateTime startFileTime;
        }

        private bool abortFlg;

        /// <summary>
        /// Update progress form by passing update information as an argument
        /// </summary>
        /// <param name="param"></param>
        /// <returns></returns>
        public bool Update(UpdateParam param)
        {
            this.Invoke((Action)(() =>
            {
                // Common
                // Total
                label_AllProgressAggregate.Text = $"{param.fileCount} / {param.totalFileCount}";
                progressBar_All.Maximum = param.totalFileCount;
                progressBar_All.Minimum = 0;
                progressBar_All.Value = param.fileCount;
                var span = DateTime.Now - param.startTime;
                var remtick = (param.bps == 0) ? 0 : (double)(param.totalAllBytes - param.transAllBytes) / param.bps;
                TimeSpan remspan = TimeSpan.FromSeconds(remtick);
                label_RemainTime.Text = $"{span.Hours:00}:{span.Minutes:00}:{span.Seconds:00} / {remspan.Hours:00}:{remspan.Minutes:00}:{remspan.Seconds:00}";

                // Indivisual
                label_IndivisualProgress.Text = $"{FileControl.getFileSize(param.transFileBytes)} / {FileControl.getFileSize(param.totalFileBytes)}  ({Math.Round(100.0f * param.transFileBytes / param.totalFileBytes, 2)}%)";
                span = DateTime.Now - param.startFileTime;
                param.bps = (double)param.transFileBytes / span.TotalSeconds;

                remtick = (param.bps == 0) ? 0 : (double)(param.totalFileBytes - param.transFileBytes) / param.bps;
                remspan = TimeSpan.FromSeconds(remtick);
                label_RemainTime2.Text = $"{span.Hours:00}:{span.Minutes:00}:{span.Seconds:00} / {remspan.Hours:00}:{remspan.Minutes:00}:{remspan.Seconds:00}  ({ FileControl.getFileSize((long)param.bps)}/s)";
                progressBar_Indivisual.Maximum = (int)param.totalFileBytes;
                progressBar_Indivisual.Minimum = 0;
                progressBar_Indivisual.Value = (int)param.transFileBytes;

                labelResult.Text = "Dawnloading....";

                if (abortFlg == true || param.endFlg == true)
                {
                    if (param.errorFlg == true) labelResult.Text = "Download failed.";
                    else if (abortFlg == true) labelResult.Text = "Dawnload aborted.";
                    else labelResult.Text = "Download completed.";

                    abortFlg = true;
                    button_Abort.Text = "Close";
                }
            }));

            return abortFlg;
        }

        private void button_Abort_Click(object sender, EventArgs e)
        {
            if (abortFlg == true)
            {
                this.Close();
                return;
            }
            abortFlg = true;
        }
    }
}
