// GLSampleView.cpp : CGLSampleView クラスの実装
//

#include "stdafx.h"
#include "GLSample.h"

#include "GLSampleDoc.h"
#include "GLSampleView.h"

#include "DlgTransSettings.h"
#include "DevIo.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


static UINT	StartButtonThread(LPVOID pParam);
static UINT	StopButtonThread(LPVOID pParam);
static int	GetRecData(CGLSampleView *pView, CDevIo *pDev);



// CGLSampleView

IMPLEMENT_DYNCREATE(CGLSampleView, CFormView)

BEGIN_MESSAGE_MAP(CGLSampleView, CFormView)
	ON_BN_CLICKED(IDC_TRNS_SETTINGS, &CGLSampleView::OnBnClickedTrnsSettings)
	ON_BN_CLICKED(IDC_BT_START, &CGLSampleView::OnBnClickedBtStart)
	ON_BN_CLICKED(IDC_BT_STOP, &CGLSampleView::OnBnClickedBtStop)
END_MESSAGE_MAP()

// CGLSampleView コンストラクション/デストラクション

CGLSampleView::CGLSampleView()
	: CFormView(CGLSampleView::IDD)
{
	// TODO: 構築コードをここに追加します。

}

CGLSampleView::~CGLSampleView()
{
}

void CGLSampleView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BT_START, m_BtStart);
	DDX_Control(pDX, IDC_BT_STOP, m_BtStop);
	DDX_Control(pDX, IDC_ED_STAT, m_EdStat);
}

BOOL CGLSampleView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: この位置で CREATESTRUCT cs を修正して Window クラスまたはスタイルを
	//  修正してください。

	return CFormView::PreCreateWindow(cs);
}

void CGLSampleView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// 枠をなくす
	ModifyStyleEx(WS_EX_CLIENTEDGE,0,SWP_DRAWFRAME);

	GetParentFrame()->RecalcLayout();
	ResizeParentToFit(FALSE);

}


// CGLSampleView 診断

#ifdef _DEBUG
void CGLSampleView::AssertValid() const
{
	CFormView::AssertValid();
}

void CGLSampleView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CGLSampleDoc* CGLSampleView::GetDocument() const // デバッグ以外のバージョンはインラインです。
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CGLSampleDoc)));
	return (CGLSampleDoc*)m_pDocument;
}
#endif //_DEBUG


// CGLSampleView メッセージ ハンドラ


//
// 通信設定メニュー
//

void CGLSampleView::OnBnClickedTrnsSettings()
{
	// TODO: ここにコントロール通知ハンドラ コードを追加します。

	CDlgTransSettings	dlg;
	INT_PTR				ret;

	dlg.m_pView = this;

	ret = dlg.DoModal();	// 通信設定ダイアログを開く
							// OKボタンでDocumentの内容を更新して戻る
}



//
// スタートボタン
//

void CGLSampleView::OnBnClickedBtStart()
{
	// TODO: ここにコントロール通知ハンドラ コードを追加します。

	// スタートボタンを無効化
	m_BtStart.EnableWindow(FALSE);

	// スタートボタンスレッドを起動
	::AfxBeginThread(StartButtonThread,this);
}


//
// ストップボタン
//

void CGLSampleView::OnBnClickedBtStop()
{
	// TODO: ここにコントロール通知ハンドラ コードを追加します。

	// ストップボタンを無効化
	m_BtStop.EnableWindow(FALSE);

	// ストップボタンスレッドを起動
	::AfxBeginThread(StopButtonThread,this);
}


//
// Logの最後に文字列を追加
//
void CGLSampleView::AddLog(CString &str)
{
	CEdit	*ed = &m_EdStat;	// Status表示
	int		nlen = ed->GetWindowTextLength();

	ed->SetSel(nlen, nlen);
	ed->ReplaceSel(str);
}

void CGLSampleView::AddLog(LPCSTR str)
{
	CString	cStr = str;
	AddLog(cStr);
}


