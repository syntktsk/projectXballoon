/////////////////////////////////////////////
//
// GL サンプルプログラム
//
//		Copyright (C) 2010 Graphtec CORP.
//
// DevIo.cpp: CDevIo クラス
//
//	TCP/IPとUSBの抱き込みをここで行う
//  上位アプリケーションからは、どっちを使ってるのか
//  意識する必要はない。
//
//////////////////////////////////////////////

// DevIo.cpp: CDevIo クラスのインプリメンテーション
//

#include "stdafx.h"

#include "ASocket.h"
#include "UsbSocket.h"
#include "DevIo.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

//
// コンストラクタ
//
CDevIo::CDevIo(char nSelIf)
:m_nSelIf(nSelIf)
{
	m_pTcp = NULL;
	m_pUsb = NULL;

	//
	// 指定された通信手段を構築する
	//
	if (m_nSelIf == IF_TCP_IP) {
		// TCP/IP
		m_pTcp = (CASocket*) new (CASocket);

		// Winsock初期化
		if (m_pTcp) {
			m_pTcp->Startup();
		}

	} else {
		// USB
		m_pUsb = (CUsbio*) new (CUsbio);
	}
}

//
// デストラクタ
//
CDevIo::~CDevIo()
{
	//
	// 通信手段を解放
	//
	if (m_pTcp) {

		// Winsock 後始末
		m_pTcp->Cleanup();

		delete m_pTcp;
	}

	if (m_pUsb) {
		delete m_pUsb;
	}
}


//
// オープン
//

/// TCP/IP用
/// nIpAddress :	IP アドレス
///	nPort :			ポート番号

BOOL CDevIo::Open(DWORD nIpAddress, DWORD nPort)
{
	m_nIpAddress = nIpAddress;
	m_nPort = nPort;

	if (m_nSelIf == IF_TCP_IP) {
		// TCP/IP

		in_addr	Adr;
		char	*sAddr;

		Adr.S_un.S_addr = htonl(m_nIpAddress);
		sAddr = inet_ntoa(Adr);
		strcpy_s(m_sHostAddr,sAddr);


		// ソケット生成  (Socket,Bind)
		if (!m_pTcp->Socket(SOCK_STREAM)) {
			TRACE("Socket Create Error!\n");
			return(FALSE);
		}


		// 接続 (Connect)
		if (!m_pTcp->Connect((LPCTSTR)m_sHostAddr,m_nPort)) {
			TRACE("Socket Connect Error!\n");
			return(FALSE);
		}

	} else {
		// 変？　通常はここは通らないはず
		return(FALSE);
	}

	return(TRUE);
}

/// USB用

BOOL CDevIo::Open(char *pIdCmd, DWORD nMachineId)
{

	m_nMachineId = nMachineId;

	m_pUsb->m_pIdCmd = pIdCmd;

	if (m_nSelIf == IF_USB) {
		// USB
		// ソケット生成  (ダミー）
		if (!m_pUsb->Create()) {
			TRACE("USB Create Error!\n");
			return(FALSE);
		}

		// 接続 (Connect)
		if (!m_pUsb->Connect(m_nMachineId)) {
			TRACE("USB Connect Error!\n");
			return(FALSE);
		}
	} else {
		// 変？　通常はここは通らないはず
		return(FALSE);
	}

	return(TRUE);
}


//
// クローズ
//

BOOL CDevIo::Close()
{
	if (m_nSelIf == IF_TCP_IP) {
		// TCP/IP
		m_pTcp->Close();
	} else {
		// USB
		m_pUsb->Close();
	}
	return(TRUE);
}

//
// リード
//		input:  pBuf     受信データバッファ
//              dwBytes  (I) 受信したいbyte数/(O) 受信したbyte数
//              dwTmout  タイムアウト(= INFINITE)
//      return: 0 = OK/ WSAExxxxxx
//              起きる可能性のあるError
//		- 注意:
//			- 受信しようとしたbyte数を一度に受信できるとは限らない
//          - タイムアウトしたときは WAIT_TIMEOUT (=258) を返す
//
//

