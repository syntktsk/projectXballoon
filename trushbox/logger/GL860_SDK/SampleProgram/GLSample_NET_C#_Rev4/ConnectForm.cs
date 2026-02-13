using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

//Use declaration of DevIo.Net [[DevIo.Net の利用宣言]]
using GtcDevIo;

namespace DevIo.Net_SampleProg
{
	public partial class ConnectForm : Form
	{
		//Preparation of a setting parameter [[設定パラメータの用意]]
		stConnectParam ConnectParam = new stConnectParam();

		//Class instantiate of DevIo.Net [[DevIo.Net のクラスインスタンス生成]]
		GtcDevIo.DevIo m_DevIo = new GtcDevIo.DevIo();
		Param param = new Param();

		//>>>>　Event processing [[イベント処理]]
		public ConnectForm()
		{
			InitializeComponent();
		}

		private void ConnectForm_Load(object sender, EventArgs e)
		{
			ConnectParam.IfType = (enIfType)GtcDevIoSampleApp.Properties.Settings.Default.ConnectionSelection;
			switch(ConnectParam.IfType)
			{
				case enIfType.IF_USB: radioButton_Usb.Checked = true; break;
				case enIfType.IF_TCP:
					{
						radioButton_TCP.Checked = true;
						var ip = GtcDevIoSampleApp.Properties.Settings.Default.IP;
						var ipp = ip.Split('.');
						if (ipp.Length == 4)
						{
							textBox_IP1.Text = ipp[0];
							textBox_IP2.Text = ipp[1];
							textBox_IP3.Text = ipp[2];
							textBox_IP4.Text = ipp[3];
						}
						var port = GtcDevIoSampleApp.Properties.Settings.Default.Port;
						textBox_Port.Text = port;
						break;
					}
				case enIfType.IF_COM: radioButton_Com.Checked = true; break;
				case enIfType.IF_UDP: radioButton_Udp.Checked = true; break;
			}

		}


		private void ConnectForm_FormClosed(object sender, FormClosedEventArgs e)
		{
			GtcDevIoSampleApp.Properties.Settings.Default.ConnectionSelection = (int)ConnectParam.IfType;
			GtcDevIoSampleApp.Properties.Settings.Default.IP = $"{textBox_IP1.Text}.{textBox_IP2.Text}.{textBox_IP3.Text}.{textBox_IP4.Text}";
			GtcDevIoSampleApp.Properties.Settings.Default.Port = textBox_Port.Text;
			GtcDevIoSampleApp.Properties.Settings.Default.Save();
			m_DevIo.Close();
		}

		private void radioButton_Usb_CheckedChanged(object sender, EventArgs e)
		{
			CheckUsb();
		}

		private void radioButton_Com_CheckedChanged(object sender, EventArgs e)
		{
			CheckCom();
		}

		private void radioButton_Udp_CheckedChanged(object sender, EventArgs e)
		{
			CheckUdp();
		}

		private void radioButton_TCP_CheckedChanged(object sender, EventArgs e)
		{
			CheckTcp();
		}

		private void CheckUsb()
		{
			comboBox_DeviceSelect.Enabled = true;
			button_search.Enabled = true;
			textBox_Port.Enabled = false;
			textBox_IP1.Enabled = false;
			textBox_IP2.Enabled = false;
			textBox_IP3.Enabled = false;
			textBox_IP4.Enabled = false;

			ConnectParam.IfType = enIfType.IF_USB;
			Device_Search();
		}
		private void CheckTcp()
		{
			button_Connect.Enabled = true;
			comboBox_DeviceSelect.Enabled = false;
			button_search.Enabled = false;
			textBox_Port.Enabled = true;
			textBox_IP1.Enabled = true;
			textBox_IP2.Enabled = true;
			textBox_IP3.Enabled = true;
			textBox_IP4.Enabled = true;
			comboBox_DeviceSelect.Items.Clear();

			ConnectParam.IfType = enIfType.IF_TCP;
			Set_TcpDeviceList();
		}

		private void CheckCom()
		{
			comboBox_DeviceSelect.Enabled = true;
			button_search.Enabled = true;
			textBox_Port.Enabled = false;
			textBox_IP1.Enabled = false;
			textBox_IP2.Enabled = false;
			textBox_IP3.Enabled = false;
			textBox_IP4.Enabled = false;

			ConnectParam.IfType = enIfType.IF_COM;
			Device_Search();
		}

		private void CheckUdp()
		{
			comboBox_DeviceSelect.Enabled = true;
			button_search.Enabled = true;
			textBox_Port.Enabled = true;
			textBox_IP1.Enabled = false;
			textBox_IP2.Enabled = false;
			textBox_IP3.Enabled = false;
			textBox_IP4.Enabled = false;

			ConnectParam.IfType = enIfType.IF_UDP;
			Device_Search();
		}