//
// Logの最終行を上書きする
//
void CGLSampleView::UpdateLog(CString &str)
{
	CEdit	*ed = &m_EdStat;	// Status表示

	int		nlen = ed->GetWindowTextLength();
	int		nline = ed->LineFromChar(nlen);
	int		nstt = ed->LineIndex(nline);

	ed->SetSel(nstt, nlen);
	ed->ReplaceSel(str);

}

void CGLSampleView::UpdateLog(LPCSTR str)
{
	CString	cStr = str;
	UpdateLog(cStr);
}




//--------------------------------------------------------
// スレッド
//--------------------------------------------------------

//
// スタートボタン押下時のスレッド
//
//	スタート処理を行い、成功した場合はSTOPボタンを有効にする。
//	失敗した場合は再度スタートボタンを有効にする。
//
//	pParamにはViewへのポインタが格納されている。
//


// 接続機種選別用テーブル
static struct {
	char	*MachineName;
	char	MachineID;
} MCIndex[] = {
	{"GL860",	MC_GL860},
	{NULL,		NULL},
};


static UINT	StartButtonThread(LPVOID pParam)
{
	CGLSampleView	*pView = (CGLSampleView *)pParam;	// View
	CGLSampleDoc	*pDoc  = pView->GetDocument();		// Document

	CDevIo *pDev = NULL;	// 通信クラス

	CString			cStr;	// 文字バッファ
	int				nPtr;	// 汎用
	int				i;		// 汎用

	int				nCh;


	//
	// 親ウィンドウを無効化
	//
	// ×ボタンが押されないように。

	pView->GetParent()->EnableWindow(FALSE);

	//
	// ステータス表示クリア
	//

	pView->m_EdStat.SetWindowText("");


	//
	// 通信クラスの構築
	//
	pDev = new CDevIo(pDoc->m_nIf);
	if (pDev == NULL) {
		// 通信クラスが構築出来なかった場合
		goto OpenErrExit;
	}

	//
	// デバイスのオープン
	//
	// オープンに関しては、TCPとUSBで引数が異なるため、
	// 個別にオープンする。
	//

	pView->AddLog("デバイスをオープンしています。\r\n");

	switch (pDoc->m_nIf) {
	// TCP/IPの場合
	case SEL_IF_TCP:
		if (pDev->m_pTcp == NULL) {
			// TCPソケットの構築に失敗している
			goto OpenErrExit;
		}

		// TCPでオープン
		if (pDev->Open(pDoc->m_dwIpAddr,pDoc->m_dwPort) == FALSE) {
			// デバイスのオープンに失敗した
			goto OpenErrExit;
		}

		break;

	// USBの場合
	case SEL_IF_USB:
		if (pDev->m_pUsb == NULL) {
			// USBソケットの構築に失敗している
			goto OpenErrExit;
		}

		// USBでオープン
		if (pDev->Open(":IF:ID?",pDoc->m_dwMachineId) == FALSE) {
			// デバイスのオープンに失敗した
			goto OpenErrExit;
		}

		break;

	// その他（通常はあり得ない）
	default:
		goto OpenErrExit;
	}


	//
	// エラークリア
	//
	if (pDev->SendCommand("*CLS")) {
		// エラー
		goto CommErrExit;
	}


	//
	// 機種判別
	//
	if (pDev->SendQuery("*IDN?", cStr)) {
		// エラー
		goto CommErrExit;
	}

	//
	// 文字列は "*IDN GRAPHTEC,GL980,0,1.00"
	// の形式で戻って来るので、目的の機種名が
	// 含まれるかを検査
	//
	for (i=0;;i++) {
		if (MCIndex[i].MachineName == NULL) {
			// 機種選別テーブルの最後まで検索し終わった
			pView->AddLog("未対応の機種です。\r\n");
			goto ErrExit;
		}
		if (cStr.Find(MCIndex[i].MachineName) >= 0) {
			// 一致した
			pDoc->m_nMachine = MCIndex[i].MachineID;
			cStr.Format("機種：%s\r\n", MCIndex[i].MachineName);
			pView->AddLog(cStr);
			break;
		}
	}


	//
	// チャネル数の取得
	//
	if (pDev->SendQuery(":INFO:CH?",cStr)) {
		// エラー
		goto CommErrExit;
	}

	//
	// 文字列は ":INFO:CH 10"
	// の形式で戻って来るので、最初のスペース文字を検出
	//
	if ((nPtr = cStr.Find(' ')) < 0 ) {
		// 区切り文字が見つからない
		goto CommErrExit;
	}

	nPtr++;									// CH数文字列の先頭
	cStr = cStr.Mid(nPtr);					// CH数文字列を抽出
	pDoc->m_nCh = atoi(cStr.GetBuffer(0));	// CH数

	cStr.Format("CH数：%d\r\n",pDoc->m_nCh);
	pView->AddLog(cStr);


	//
	// AMPの設定
	//
	// 全CH同じ設定とする。
	// DC/10V/FILT OFF
	// Alarm OFF
	//
	for (nCh = 0; nCh < pDoc->m_nCh; nCh++) {
		cStr.Format("AMPの設定を行っています・・・CH%d", nCh+1);
		pView->UpdateLog(cStr);

		// 入力、レンジ、フィルタ
		cStr.Format(":AMP:CH%d:INP DC;RANG 10V;FILT OFF", nCh+1);
		if (pDev->SendCommand(cStr)) {
			// エラー
			goto CommErrExit;
		}

		// アラーム
		cStr.Format(":ALAR:CH%d:SET OFF", nCh+1);
		if (pDev->SendCommand(cStr)) {
			// エラー
			goto CommErrExit;
		}
	}
	pView->AddLog("\r\n");


	//
	// LOGICの設定
	//
	// OFFとする
	//
	pView->AddLog("LOGICとPULSEを設定しています。\r\n");

	// Logic記録
	if (pDev->SendCommand(":LOGIPUL:FUNC OFF")) {
		// エラー
		goto CommErrExit;
	}

	//
	// TRIGGERの設定
	//
	// OFFとする。
	//

	// スタートトリガ
	if (pDev->SendCommand(":TRIG:COND0:SOUR OFF")) {
		// エラー
		goto CommErrExit;
	}

	// ストップトリガ
	if (pDev->SendCommand(":TRIG:COND1:SOUR OFF")) {
		// エラー
		goto CommErrExit;
	}




	//
	// データの保存先を設定
	//
	//	\MEM\Sample.GBD
	//  に固定。
	//

	pView->AddLog("データの保存先を設定しています。\r\n");

	// 既にファイルが作成されている場合に備えてファイル消去コマンドを送信。
	// GLでは同名のファイルが存在した場合、自動で最後に_CPを付加する為。
	// 本サンプルプログラムでは、固定のファイルを使用するために、削除する。

	if (pDev->SendCommand(":FILE:RM \"\\MEM\\Sample.GBD\"")) {
		// エラー
		goto CommErrExit;
	}

	// \MEM\Dample.GBDを設定
	cStr = ":DATA:CAPT DISK,\"\\MEM\\Sample.GBD\"";	// "\MEM\Sample.GBD"

	if (pDev->SendCommand(cStr)) {
		// エラー
		goto CommErrExit;
	}


	//
	// サンプリングの設定
	//
	// 1sに固定
	// (GL800で101ch以上実装の場合は2s)
	//

	pView->AddLog("サンプリングを設定しています。\r\n");

	if (pDev->SendCommand(":DATA:SAMP 1S")) {
		// エラー
		goto CommErrExit;
	}

	//
	// 記録スタート
	//

	pView->AddLog("記録をスタートしています。\r\n");

	if (pDev->SendCommand(":MEAS:START")) {
		// エラー
		goto CommErrExit;
	}

	//
	// 本当にスタートしたか確認する
	//
	for (i = 0; i < 20; i++) {
		int	nStat;

		if (pDev->SendQuery(":STAT:COND?",cStr)) {
			// エラー
			goto CommErrExit;
		}

		//
		// 文字列は ":STAT:COND 1234"
		// の形式で戻って来るので、最初のスペース文字を検出
		//
		if ((nPtr = cStr.Find(' ')) < 0 ) {
			// 区切り文字が見つからない
			goto CommErrExit;
		}

		nPtr++;								// ステータス文字列の先頭
		cStr = cStr.Mid(nPtr);				// ステータス文字列を抽出
		nStat = atoi(cStr.GetBuffer(0));	// ステータス

		if (nStat & 1) {
			// 本体が記録状態になった
			break;
		}

		::Sleep(500);	//500ms Wait
	}
	if (i == 20) {
		// 本体はスタートしてない？
		pView->AddLog("本体がスタートできませんでした。\r\n");

		goto ErrExit;
	}

	pView->AddLog("スタートしました。\r\n");

	//
	// デバイスをクローズ
	//
	pView->AddLog("デバイスを閉じています。\r\n");
	pDev->Close();

	//
	// ストップボタンを有効に
	//
	pView->m_BtStop.EnableWindow(TRUE);


	//
	// 通信クラスを解放
	//
	delete pDev;

	//
	// 親ウィンドウを復帰
	//
	pView->GetParent()->EnableWindow(TRUE);

	return(0);


/////////////////////////////////////////////////////////////////


	//
	// エラー処理
	//


	//
	// デバイスのオープンに失敗した
	//
OpenErrExit:
	pView->AddLog("デバイスのオープンに失敗しました。\r\n");
	goto ErrExit;

	//
	// コマンドの送受信に失敗した
	//
CommErrExit:
	pView->AddLog("デバイスとの通信に失敗しました。\r\n");
	goto ErrExit;


	//
	// 何某かのエラーで最後まで処理が行われなかった場合
	//
ErrExit:
	//
	// 後始末
	//
	pDev->Close();
	if (pDev) {
		delete pDev;
	}

	//
	// スタートボタンを有効にする
	//
	pView->m_BtStart.EnableWindow(TRUE);

	//
	// 親ウィンドウを復帰
	//
	pView->GetParent()->EnableWindow(TRUE);

	return(0);
}




