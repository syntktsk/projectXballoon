//////////////////////////////////////////////////////////////////
//
// GL サンプルプログラム
//
//		Copyright (C) 2010 Graphtec CORP.
//
// UsbSock.cpp: CUsbSocket クラス
//
//
//	USBをWinsock風に使用するためのWrapper関数群
//
//////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UsbSocket.h"
#include "Fifo.h"

#include "gtcusbr.h"	// gtcusbr.dll使用の為のヘッダ


/////////////////////////////////
// 構築/消滅
/////////////////////////////////

//
//-- コンストラクタ:
//
CUsbio::CUsbio()
{
}

//
//-- デストラクタ:
//
CUsbio::~CUsbio()
{
}

/////////////////////////////////
// 操作関数:
//

//
//-- Create() 作成
//
BOOL CUsbio::Create()
{
	return (TRUE);
}

//
//-- USB のクローズ:
//
void CUsbio::Close()
{
TRACE("USB Close!!\n");
	if (m_hUsb != INVALID_HANDLE_VALUE) {
		GtcUSBr_CloseDevice(m_hUsb);
		m_hUsb = INVALID_HANDLE_VALUE;
	}
}


//
//-- 接続要求:
//		input:  nId   機器ID
//
BOOL CUsbio::Connect(int nId)
{
	//USB接続
	BOOL bRet;
	int nErr;
	HANDLE hUsb[128];
	int nOpen = 0;

	while(1) {
		//USBをオープン
		m_hUsb = GtcUSBr_OpenDevice();
		if(m_hUsb == INVALID_HANDLE_VALUE || m_hUsb == NULL) {
			// オープンに失敗した
			SetLastError(USBERR_INVALID_HANDLE);	//後でDEFINEにする。
			bRet = FALSE;
			goto Exit;
		} else {
			hUsb[nOpen] = m_hUsb;	//HANDLE Set
			nOpen++;

			// 機器IDチェック
			DWORD nWrite;
			char buf[64];

			strcpy_s(buf,m_pIdCmd);	// 機器ID問い合わせコマンド文字列
			nWrite = (DWORD)strlen(buf);
			buf[nWrite] = 0x0d;
			buf[nWrite+1] = 0x0a;
			buf[nWrite+2] = 0x00;
			nWrite += 2;

			// 機器ID問い合わせコマンド送信
			nErr = UsbWrite(buf, nWrite, 2000);
			if(nErr == 0) {
				char pBuf[256];
				//char pIdNo[256];
				int  nIdNo;
				DWORD dwBytes = 256;

				// クエリーを受信
				nErr = UsbRead(pBuf, dwBytes, 2000);
				if(nErr == 0) {
					nIdNo = ::atoi(&pBuf[6]);
					if(nIdNo == nId) {
						//IDが一致した
						nOpen--;
						bRet = TRUE;
						SetLastError(0);		// clear Error
						//goto Exit;				// 不要HANDLE終了
						break;
					}
				} else {
					//USB READ error
					bRet = FALSE;
					SetLastError(USBERR_READ_ERROR);
					goto Exit;
				}
			} else {
				//USB WRITE error
				bRet = FALSE;
				SetLastError(USNERR_WRITE_ERROR);	//後でDEFINEにする。
				goto Exit;
			}
		}
	}
Exit:
	//指定ID以外のハンドルをクローズする
	for(int i=0; i < nOpen; i++) {
		GtcUSBr_CloseDevice(hUsb[i]);
	}

	return(bRet);
}

//
//-- 受信: 同期I/O
//		input:  pBuf			データへのポインタ
//				dwReadBytes		受信するデータのバイト数
//              pBytes			受信byte数を受け取る変数
//				dwTmout			タイムアウト
//
BOOL CUsbio::ReadDevice(LPCSTR pBuf, DWORD dwReadBytes, LPDWORD pBytes, DWORD dwTmout)
{
	BOOL bRet;
	bRet = GtcUSBr_ReadDevice(m_hUsb, (void*)pBuf, dwReadBytes, pBytes, dwTmout);
	return(bRet);
}

//
//-- 送信: 同期I/O
//		input:  pBuf			格納バッファへのポインタ
//				dwWriteBytes	送信するデータのバイト数
//              pBytes			送信byte数を受け取る変数
//				dwTmout			タイムアウト
//
BOOL CUsbio::WriteDevice(LPCSTR pBuf, DWORD dwWriteBytes, LPDWORD pBytes, DWORD dwTmout)
{
	BOOL bRet;
	bRet = GtcUSBr_WriteDevice(m_hUsb, (void*)pBuf, dwWriteBytes, pBytes, dwTmout);
	return(bRet);
}


///////////////////////////////////////////
//  Socketより上位レベルのI/O関数:
//    Read()/Write()関数に近い仕様とする
//    以下の関数はI/O完了までブロックする
//