		private void textBox_Port_KeyPress(object sender, KeyPressEventArgs e)
		{
			//0-9 and at all except for a backspace cancel the event. [[0～9と、バックスペース以外の時は、イベントをキャンセルする]]
			if ((e.KeyChar < '0' || '9' < e.KeyChar) && e.KeyChar != '\b')
			{
				e.Handled = true;
			}
			SearchUdp();
			Set_TcpDeviceList();
		}

		private void textBox_IP_KeyPress(TextBox NextTextBox, object sender, KeyPressEventArgs e)
		{
			//If '.' is input, I move to next TextBox. [['.'が入力されたら、次のTextBoxに移動する]]
			if (e.KeyChar == '.')
			{
				NextTextBox.Focus();
				e.Handled = true;
			}
			else
			{
				//0-9 and at all except for a backspace cancel the event. [[0～9と、バックスペース以外の時は、イベントをキャンセルする]]
				if ((e.KeyChar < '0' || '9' < e.KeyChar) && e.KeyChar != '\b')
				{
					e.Handled = true;
				}
			}
			Set_TcpDeviceList();
		}

		private void textBox_IP1_KeyPress(object sender, KeyPressEventArgs e)
		{
			textBox_IP_KeyPress(textBox_IP2, sender, e);
		}

		private void textBox_IP2_KeyPress(object sender, KeyPressEventArgs e)
		{
			textBox_IP_KeyPress(textBox_IP3, sender, e);
		}

		private void textBox_IP3_KeyPress(object sender, KeyPressEventArgs e)
		{
			textBox_IP_KeyPress(textBox_IP4, sender, e);
		}

		private void textBox_IP4_KeyPress(object sender, KeyPressEventArgs e)
		{
			textBox_IP_KeyPress(textBox_IP1, sender, e);
		}

		private void button_Connect_Click(object sender, EventArgs e)
		{
			Set_TcpDeviceList(true);	//We assume that data is effective. [[データを有効とする]]

			this.Close();
		}

		private void button_search_Click(object sender, EventArgs e)
		{
			Device_Search();
		}

		private void comboBox_DeviceSelect_SelectedIndexChanged(object sender, EventArgs e)
		{
			SetDeviceList();
		}
		//<<<<


		//>>>> The additional function which is being exhibited in outside [[外部に公開している追加関数]]
		public void SetTcpData(stDeviceList Param)
		{
			textBox_Port.Text = Param.Port.ToString();
			textBox_IP1.Text = (Param.Tcp_IpAdd.ipAddr & 255).ToString();
			textBox_IP2.Text = (Param.Tcp_IpAdd.ipAddr >> 8 & 255).ToString();
			textBox_IP3.Text = (Param.Tcp_IpAdd.ipAddr >> 16 & 255).ToString();
			textBox_IP4.Text = (Param.Tcp_IpAdd.ipAddr >> 24 & 255).ToString();

			switch (Param.IfType)
			{
				case enIfType.IF_COM:
					{
						this.radioButton_Com.Select(); break;
					}
				case enIfType.IF_TCP:
					{
						this.radioButton_TCP.Select(); break;
					}
				case enIfType.IF_UDP:
					{
						this.radioButton_Udp.Select(); break;
					}
				case enIfType.IF_USB:
					{
						this.radioButton_Usb.Select(); break;
					}
			}
		}

		public stDeviceList GetTcpData()
		{
			stDeviceList ret = new stDeviceList();
			ret.Port = stoi(textBox_Port.Text);
			ret.Tcp_IpAdd = new IpAddr();
			ret.Tcp_IpAdd.ipAddr = ret.Tcp_IpAdd.ConvertInt4ToInt(
				stou(textBox_IP1.Text),
				stou(textBox_IP2.Text),
				stou(textBox_IP3.Text),
				stou(textBox_IP4.Text)
				);
			ret.IfType = ConnectParam.IfType;
			ret.ConnectFlag = ConnectParam.DeviceList.ConnectFlag;
			return ret;
		}

		public stConnectParam GetParam()
		{
			return ConnectParam;
		}

		//<<<<

		//>>>> The closed additional function [[非公開の追加関数]]
		private void Device_Search()
		{
			this.comboBox_DeviceSelect.Enabled = false;
			this.button_search.Enabled = false;
			this.Refresh();
			switch (ConnectParam.IfType)
			{
				case enIfType.IF_USB:
					{
						SearchUsb();
						break;
					}
				case enIfType.IF_COM:
					{
						SearchCom();
						break;
					}
				case enIfType.IF_UDP:
					{
						SearchUdp();
						break;
					}
			}
			this.comboBox_DeviceSelect.Enabled = true;
			this.button_search.Enabled = true;

			if (comboBox_DeviceSelect.Items.Count == 0)
			{
				this.button_Connect.Enabled = false;
			}
			else
			{
				this.button_Connect.Enabled = true;
			}
		}
	
