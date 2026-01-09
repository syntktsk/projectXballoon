//////////////////////////////////////////////////////////////////
//
// GL サンプルプログラム
//
//		Copyright (C) 2010 Graphtec CORP.
//
/////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////
//
// UsbSocket.h: CUsbio クラスのインターフェイス
//   - USBをWinsock風に使用するための Wrapper class
//
////////////////////////////////////////////////////////

#if !defined(USBIO_H__325776F3_ED22_4A27_9903_EC1B0806B012__INCLUDED_)
#define USBIO_H__325776F3_ED22_4A27_9903_EC1B0806B012__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <setupapi.h>
#include <basetyps.h>

#include "gtcusbr.h"

class CFifo;


// エラー定義
#define	USBERR_INVALID_HANDLE	0x20000001
#define	USBERR_READ_ERROR		0x20000002
#define	USNERR_WRITE_ERROR		0x20000003

///////////////////////////////////////
//  CUsbio クラスの定義:
//
class CUsbio
{
	//
	//  Construct/ Destruct
	//
public:
	CUsbio();
	~CUsbio();

	//
	//  Attribute
	//
private:
	//
	//-- USB I/F
	//
	HANDLE m_hUsb;
public:
	char	*m_pIdCmd;		// USB IDコマンド文字列
	//
	//  Operations:
	//
public:
	//
	//-- ソケットのクローズ:
	//
	BOOL Create();
	void Close();
	//
	//-- 接続要求:
	//
	BOOL Connect(int nId);
	//
	//-- 送受信
	//
	BOOL ReadDevice(LPCSTR pBuf, DWORD dwReadBytes, LPDWORD pBytes, DWORD dwTmout);
	BOOL WriteDevice(LPCSTR pBuf, DWORD dwWriteBytes, LPDWORD pBytes, DWORD dwTmout);

	//
	//-- Errorの取得:
	//
	int  GetLastError() const { return (::GetLastError()); }
	static BOOL GetErrMsg(int code, CString& msg);
	//
	//-- 上位レベルの入出力: Read()/Write()
	//
	int  UsbWrite(LPCSTR pSrc, DWORD &dwSize, DWORD dwTmout = INFINITE);
	int  Write(LPCSTR pBuf, DWORD &dwBytes, DWORD dwTmout = INFINITE);
	int  UsbRead(LPSTR pDst, DWORD &dwSize, DWORD dwTmout = INFINITE);
	int  Read(LPSTR pBuf, DWORD &dwBytes, DWORD dwTmout = INFINITE);
	int  ReadBinary(LPSTR pDst, DWORD &dwSize, DWORD dwTmout = INFINITE);
	//
private:

};

#endif // !defined(USBIO_H__325776F3_ED22_4A27_9903_EC1B0806B012__INCLUDED_)
