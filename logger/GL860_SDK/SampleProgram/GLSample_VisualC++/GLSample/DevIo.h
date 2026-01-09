//////////////////////////////////////////////////////////////////
//
// GL サンプルプログラム
//
//		Copyright (C) 2008 Graphtec CORP.
//
/////////////////////////////////////////////////////////////////

// DevIo.h: CDevIo クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DEVIO_H__280A453A_D46A_4649_AED2_1E9DAFF18B77__INCLUDED_)
#define AFX_DEVIO_H__280A453A_D46A_4649_AED2_1E9DAFF18B77__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CASocket;
class CUsbio;

class CDevIo
{
public:
	CDevIo(char bSelIf);
	virtual ~CDevIo();

	BOOL Open(DWORD nIpAddress, DWORD nPort);
	BOOL Open(char *pIdCmd, DWORD nMachineId);
	BOOL Close();
	int  Read(LPSTR pBuf, DWORD &dwBytes, DWORD dwTmout = INFINITE);
	int  ReadLn(LPSTR pDst, DWORD &dwSize, DWORD dwTmout = INFINITE);
	int  ReadBinary(LPSTR pDst, DWORD &dwSize, DWORD dwTmout = INFINITE);
	int  Write(LPCSTR pBuf, DWORD &dwBytes, DWORD dwTmout = INFINITE);
	int  WriteLn(LPCSTR pSrc, DWORD dwSize = 4096, DWORD dwTmout = INFINITE);
	int  SendQuery(LPCSTR pCmd, LPSTR pRes);
	int  SendQuery(CString &pCmd, CString &pRes);
	int  SendQuery(LPCSTR pCmd, CString &pRes);
	int  SendCommand(LPCSTR pCmd);
	int  SendCommand(CString &pCmd);

public:
	CASocket	*m_pTcp;		// TCP/IP通信クラス
	CUsbio		*m_pUsb;		// USB通信クラス

	char		m_nSelIf;		// I/Fセレクト

	DWORD		m_nIpAddress;	// IPアドレス
	DWORD		m_nPort;		// PORT番号

	DWORD		m_nMachineId;	// 機器ID

	char		m_sHostAddr[32]; // ホストアドレス文字列
};


enum {IF_TCP_IP,IF_USB};



#endif // !defined(AFX_DEVIO_H__280A453A_D46A_4649_AED2_1E9DAFF18B77__INCLUDED_)
