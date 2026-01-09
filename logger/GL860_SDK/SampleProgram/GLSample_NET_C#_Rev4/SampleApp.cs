using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using FileControlWindow;

//Use declaration of DevIo.Net [[DevIo.Net の利用宣言]]
using GtcDevIo;
using GtcDevIoSampleApp;

namespace DevIo.Net_SampleProg
{

	public partial class SampleApp : Form
	{
		//Class instantiate of DevIo.Net (Only once is carried out at the time of an application start.) [[DevIo.Net のクラスインスタンス生成(アプリ起動時に一回だけ実行)]]
		private GtcDevIo.DevIo m_DevIo = new GtcDevIo.DevIo();
		stDeviceList TcpParam = new stDeviceList();
		CommandHistory CommandHistory { get; set; } = new CommandHistory();

		//>>>>　Event processing [[イベント処理]]
		public SampleApp()
		{
			InitializeComponent();
			richTextBox.Text = "";
			button_Connect.Text = "Connect";

			//An original value of a connection parameter is set. [[接続パラメータの初期値をセット]]
			TcpParam.IfType = enIfType.IF_USB;
			TcpParam.Tcp_IpAdd = new IpAddr();
			TcpParam.Port = 8023;
			TcpParam.Tcp_IpAdd.ipAddr = TcpParam.Tcp_IpAdd.ConvertInt4ToInt(192, 168, 0, 1);
		}

		private void SampleApp_Load(object sender, EventArgs e)
		{
			buttonFile.Enabled = false;
			InitComboBoxCommand();  // Initializing the combo box control
		}

		private void InitComboBoxCommand()
		{
			comboBoxCommand.Enabled = false;
			comboBoxCommand.Items.AddRange(Command.CommandList.ToArray());
		}

		private void SampleProg_FormClosed(object sender, FormClosedEventArgs e)
		{
			//By the timing by which the form closes, DevIo, close [[フォームが閉じるタイミングでDevIoも閉じる]]
			m_DevIo.Close();
		}

		private void richTextBox_KeyPress(object sender, KeyPressEventArgs e)
		{
			if (e.KeyChar == (char)Keys.Enter)
			{
				//[enter] when pressing a key, a party is taken out from the last line of a rich text. [[[enter]キーを押した際にリッチテキストの最終行から一行取り出す]]
				var Tmp = GetRichText();

				if (m_DevIo.isOpen() == true)
				{
					SetRichText(SendQueryDevice(Tmp.TrimEnd() + Environment.NewLine, true));
				}
			}
		}

		public void richTextBox_KeyDown(object sender, KeyEventArgs e)
		{
			string Str;
			//Basic action of key ups and downs is made more invalid. [[キーアップダウンの基本動作を無効化する]]
			switch(e.KeyCode)
			{
				case Keys.Up:	//↑
				{
					if (m_DevIo.isOpen() == true)
					{
						Str = CommandHistory.NextHistory();
						ModifyRichText(Str,false);
					}

					//Movement is invalidated. [[動作を無効にする]]
					e.Handled = true;
					break;
				}
				case Keys.Down:	//↓
				{
					if (m_DevIo.isOpen() == true)
					{
						Str = CommandHistory.PrevHistory();
						ModifyRichText(Str,false);
					}

					//Movement is invalidated. [[動作を無効にする]]
					e.Handled = true;
					break;
				}
				default:
				{
					break;
				}
			}
		}

		private void button_Connect_Click(object sender, EventArgs e)
		{
			if (m_DevIo.isOpen() == false)
			{
				button_Connect.Text = "Connecting";

				ConnectForm frm = new ConnectForm();

				frm.SetTcpData(TcpParam);
				frm.ShowDialog();	//The connection form is opened modally. [[接続フォームをモーダルでオープン]]
				TcpParam = frm.GetTcpData();

				if (TcpParam.ConnectFlag == true)
				{
					stConnectParam ConnectParam = frm.GetParam();

					//Generation of a parameter [[パラメータの生成]]
					Param param = new Param();

					//Device connection [[デバイスと接続]]
					stDeviceList List = ConnectParam.DeviceList;

					param.IpAddr = null;
					if (ConnectParam.IfType == enIfType.IF_UDP)
					{
						//A UDP connection is processed as a TCP connection finally. [[UDP接続は最終的にTCP接続として処理されます]]
						ConnectParam.IfType = enIfType.IF_TCP;
						param.IpAddr = List.Udp_IpAdd;
					}
					else if (ConnectParam.IfType == enIfType.IF_TCP)
					{
						param.IpAddr = List.Tcp_IpAdd;
					}
					m_DevIo.Create(ConnectParam.IfType);
					m_DevIo.SetBinaryCrPoint(5);

					param.UsbId.usbId = List.Usb_Id;
					param.ComId.ComParam.portName = List.Com_Id;
					param.PortNumber = (uint)List.Port;

					bool Flag = m_DevIo.Open(param);
					if (Flag == false)
					{
						button_Connect.Text = "Error";
					}
					else
					{
						//Information acquisition [[情報取得]]
						string s;
						s = m_DevIo.GetClassName();
						s = m_DevIo.SendQuery("*IDN?");

						SetRichText(s);

						button_Connect.Text = "DisConnect";
						comboBoxCommand.Enabled = true;
						buttonFile.Enabled = true;
					}
				}
				else
				{
					button_Connect.Text = "Connect";
				}
			}
			else
			{
				button_Connect.Text = "DisConnecting";
				m_DevIo.Close();
				ClearRichText();
				button_Connect.Text = "Connect";
				comboBoxCommand.Enabled = false;
				buttonFile.Enabled = false;
			}
		}