		private int stoi(string data)
		{
			int ret = 0;
			if (data.Length != 0)
			{
				ret = Convert.ToInt32(data);
			}
			return ret;
		}

		private uint stou(string data)
		{
			uint ret = 0;
			if (data.Length != 0)
			{
				ret = Convert.ToUInt32(data);
			}
			return ret;
		}
		private void SearchUsb()
		{
			//Device and connection [[デバイスと接続]]
			m_DevIo.Create(ConnectParam.IfType);
			stDeviceList[] List = m_DevIo.GetDeviceList();
			int Count = List.Length;
			comboBox_DeviceSelect.Items.Clear();

			ComboBoxItem item;
			for (int i = 0; i < Count; i++)
			{
				item = new ComboBoxItem();

				item.Text = List[i].SelectName;
				item.DeviceList = List[i];
				int Index = comboBox_DeviceSelect.Items.Add(item);
			}
			if (comboBox_DeviceSelect.Items.Count != 0)
			{
				comboBox_DeviceSelect.SelectedIndex = 0;	//The first choices are chosen by default. [[デフォルトで最初の選択肢を選んでおく]]
				comboBox_DeviceSelect.Refresh();
			}
			SetDeviceList();
		}

		private void SearchCom()
		{
			//Device and connection [[デバイスと接続]]
			m_DevIo.Create(ConnectParam.IfType);
			stDeviceList[] List = m_DevIo.GetDeviceList();
			int Count = List.Length;
			comboBox_DeviceSelect.Items.Clear();

			ComboBoxItem item;
			for (int i = 0; i < Count; i++)
			{
				item = new ComboBoxItem();
				item.Text = List[i].SelectName;
				item.DeviceList = List[i];

				int Index = comboBox_DeviceSelect.Items.Add(item);
			}
			if (comboBox_DeviceSelect.Items.Count != 0)
			{
				comboBox_DeviceSelect.SelectedIndex = 0;	//The first choices are chosen by default. [[デフォルトで最初の選択肢を選んでおく]]
			}
			SetDeviceList();
		}

		private void SearchUdp()
		{
			//Device and connection [[デバイスと接続]]
			m_DevIo.Create(ConnectParam.IfType);
			stDeviceList[] List = m_DevIo.GetDeviceList( Convert.ToUInt32(textBox_Port.Text) );
			int Count = List.Length;
			comboBox_DeviceSelect.Items.Clear();

			ComboBoxItem item;
			for (int i = 0; i < Count; i++)
			{
				item = new ComboBoxItem();
				item.Text = List[i].SelectName;
				item.DeviceList = List[i];

				int Index = comboBox_DeviceSelect.Items.Add(item);
			}
			if (comboBox_DeviceSelect.Items.Count != 0)
			{
				comboBox_DeviceSelect.SelectedIndex = 0;	//The first choices are chosen by default. [[デフォルトで最初の選択肢を選んでおく]]
			}
			SetDeviceList();
			comboBox_DeviceSelect.Refresh();
		}

		private void SetDeviceList()
		{
			ComboBoxItem Item = (ComboBoxItem)comboBox_DeviceSelect.SelectedItem;
			if (Item != null)
			{
				ConnectParam.DeviceList = Item.DeviceList;
			}
			else
			{
				stDeviceList List = new stDeviceList();
				List.ConnectFlag = false;
				ConnectParam.DeviceList = List;
			}
		}

		private void Set_TcpDeviceList(bool ConnectFlag = false)
		{
			stDeviceList List = new stDeviceList();
			List = ConnectParam.DeviceList;
			List.IfType = ConnectParam.IfType;

			List.Tcp_IpAdd = new IpAddr();
			List.Tcp_IpAdd.ipAddr = List.Tcp_IpAdd.ConvertInt4ToInt(
				(uint)stoi(textBox_IP1.Text),
				(uint)stoi(textBox_IP2.Text),
				(uint)stoi(textBox_IP3.Text),
				(uint)stoi(textBox_IP4.Text)
				);
			List.Port = stoi(textBox_Port.Text);
			List.ConnectFlag = ConnectFlag;

			if (List.IfType != enIfType.IF_UDP)
			{
				List.Udp_IpAdd = null;
			}
			if (List.IfType != enIfType.IF_TCP)
			{
				if (List.SelectName == null)
				{
					List.ConnectFlag = false;
				}
				else
				{
					if (List.SelectName == "")
					{
						List.ConnectFlag = false;
					}
				}
			}

			ConnectParam.DeviceList = List;
		}

	}

	public struct stConnectParam
	{
		public enIfType IfType;
		public stDeviceList DeviceList;
	}

	public class ComboBoxItem
	{
		public string Text;
		public stDeviceList DeviceList;

		public override string ToString()
		{
			return Text;
		}
	}
}