int CDevIo::Read(LPSTR pBuf, DWORD &dwBytes, DWORD dwTmout /*= INFINITE*/)
{
	if (m_nSelIf == IF_TCP_IP) {
		// TCP/IP
		return(m_pTcp->Read(pBuf,dwBytes,dwTmout));
	}

	// USB
	return(m_pUsb->Read(pBuf,dwBytes,dwTmout));
}




//
// １行リード
//		input:  pDst       Read Buffer
//				dwSize     (I) Buffer size/(O) Bytes to read
//              dwTmout    タイムアウト
//      return: Error code: 0 = OK/ WSAExxxxxx
//
//		<終了条件>
//		Text 転送:
//      - 行末検出
//		- Buffer sizeまで読んでも行末が検出できなかったときには
//        dwSize = 0 を返す(このときはBuffer Full)
//
//

int CDevIo::ReadLn(LPSTR pDst, DWORD &dwSize, DWORD dwTmout /*= INFINITE*/)
{
	int   nCode, nErr;
	char  pBuf[4];
	//
	DWORD dwRead = 0;
	LPSTR pStr = pDst;	// 保存
	for ( ; ; ) {
		//
		//-- 1 byte づつ読むしかない:
		//
		DWORD dwBytes = 1;
		if ((nErr = Read(pBuf, dwBytes, dwTmout)) == 0) {
			//
			//-- 正常に読めている:
			//
			nCode = *pBuf & 0xff;		// 受信code
			//-- テキスト転送モード: 改行を検出
			switch (nCode) {
			case '\r':
				break;			// 無視
			case '\n':
			case '\0':
				*pDst = '\0';		// 行末検出
				dwSize = dwRead;	// 実際に読んだByte数
				return (0);			// 終了
			default:
				*pDst++ = nCode;
				if (++dwRead >= dwSize) {
					//
					//-- Buffer size 読んでも行末がないとき:
					//
					*pDst = '\0';
					dwSize = 0;		// Buffer Full: dwSize = 0
					return (0);
				}
			}
		} else {
			//
			//-- Read() が失敗:
			//
			dwSize = dwRead;	// ここまで読めたbyte数
			return (nErr);		// WSAExxxxx WAIT_TIMEOUT
		}
	}
}


//
//  バイナリーリード
//		input:  pDst       Read Buffer
//				dwSize     (I) Buffer size/(O) Bytes to read
//              dwTmout    タイムアウト
//		return: 0 = OK/ WSAExxxxx
//
int CDevIo::ReadBinary(LPSTR pDst, DWORD &dwSize, DWORD dwTmout/* = INFINITE*/)
{
	DWORD  dwBytesToRead = dwSize;
	LPCSTR pStr = pDst;
	DWORD  dwBytes;
	int    nErr = 0;

	for ( ; dwBytesToRead; ) {

		dwBytes = dwBytesToRead;	// 読み込みバイト数

		if (nErr = Read(pDst, dwBytes, dwTmout)) {
			//-- Readに失敗/タイムアウト:
			if (dwBytes == 0) {
				// エラー
				break;
			}
		}
		pDst += dwBytes;			// 今回読めたbyte数だけpointerを進める
		dwBytesToRead -= dwBytes;	// まだ読めていないbyte数
	}
	dwSize = (DWORD)(pDst - pStr);			// 読めたbyte数
	return (nErr);
}

//
//　ライト
//		input:  pBuf     送信するデータ
//              dwBytes  (I) データbyte数/(O) 送信したbyte数
//              dwTmout  タイムアウト(= INFINITE)
//      return: Error code: 0 = OK/ WSAExxxxxx
//		- 注意:
//			- 送信しようとしたbyte数を一度に送信できるとは限らない
//          - タイムアウトしたときは WAIT_TIMEOUT (=258) を返す
//

int CDevIo::Write(LPCSTR pBuf, DWORD &dwBytes, DWORD dwTmout /*= INFINITE*/)
{
	if (m_nSelIf == IF_TCP_IP) {
		// TCP/IP
		return(m_pTcp->Write(pBuf,dwBytes,dwTmout));
	}

	// USB
	return(m_pUsb->Write(pBuf,dwBytes,dwTmout));
}