		/// <summary>
		/// UDP Button click Event [[UDPボタンクリックイベント]]
		/// </summary>
		/// <param name="sender"></param>
		/// <param name="e"></param>
		private void button_Udp_Click(object sender, EventArgs e)
		{
			GtcDevIo.DevIo m_DevIoUdp = new GtcDevIo.DevIo();
			//Param param = new Param();
			m_DevIoUdp.Create(enIfType.IF_UDP);
			stDeviceList[] List = m_DevIoUdp.GetDeviceList(8023,"");
			DevIoAbs abs = new DevIoAbs();
			int Count = List.Length;
			int Add = 0;
			for (int i = 0; i < Count; i++)
			{
				string str = BinToStrHex(Add,List[i].Udp_Header, List[i].Udp_Header.Length, 16);
				Add += List[i].Udp_Header.Length;
				SetRichText(str);

				var res = PacketSearch.GetAnalyzedPacket(List[i].Udp_Header);

				SetRichText("modelname = " + res.modelname.TrimEnd());
				SetRichText("firmversion = " + res.firmversion.TrimEnd());
				SetRichText("suffix = " + res.suffix.TrimEnd());
				SetRichText("hostname = " + res.hostname.TrimEnd());

				SetRichText(string.Format("ipaddress = {0},{1},{2},{3}",
					(res.ipaddress >>(0*8)) & 255,
					(res.ipaddress >>(1*8)) & 255,
					(res.ipaddress >>(2*8)) & 255,
					(res.ipaddress >>(3*8)) & 255));

				this.Refresh();
			}

		}


		private void button_Clr_Click(object sender, EventArgs e)
		{
			ClearRichText();
		}

		/// <summary>
		/// Binary data is converted to a text. [[バイナリデータをテキストに変換]]
		/// </summary>
		/// <param name="BinaryData"></param>
		/// <param name="BinarySize"></param>
		/// <param name="crPoint"></param>
		/// <returns></returns>
		static string BinToStrHex(int Add, byte[] BinaryData, int BinarySize, int crPoint = 10)
		{
			string str = "";
			int k = 0;
			int a = 0;
			string ss;
			str += "\r\n";

			string ascii="";
			for (int i = 0; i < BinarySize; i += 1)
			{
				byte b = BinaryData[i];
				short s = (short)b;

				if (b<32 || b>=127)
				{
					ascii += ".";
				}
				else
				{
					ascii += (Char)b;
				}

				if (k != 0)
				{
					str += " ";
				}
				else
				{
					ss = string.Format("{0,0:X8}: ", a + Add);
					str += ss;
				}

				ss = string.Format("{0,0:X2}", s);
				str += ss;

				a++;
				k++;
				if (k == crPoint)	//In cr, carriage return [[crで改行]]
				{
					k = 0;
					str += " : " + ascii + "\r\n";
					ascii = "";
				}
			}
			return str;
		}


		//<<<<

		//>>>> The closed additional function [[非公開の追加関数]]
		private void ClearRichText()
		{
			richTextBox.Clear();
			richTextBox.Focus();
		}

		private void SetRichText(string Text,bool CrFlag=true)
		{
			richTextBox.AppendText(Text.TrimEnd());
			if (CrFlag == true)
			{
				//New paragraph giving [[改行付与]]
				richTextBox.AppendText(Environment.NewLine);
			}
			richTextBox.SelectionStart = richTextBox.Text.Length;
			richTextBox.Focus();
		}

		private void ModifyRichText(string Text, bool CrFlag = true)
		{
			var len = richTextBox.Lines.Length;
			var lines = new List<string>(richTextBox.Lines);
			lines.RemoveAt(len - 1);
			lines.Add(Text + ((CrFlag == true) ? Environment.NewLine : ""));
			richTextBox.Text = String.Join(Environment.NewLine, lines);
			richTextBox.SelectionStart = richTextBox.Text.Length;
			//richTextBox.ScrollToCaret();
			richTextBox.Focus();
		}

		private string GetRichText()
		{
			//A party is taken out from the last line of a rich text. [[リッチテキストの最終行から一行取り出す]]
			string Str = richTextBox.Text;

			//The last line feed code is excluded. [[ラストの改行コードを排除する]]
			int iFind1 = Str.LastIndexOf('\n');
			if (iFind1 == -1) iFind1 = 0;
			int size = Str.Length;
			string Tmp = Str.Substring(0, iFind1);

			//A text from the new paragraph location one before to the last is taken out. [[ひとつ前の改行位置から最終までのテキストを取り出す]]
			iFind1 = Tmp.LastIndexOf('\n');
			if (iFind1 == -1)
			{
				iFind1 = 0;
			}
			else
			{
				iFind1++;
			}
			size = Tmp.Length;
			Tmp = Tmp.Substring(iFind1, size - iFind1).TrimEnd();

			return Tmp;
		}

		private void comboBoxCommand_SelectedIndexChanged(object sender, EventArgs e)
		{
			var comboBox = (ComboBox)sender;
			try
			{
				var ret = ((Command)comboBox.SelectedItem).Exec(SendQueryDevice);
				SetRichText(ret);
			}
			catch { }
		}

		public string SendQueryDevice(string command, bool enlog)
		{
			var ret = m_DevIo?.SendQuery(command);
			if (enlog == true) CommandHistory.PushHistory(command);
			return ret;
		}

		private void buttonFile_Click(object sender, EventArgs e)
		{
			this.Cursor = Cursors.WaitCursor;
			var fileform = FileControl.Builder.Instance.SetDevIo(m_DevIo).SetFormType(FileFormType.Opeartion).Build();
			fileform.ShowDialog();
			this.Cursor = Cursors.Default;
		}
	}
}
