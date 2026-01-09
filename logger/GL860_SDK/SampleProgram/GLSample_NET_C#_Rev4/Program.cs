using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;

//Use declaration of DevIo.Net [[DevIo.Net の利用宣言]]
using GtcDevIo;	

namespace DevIo.Net_SampleProg
{
	static class Program
	{
		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		[STAThread]
		static void Main()
		{
			Application.EnableVisualStyles();
			Application.SetCompatibleTextRenderingDefault(false);
			Application.Run(new SampleApp());
		}
	}
}