//
// ストップボタン押下時のスレッド
//
//	ストップ処理を行い、成功した場合はスタートボタンを有効にする。
//	失敗した場合は再度ストップボタンを有効にする。
//
//	pParamにはViewへのポインタが格納されている。
//
//  機種判別はスタート時に行った物を使用していることに注意。
//
static UINT	StopButtonThread(LPVOID pParam)
{
	CGLSampleView	*pView = (CGLSampleView *)pParam;	// View
	CGLSampleDoc	*pDoc  = pView->GetDocument();		// Document

	CDevIo *pDev = NULL;	// 通信クラス

	CString			cStr;	// 文字バッファ
	int				nPtr;	// 汎用
	int				i;		// 汎用

	int				nErr;	// エラーコード


	//
	// 親ウィンドウを無効化
	//
	// ×ボタンが押されないように。

	pView->GetParent()->EnableWindow(FALSE);

	//
	// ステータス表示クリア
	//

	pView->m_EdStat.SetWindowText("");


	//
	// 通信クラスの構築
	//
	pDev = new CDevIo(pDoc->m_nIf);
	if (pDev == NULL) {
		// 通信クラスが構築出来なかった場合
		goto OpenErrExit;
	}

	//
	// デバイスのオープン
	//
	// オープンに関しては、TCPとUSBで引数が異なるため、
	// 個別にオープンする。
	//

	pView->AddLog("デバイスをオープンしています。\r\n");

	switch (pDoc->m_nIf) {
	// TCP/IPの場合
	case SEL_IF_TCP:
		if (pDev->m_pTcp == NULL) {
			// TCPソケットの構築に失敗している
			goto OpenErrExit;
		}

		// TCPでオープン
		if (pDev->Open(pDoc->m_dwIpAddr,pDoc->m_dwPort) == FALSE) {
			// デバイスのオープンに失敗した
			goto OpenErrExit;
		}

		break;

	// USBの場合
	case SEL_IF_USB:
		if (pDev->m_pUsb == NULL) {
			// USBソケットの構築に失敗している
			goto OpenErrExit;
		}

		// USBでオープン
		if (pDev->Open(":IF:ID?",pDoc->m_dwMachineId) == FALSE) {
			// デバイスのオープンに失敗した
			goto OpenErrExit;
		}

		break;

	// その他（通常はあり得ない）
	default:
		goto OpenErrExit;
	}


	//
	// エラークリア
	//
	if (pDev->SendCommand("*CLS")) {
		// エラー
		goto CommErrExit;
	}




	//
	// 記録ストップ
	//

	pView->AddLog("記録をストップしています。\r\n");

	if (pDev->SendCommand(":MEAS:STOP")) {
		// エラー
		goto CommErrExit;
	}

	//
	// 本当にストップしたか確認する
	//
	for (i = 0; i < 20; i++) {
		int	nStat;

		if (pDev->SendQuery(":STAT:COND?",cStr)) {
			// エラー
			goto CommErrExit;
		}

		//
		// 文字列は ":STAT:COND 1234"
		// の形式で戻って来るので、最初のスペース文字を検出
		//
		if ((nPtr = cStr.Find(' ')) < 0 ) {
			// 区切り文字が見つからない
			goto CommErrExit;
		}

		nPtr++;								// ステータス文字列の先頭
		cStr = cStr.Mid(nPtr);				// ステータス文字列を抽出
		nStat = atoi(cStr.GetBuffer(0));	// ステータス

		if (!(nStat & 1)) {
			// 本体が停止状態になった
			break;
		}

		::Sleep(500);	//500ms Wait
	}
	if (i == 20) {
		// 本体はストップしてない？
		pView->AddLog("本体がストップできませんでした。\r\n");

		goto ErrExit;
	}

	pView->AddLog("ストップしました。\r\n");

	//
	// 収録されたデータを取得する
	//

	if (::MessageBox(NULL,
					 "収録されたデータを転送しますか？",
					 "確認",
					 MB_YESNO|MB_ICONQUESTION
					 ) == IDYES) {

		//
		// 収録されたデータを保存する
		//
		nErr = GetRecData(pView, pDev);
		switch (nErr) {
		case -1:	// ディスクエラー
			goto ErrExit;
		case 1: 	// 通信エラー
			goto CommErrExit;
		case 2:		// 本体エラー
			goto ErrExit;
		case 3:		// メモリエラー
			goto ErrExit;
		}

	}

	//
	// デバイスをクローズ
	//
	pView->AddLog("デバイスを閉じています。\r\n");
	pDev->Close();

	//
	// スタートボタンを有効にする
	//
	pView->m_BtStart.EnableWindow(TRUE);


	//
	// 通信クラスを解放
	//
	delete pDev;

	//
	// 親ウィンドウを復帰
	//
	pView->GetParent()->EnableWindow(TRUE);

	return(0);


/////////////////////////////////////////////////////////////////


	//
	// エラー処理
	//


	//
	// デバイスのオープンに失敗した
	//
OpenErrExit:
	pView->AddLog("デバイスのオープンに失敗しました。\r\n");
	goto ErrExit;

	//
	// コマンドの送受信に失敗した
	//
CommErrExit:
	pView->AddLog("デバイスとの通信に失敗しました。\r\n");
	goto ErrExit;


	//
	// 何某かのエラーで最後まで処理が行われなかった場合
	//
ErrExit:
	//
	// 後始末
	//
	pDev->Close();
	if (pDev) {
		delete pDev;
	}

	//
	// ストップボタンを有効に
	//
	pView->m_BtStop.EnableWindow(TRUE);

	//
	// 親ウィンドウを復帰
	//
	pView->GetParent()->EnableWindow(TRUE);

	return(0);
}




