// DlgTransSettings.cpp : 実装ファイル
//

#include "stdafx.h"
#include "GLSample.h"
#include "DlgTransSettings.h"

#include "GLSampleDoc.h"
#include "GLSampleView.h"


// CDlgTransSettings ダイアログ

IMPLEMENT_DYNAMIC(CDlgTransSettings, CDialog)

CDlgTransSettings::CDlgTransSettings(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgTransSettings::IDD, pParent)
{

}

CDlgTransSettings::~CDlgTransSettings()
{
}

void CDlgTransSettings::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IPADDRESS, m_IpAddress);
	DDX_Control(pDX, IDC_ED_PORT, m_EdPort);
	DDX_Control(pDX, IDC_ED_MACHINE_ID, m_EdMachineId);
}


BEGIN_MESSAGE_MAP(CDlgTransSettings, CDialog)
	ON_BN_CLICKED(IDOK, &CDlgTransSettings::OnBnClickedOk)
	ON_BN_CLICKED(IDC_RD_TCP, &CDlgTransSettings::OnBnClickedRdTcp)
	ON_BN_CLICKED(IDC_RD_USB, &CDlgTransSettings::OnBnClickedRdUsb)
END_MESSAGE_MAP()


// CDlgTransSettings メッセージ ハンドラ


BOOL CDlgTransSettings::OnInitDialog()
{
	CGLSampleDoc	*pDoc = m_pView->GetDocument();
	CButton			*btTcp, *btUsb;
	CString			str;

	CDialog::OnInitDialog();

	// TODO: この位置に初期化の補足処理を追加してください

	//
	// UIの状態の初期化
	//

	// I/Fラジオボタンの取得
	btTcp = (CButton *)GetDlgItem(IDC_RD_TCP);
	btUsb = (CButton *)GetDlgItem(IDC_RD_USB);

	// 状態初期化
	if (pDoc->m_nIf == SEL_IF_TCP) {
		btTcp->SetCheck(1);
		btUsb->SetCheck(0);
		m_IpAddress.EnableWindow(TRUE);
		m_EdPort.EnableWindow(TRUE);
		m_EdMachineId.EnableWindow(FALSE);
	} else {
		btTcp->SetCheck(0);
		btUsb->SetCheck(1);
		m_IpAddress.EnableWindow(FALSE);
		m_EdPort.EnableWindow(FALSE);
		m_EdMachineId.EnableWindow(TRUE);
	}

	//
	// UIの中身の初期化
	//

	// IPアドレス

	m_IpAddress.SetAddress(pDoc->m_dwIpAddr);

	// Port
	str.Format("%d",pDoc->m_dwPort);
	m_EdPort.SetWindowText(str);


	// 機器ID
	str.Format("%d",pDoc->m_dwMachineId);
	m_EdMachineId.SetWindowText(str);

	return TRUE;

}


//
// OKボタンの処理
//
void CDlgTransSettings::OnBnClickedOk()
{
	// TODO: ここにコントロール通知ハンドラ コードを追加します。
	CGLSampleDoc	*pDoc = m_pView->GetDocument();
	CButton			*btTcp, *btUsb;
	CString			str;

	// I/Fラジオボタンの取得
	btTcp = (CButton *)GetDlgItem(IDC_RD_TCP);
	btUsb = (CButton *)GetDlgItem(IDC_RD_USB);

	//
	// 設定をDocumentに反映する
	//

	// I/Fセレクト
	if (btTcp->GetCheck()) {
		pDoc->m_nIf = SEL_IF_TCP;
	} else {
		pDoc->m_nIf = SEL_IF_USB;
	}

	// IpAddress
	m_IpAddress.GetAddress(pDoc->m_dwIpAddr);

	//Port
	m_EdPort.GetWindowText(str);
	pDoc->m_dwPort = atoi(str);

	// 機器ID
	m_EdMachineId.GetWindowText(str);
	pDoc->m_dwMachineId = atoi(str);

	OnOK();
}

//
// TCP/IPラジオボタンが選択された時の処理
//
void CDlgTransSettings::OnBnClickedRdTcp()
{
	// TODO: ここにコントロール通知ハンドラ コードを追加します。

	m_IpAddress.EnableWindow(TRUE);
	m_EdPort.EnableWindow(TRUE);
	m_EdMachineId.EnableWindow(FALSE);
}

//
// USBラジオボタンが選択された時の処理
//

void CDlgTransSettings::OnBnClickedRdUsb()
{
	// TODO: ここにコントロール通知ハンドラ コードを追加します。

	m_IpAddress.EnableWindow(FALSE);
	m_EdPort.EnableWindow(FALSE);
	m_EdMachineId.EnableWindow(TRUE);
}
