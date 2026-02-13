// GLSample.h : GLSample アプリケーションのメイン ヘッダー ファイル
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH に対してこのファイルをインクルードする前に 'stdafx.h' をインクルードしてください"
#endif

#include "resource.h"       // メイン シンボル


// CGLSampleApp:
// このクラスの実装については、GLSample.cpp を参照してください。
//

class CGLSampleApp : public CWinApp
{
public:
	CGLSampleApp();


// オーバーライド
public:
	virtual BOOL InitInstance();

// 実装
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CGLSampleApp theApp;