//
// 本体からデータを取得する
//
//  pView: Viewへのポインタ
//	pDev:  通信クラスへのポインタ
//
//	return =	0:成功
//				1:通信エラー
//				2:本体動作エラー
//				3:メモリーエラー
//				-1:ファイルエラー
//

// 通信タイムアウト
#define	R_TIMOUT		10000	// 受信タイムアウト(10秒）
#define	T_TIMOUT		10000	// 送信タイムアウト(10秒）

// :TRANS:OUTP:DATA? のステータス
#define	S_OUT			0x0004	// 開始点範囲外
#define	E_OUT			0x0002	// 終了点範囲外
#define	T_ERR			0x0001	// エラー

#define	TRNS_POINTS		500		// 一回の転送で受け取るデータ点数

static int	GetRecData(CGLSampleView *pView, CDevIo *pDev)
{
	CGLSampleDoc	*pDoc  = pView->GetDocument();		// Document

	CFileDialog		*pFDlg;			// ファイルダイアログ
	CString			cFileName;		// 保存先のファイルネーム（PC フルパス）

	CFile			*pFile = NULL;	// ファイルハンドル

	CString			cStr;			// 文字バッファ
	char			*pBuf = NULL;	// データ受信バッファ
	char			nBuf[32];		// 受信バッファ

	char			nStat[16];		// ステータス

	DWORD			dwSize;			// 読み込みサイズ

	DWORD			dwHeaderSize;	// ヘッダーサイズ
	DWORD			dwDataSize;		// データサイズ
	DWORD			dwRcvDataSize;	// 受信データサイズ

	DWORD			dwSttPos;		// データ読み込み開始点
	DWORD			dwStpPos;		// データ読み込み終了点

	int				nErr = 0;		// エラーコード

	int				i;				// 汎用
	char			*pPtr;			// 汎用
	//
	// 転送先のファイル名を取得する
	//

	// ダイアログを開く

	pFDlg = new CFileDialog(	FALSE,
								"GBD",
								"Sample.GBD",
								OFN_OVERWRITEPROMPT,
								"GBDファイル(*.gbd)|*.gbd|全てのファイル(*.*)|*.*||"
							 );
	if (pFDlg->DoModal() == IDCANCEL) {
		// キャンセルされた
		// この場合は正常終了する。

		delete pFDlg;
		return(0);
	}

	// ファイルネームを取得する
	cFileName = pFDlg->GetPathName();

	delete pFDlg;


	//
	// 転送元の設定を行う
	//

	pView->AddLog("データの転送元を設定しています。\r\n");

	// \MEM\Dample.GBDを設定
	cStr = ":TRANS:SOUR DISK,\"\\MEM\\Sample.GBD\"";	//"\MEM\Sample.GBD"
	if (pDev->SendCommand(cStr)) {
		// エラー
		goto CommErrExit;
	}


	try {	//CFileException

		//
		// ファイルのオープン
		//

		pView->AddLog("ファイルをオープンしています。\r\n");

		pFile = new CFile(	cFileName,
							CFile::modeCreate | CFile::modeWrite | CFile::shareDenyNone
							);


		//
		// 本体側のオープン
		//

		pView->AddLog("データをオープンしています。\r\n");

		// コマンド送信
		if (pDev->SendCommand(":TRANS:OPEN?")) {
			// エラー
			goto CommErrExit;
		}
		// ステータス受信
		dwSize = 3;	// ステータスは3バイト
		if (pDev->ReadBinary(nStat, dwSize, R_TIMOUT)) {
			// エラー
			goto CommErrExit;
		}
		// ステータスの確認
		if (nStat[2]) {
			// 本体のオープン処理に失敗した
			pView->AddLog("本体ファイルのオープンに失敗しました。\r\n");
			goto RecErrExit;
		}

		//
		// ヘッダーを読み込む
		//

		pView->AddLog("ヘッダーを読み込んでいます。\r\n");

		// コマンド送信
		if (pDev->SendCommand(":TRANS:OUTP:HEAD?")) {
			// エラー
			goto CommErrExit;
		}
		// 受信サイズの受信
		dwSize = 8;	// 8バイト #6xxxxxx
		if (pDev->ReadBinary(nBuf, dwSize, R_TIMOUT)) {
			// エラー
			goto CommErrExit;
		}
		if (dwSize != 8) {
			// エラー
			goto CommErrExit;
		}
		// 受信サイズを取り出す
		nBuf[8] = 0;
		if (nBuf[0] != '#') {
			// おかしい？
			// 予期せぬデータ
			// 余分な受信データが有るかも知れないが、とりあえず無視。
			// 念のため、本体の電源を一度切った方が良い。
			pView->AddLog("ヘッダーを受信できませんでした。\r\n");
			goto CommErrExit;
		}

		dwHeaderSize = atoi(&nBuf[2]);

		// 一時受信領域を確保
		pBuf = new char[dwHeaderSize + 5];	// 受信領域
											// ステータス(2byte)＋ヘッダー本体＋チェックサム(2byte)＋NULL文字
		if (pBuf == NULL) {
			// メモリが確保できなかった？
			goto MemErrExit;
		}

		// ヘッダーの受信
		dwSize = dwHeaderSize + 4;	// ステータスとチェックサム領域を足す
		if (pDev->ReadBinary(pBuf, dwSize, R_TIMOUT)) {
			// エラー
			goto CommErrExit;
		}
		if (dwSize != dwHeaderSize + 4) {
			// エラー
			goto CommErrExit;
		}

		// チェックサムの確認
		// （当サンプルプログラムでは行わない）

		// ヘッダーの書き込み
		pFile->Write(&pBuf[2], dwHeaderSize);


		//
		// データの読み込み
		//

		// ヘッダーから、データ点数を得る

		// "Counts"を探す
		pPtr = strstr(&pBuf[2],"Counts");
		if (pPtr == NULL) {
			// 見つからない
			goto CommErrExit;
		}
		pPtr += 6;	// "Counts"をスキップ
		// 最初に出現する数字を見つける
		for (i = 0; i < 16; i++, pPtr++) {	// 無限ループにならないように
			if (*pPtr >= '0' && *pPtr <= '9') {
				// 発見
				break;
			}
		}
		if (i == 16) {
			// 見つからなかった
			goto CommErrExit;
		}
		// 数字を数値化する
		dwDataSize = 0;
		for (i = 0; i < 32; i++, pPtr++) {	// 無限ループにならないように
			if (*pPtr >= '0' && *pPtr <= '9') {
				dwDataSize *= 10;
				dwDataSize += *pPtr - '0';
			} else {
				// 数字以外の文字
				// ここで終了
				break;
			}
		}
		if (i == 32) {
			// おかしい？
			goto CommErrExit;
		}

		delete pBuf;
		pBuf = NULL;



		// データの受信
		//
		// dwDataSize: データの総点数
		//
		for (dwSttPos = 0; dwSttPos < dwDataSize; dwSttPos += TRNS_POINTS) {

			cStr.Format("レコード %d/%d を転送しています・・・", dwSttPos, dwDataSize);
			pView->UpdateLog(cStr);

			// データ転送の設定
			dwStpPos = dwSttPos + TRNS_POINTS - 1;
			if (dwStpPos >= dwDataSize) {
				dwStpPos = dwDataSize - 1;	// Base 0
			}

			cStr.Format(":TRANS:OUTP:DATA %d,%d", dwSttPos + 1, dwStpPos + 1);	// Base 1
			if (pDev->SendCommand(cStr)) {
				// エラー
				goto CommErrExit;
			}

			// データ取得コマンドの送信
			if (pDev->SendCommand(":TRANS:OUTP:DATA?")) {
				// エラー
				goto CommErrExit;
			}

			// 受信サイズの受信
			dwSize = 8;	// 8バイト #6xxxxxx
			if (pDev->ReadBinary(nBuf, dwSize, R_TIMOUT)) {
				// エラー
				goto CommErrExit;
			}
			if (dwSize != 8) {
				// エラー
				goto CommErrExit;
			}

			// 受信サイズを取り出す
			nBuf[8] = 0;
			if (nBuf[0] != '#') {
				// おかしい？
				// 予期せぬデータ
				// 余分な受信データが有るかも知れないが、とりあえず無視。
				// 念のため、本体の電源を一度切った方が良い。
				pView->AddLog("\r\nデータを受信できませんでした。\r\n");
				goto CommErrExit;
			}

			dwRcvDataSize = atoi(&nBuf[2]);

			if (dwRcvDataSize == 0) {
				// 受信データなし？
				break;
			}

			// 一時受信領域を確保
			pBuf = new char[dwRcvDataSize];	// 受信領域
												// ヘッダー本体
			if (pBuf == NULL) {
				// メモリが確保できなかった？
				goto MemErrExit;
			}

			// データの受信
			dwSize = dwRcvDataSize;	// ステータスとチェックサムを含んている
			if (pDev->ReadBinary(pBuf, dwSize, R_TIMOUT)) {
				// エラー
				goto CommErrExit;
			}
			if (dwSize != dwRcvDataSize) {
				// エラー
				goto CommErrExit;
			}

			// チェックサムの確認
			// （当サンプルプログラムでは行わない）

			// データの書き込み
			pFile->Write(&pBuf[2], dwRcvDataSize + 2);	// ステータス[2]とチェックサム(２)を加味する


			// データバッファの解放
			delete pBuf;
			pBuf = NULL;

		}

		cStr.Format("レコード %d/%d を転送しています・・・\r\n", dwDataSize, dwDataSize);
		pView->UpdateLog(cStr);

		//
		// 本体側のクローズ
		//

		pView->AddLog("データをクローズしています。\r\n");

		// コマンド送信
		if (pDev->SendCommand(":TRANS:CLOSE?")) {
			// エラー
			goto CommErrExit;
		}
		// ステータス受信
		dwSize = 2;	// ステータスは2バイト
		if (pDev->ReadBinary(nStat, dwSize, R_TIMOUT)) {
			// エラー
			goto CommErrExit;
		}
		// ステータスの確認
		if (nStat[1]) {
			// 本体のクローズ処理に失敗した
			pView->AddLog("本体ファイルのクローズに失敗しました。\r\n");
			goto RecErrExit;
		}

	}
	catch (CFileException *ex) {
		//
		// ファイル操作でエラーになった
		//
		ex->ReportError();
		ex->Delete();

		pFile->Close();

		delete pFile;

		pView->AddLog("\r\n");

		return(-1);
	}

	//
	// ファイルのクローズ
	//

	pView->AddLog("ファイルをクローズしています。\r\n");

	pFile->Close();

	//
	// ファイルクラスの破棄
	//
	delete pFile;

	return(0);




/////////////////////////////////////////////////////////////////


	//
	// エラー処理
	//

	// 通信エラー
CommErrExit:
	nErr = 1;
	goto ErrExit;

	// 本体側のエラー
RecErrExit:
	nErr = 2;
	goto ErrExit;

	// メモリエラー
MemErrExit:
	pView->AddLog("メモリが確保できませんでした。\r\n");
	nErr = 3;
	goto ErrExit;

ErrExit:
	//
	// 後始末
	//
	if (pFile) {
		pFile->Close();
		delete pFile;
	}

	if (pBuf) {
		delete pBuf;
	}

	return(nErr);

}


