// GLSampleView.h : CGLSampleView クラスのインターフェイス
//


#pragma once
#include "afxwin.h"


class CGLSampleView : public CFormView
{
protected: // シリアル化からのみ作成します。
	CGLSampleView();
	DECLARE_DYNCREATE(CGLSampleView)

public:
	enum{ IDD = IDD_GLSAMPLE_FORM };

// 属性
public:
	CGLSampleDoc* GetDocument() const;

// 操作
public:

// オーバーライド
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	virtual void OnInitialUpdate(); // 構築後に初めて呼び出されます。

// 実装
public:
	virtual ~CGLSampleView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif


	void AddLog(CString &str);
	void AddLog(LPCSTR str);
	void UpdateLog(CString &str);
	void UpdateLog(LPCSTR str);



protected:

// 生成された、メッセージ割り当て関数
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedTrnsSettings();
	afx_msg void OnBnClickedBtStart();
	afx_msg void OnBnClickedBtStop();
	CButton m_BtStart;
	CButton m_BtStop;
	CEdit m_EdStat;
};

#ifndef _DEBUG  // GLSampleView.cpp のデバッグ バージョン
inline CGLSampleDoc* CGLSampleView::GetDocument() const
   { return reinterpret_cast<CGLSampleDoc*>(m_pDocument); }
#endif

