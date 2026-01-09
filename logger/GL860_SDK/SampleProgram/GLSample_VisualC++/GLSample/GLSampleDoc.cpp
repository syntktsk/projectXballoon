// GLSampleDoc.cpp : CGLSampleDoc クラスの実装
//

#include "stdafx.h"
#include "GLSample.h"

#include "GLSampleDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CGLSampleDoc

IMPLEMENT_DYNCREATE(CGLSampleDoc, CDocument)

BEGIN_MESSAGE_MAP(CGLSampleDoc, CDocument)
END_MESSAGE_MAP()


// CGLSampleDoc コンストラクション/デストラクション

CGLSampleDoc::CGLSampleDoc()
{
	// TODO: この位置に 1 度だけ呼ばれる構築用のコードを追加してください。

}

CGLSampleDoc::~CGLSampleDoc()
{
}

BOOL CGLSampleDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: この位置に再初期化処理を追加してください。
	// (SDI ドキュメントはこのドキュメントを再利用します。)

	m_dwIpAddr = 0xc0a80001;	// 192.168.0.1
	m_dwPort = 8023;

	m_dwMachineId = 0;

	m_nIf = SEL_IF_TCP;

	m_nMachine = MC_GL860;

	return TRUE;
}




// CGLSampleDoc シリアル化

void CGLSampleDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 格納するコードをここに追加してください。
	}
	else
	{
		// TODO: 読み込むコードをここに追加してください。
	}
}


// CGLSampleDoc 診断

#ifdef _DEBUG
void CGLSampleDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CGLSampleDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CGLSampleDoc コマンド