//
//-- UsbWrite():	指定バイト数分を全部送信しようと試みる
//		input:  pSrc       Write Buffer
//              dwSize     (I)Write size/(O)Written size
//              dwTmout    タイムアウト
//		return: 0 = OK/ != 0 エラー
//
int CUsbio::UsbWrite(LPCSTR pSrc, DWORD &dwSize, DWORD dwTmout/* = INFINITE*/)
{
	DWORD  dwBytesToWrite = dwSize;
	LPCSTR pStr = pSrc;
	DWORD  dwBytes;
	int    nErr = 0;

	for ( ; dwBytesToWrite; ) {
		dwBytes = dwBytesToWrite;
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
//-- Write():	指定バイト数に満たなくても送信できた分だけで戻る
//
//		input:  pBuf     送信するデータ
//              dwBytes  (I) データbyte数/(O) 送信したbyte数
//              dwTmout  タイムアウト(= INFINITE)
//      return: Error code: 0 = OK/ != 0 エラー
//		- 注意:
//			- 送信しようとしたbyte数を一度に送信できるとは限らない
//          - タイムアウトしたときは WAIT_TIMEOUT (=258) を返す
//
int CUsbio::Write(LPCSTR pBuf, DWORD &dwBytes, DWORD dwTmout /*= INFINITE*/)
{
TRACE("Write %s",pBuf);
	DWORD   dwWritten = 0;
	int     nErr;
	//
	//-- 送信:
	//
	//
	if (WriteDevice(pBuf, dwBytes, &dwWritten, dwTmout)) {
		nErr = 0;
	} else {
		nErr = GetLastError();
TRACE("USB WRITE ERROR!! %d\n", nErr);
	}
	//
	//-- 実際に送信できたbyte数を設定:
	//
	if (nErr == 0) {
		dwBytes = dwWritten;
	} else {
		dwBytes = 0;
	}
	return (nErr);
}

//
//-- UsbRead():	指定バイト数分か、改行文字/NULL文字まで読む
//
//		input:  pDst       Read Buffer
//				dwSize     (I) Buffer size/(O) Bytes to read
//              dwTmout    タイムアウト
//		return: 0 = OK/ != 0 エラー
//
//		<終了条件>
//		Text 転送:
//      - 行末検出
//		- Buffer sizeまで読んでも行末が検出できなかったときには
//        dwSize = 0 を返す(このときはBuffer Full)
//
//		Binary 転送:
//		- dwSize bytes 読んだ
//
int CUsbio::UsbRead(LPSTR pDst, DWORD &dwSize, DWORD dwTmout/* = INFINITE*/)
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
#if _DEBUG
if(dwSize < 256) {
TRACE(">>RECV %s\n",pStr);
} else {
TRACE(">>RECV 長い文字列\n");
}
#endif
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
	return(TRUE);
}

//
//-- Read():
//		input:  pBuf     受信データバッファ
//              dwBytes  (I) 受信したいbyte数/(O) 受信したbyte数
//              dwTmout  タイムアウト(= INFINITE)
//      return: 0 = OK / != 0 エラー
//		- 注意:
//			- 受信しようとしたbyte数を一度に受信できるとは限らない
//          - タイムアウトしたときは WAIT_TIMEOUT (=258) を返す
//
int CUsbio::Read(LPSTR pBuf, DWORD &dwBytes, DWORD dwTmout /*= INFINITE*/)
{
	int     nErr;
	DWORD   dwRead;
	char*	pDat = pBuf;

	nErr = 0;	////
	if (ReadDevice(pBuf, dwBytes, &dwRead, dwTmout)) {
		nErr = 0;
	} else {
		nErr = GetLastError();
TRACE("USB READ ERROR!! %d\n", nErr);
	}
	//
	//-- 実際に受信できたbyte数を設定:
	//
	if (nErr == 0) {
		dwBytes = dwRead;
	} else {
		dwBytes = 0;
	}

	return (nErr);
}

//
//-- protocolなしのread():	指定バイト数まで読み込む
//		input:  pDst       Read Buffer
//				dwSize     (I) Buffer size/(O) Bytes to read
//              dwTmout    タイムアウト
//		return: 0 = OK/ != 0 エラー
//
int CUsbio::ReadBinary(LPSTR pDst, DWORD &dwSize, DWORD dwTmout/* = INFINITE*/)
{
	DWORD  dwBytesToRead = dwSize;
	LPCSTR pStr = pDst;
	DWORD  dwBytes;
	int    nErr = 0;

	for ( ; dwBytesToRead; ) {
		if (!ReadDevice(pDst, dwBytesToRead, &dwBytes, dwTmout)) {
			//-- Readに失敗/タイムアウト:
			nErr = ::GetLastError();
			if(nErr == ERROR_HANDLE_EOF) {
				nErr = 0;	// 残りを読込み
			} else {
				break;
			}
		}
		pDst += dwBytes;			// 今回読めたbyte数だけpointerを進める
		dwBytesToRead -= dwBytes;	// まだ読めていないbyte数
	}
	dwSize = (DWORD)(pDst - pStr);			// 読めたbyte数
	return (nErr);
}



