#pragma once
#include "afxcmn.h"
#include "afxwin.h"

#include "GLSampleDoc.h"
#include "GLSampleView.h"


// CDlgTransSettings ダイアログ

class CDlgTransSettings : public CDialog
{
	DECLARE_DYNAMIC(CDlgTransSettings)

public:
	CDlgTransSettings(CWnd* pParent = NULL);   // 標準コンストラクタ
	virtual ~CDlgTransSettings();

// アトリビュート
public:
	CGLSampleView	*m_pView;	// 親のView


	// ダイアログ データ
	enum { IDD = IDD_TRANS_SETTINGS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	virtual BOOL OnInitDialog();


	DECLARE_MESSAGE_MAP()
public:
	CIPAddressCtrl m_IpAddress;
	CEdit m_EdPort;
	CEdit m_EdMachineId;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedRdTcp();
	afx_msg void OnBnClickedRdUsb();
};