//
// 1行ライト
//		input:  pSrc       Write Buffer
//              dwSize     (I)Write size/(O)Written size
//              dwTmout    タイムアウト
//		return: 0 = OK/ WSAExxxxx
//
//		pSrcのNULLコード検出まで送信する。
//		dwSize文字内にNULLコードが無い場合はdwSize分送信する
//

int CDevIo::WriteLn(LPCSTR pSrc, DWORD dwSize /*=4096 */, DWORD dwTmout /*= INFINITE*/)
{
	DWORD  dwBytesToWrite;
	LPCSTR pStr = pSrc;
	DWORD  dwBytes;
	int    nErr = 0;

	// NULLがdwSize内に有る場合はNULLまでを送信対象とする
	for (dwBytesToWrite=0; dwBytesToWrite<dwSize; dwBytesToWrite++) {
		if (*(pStr+dwBytesToWrite) == 0) {
			break;
		}
	}

	for ( ; dwBytesToWrite; ) {

		dwBytes = dwBytesToWrite;	// 送信するバイト数

		if ((nErr = Write(pSrc, dwBytes, dwTmout)) != 0) {
			//-- Writeに失敗/タイムアウト:
			break;
		}
		pSrc += dwBytes;			// 今回書けたbyte数だけpointerを進める
		dwBytesToWrite -= dwBytes;	// まだ書いてないbyte数
	}
	dwSize = (DWORD)(pSrc - pStr);			// 書けたbyte数
	return (nErr);
}



//
//-- 問い合わせコマンドの送信と応答文字列受信:
//
int CDevIo::SendQuery(LPCSTR pCmd, LPSTR pRes)
{
	const DWORD dwSendTmout = 5000;		// 送信タイムアウト
	const DWORD dwRecvTmout = 10000;		// 受信タイムアウト
	char  pCmdStr[512];
	DWORD dwBytes;
	int   nErr = 0;

	::lstrcpyn(pCmdStr, pCmd, 510);	//
	::lstrcat(pCmdStr, "\r\n");		//// 注意!!
	dwBytes = ::lstrlen(pCmdStr);	//
	//
	//-- 問い合わせコマンド送信:
	//
	if ((nErr = WriteLn(pCmdStr, dwBytes, dwSendTmout)) != 0) {
		return (nErr);
	}
	//
	//-- 応答文字列を受信:
	//
	dwBytes = 512;	// 注意
	nErr = ReadLn(pRes, dwBytes, dwRecvTmout);
	if (dwBytes == 0) {
		// 行末文字が無かった
		nErr = 1;
	}
	return (nErr);
}

int CDevIo::SendQuery(CString &pCmd, CString &pRes)
{
	char  pR[512];
	int   nErr = 0;

	nErr = SendQuery(pCmd.GetBuffer(0), pR);

	if (nErr == 0) {
		pRes = pR;
	} else {
		pRes = "";
	}

	return(nErr);
}

int CDevIo::SendQuery(LPCSTR pCmd, CString &pRes)
{
	char  pR[512];
	int   nErr = 0;

	nErr = SendQuery(pCmd, pR);

	if (nErr == 0) {
		pRes = pR;
	} else {
		pRes = "";
	}

	return(nErr);
}



//
//-- コマンドの送信のみ:
//
int CDevIo::SendCommand(LPCSTR pCmd)
{
	const DWORD dwSendTmout = 5000;		// 送信タイムアウト
	char  pCmdStr[512];
	DWORD dwBytes;
	int   nErr = 0;

	::lstrcpyn(pCmdStr, pCmd, 510);	//
	::lstrcat(pCmdStr, "\r\n");		//// 注意!!
	dwBytes = ::lstrlen(pCmdStr);	//
	//
	//-- 問い合わせコマンド送信:
	//
	if ((nErr = WriteLn(pCmdStr, dwBytes, dwSendTmout)) != 0) {
		return (nErr);
	}
	return (nErr);
}

int CDevIo::SendCommand(CString &pCmd)
{
	return(SendCommand(pCmd.GetBuffer(0)));
}
