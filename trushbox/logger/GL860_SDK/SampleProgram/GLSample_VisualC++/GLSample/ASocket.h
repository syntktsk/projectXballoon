//////////////////////////////////////////////////////////////////
//
// GL サンプルプログラム
//
//		Copyright (C) 2010 Graphtec CORP.
//
/////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////
//
// ASock.h: CASocketet クラスのインターフェイス
//   - 単純なWindows Socketの Wrapper class
//
//////////////////////////////////////////////////

#if !defined(AFX_ASOCK_H__325776F3_ED22_4A27_9903_EC1B0806B012__INCLUDED_)
#define AFX_ASOCK_H__325776F3_ED22_4A27_9903_EC1B0806B012__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CFifo;
class CASocket;

//////////////////////////////////////
//  CASocket クラスの I/O で使用する
//  WSABUF と互換性のあるバッファ
//
class CWsaBuf
{
	//
	// Construct/Destruct
	//
public:
	CWsaBuf(LPSTR pBuf = NULL, DWORD dwSize = 0);
	~CWsaBuf();
	//
	// Attribute
	//
private:
	// WSABUF と同一
	struct {
		DWORD  len;		// Bufferの長さ
		char   *buf;	// Bufferへのポインタ
	} m_tBuf;
	BOOL   m_bNew;		// デストラクタで使用
	//
	// Operation
	//
public:
	LPWSABUF BufAddr() const	{ return ((LPWSABUF)&m_tBuf); }
	//
	DWORD GetLen() const		{ return (m_tBuf.len); }
	int   SetStrings(LPCSTR pStr);
	int   Update(DWORD dwProg);
	char* GetStr() const	{ return(m_tBuf.buf);	}
};


///////////////////////////////////////
//  CASocket クラスの定義:
//
class CASocket
{
	//
	//  Construct/ Destruct
	//
public:
	CASocket();
	~CASocket();

	//
	//  Attribute
	//
private:
	//
	//-- Socket Handle:
	//
	SOCKET   m_hSocket;		// Socket Handle
	//
	HANDLE   m_hIEvent;		// 入力Overlap I/O用Event
	HANDLE   m_hOEvent;		// 出力Overlap I/O用Event
	//
	HANDLE   m_hIoMtx;		// I/O操作Lock用Mutex
	//
	HANDLE   m_hSEvent;			// FD_xxxx全体のEvent
	//
	//-- Overlap I/O 構造体:
	//
	LPWSAOVERLAPPED m_pIOvrlp;	// 入力I/O Overlapped
	LPWSAOVERLAPPED m_pOOvrlp;	// 出力I/O Overlapped
	//
	LPWSAOVERLAPPED_COMPLETION_ROUTINE m_pICmp;
	LPWSAOVERLAPPED_COMPLETION_ROUTINE m_pOCmp;
	//
	//-- Input FIFO:
	//
	CFifo *m_pRcvBuf;	// 受信キューへのPointer
	//
	//  Operations:
	//
public:
	//
	//-- WinSockのStartup/Release:
	//
	BOOL Startup(void);		// WSAStartup(): WinSock 2.0 固定
	void Cleanup(void);		// WSACleanup():
	//
	//-- 非同期(Async)に設定する
	//
	BOOL SetAsync(DWORD dwEvent = (FD_CONNECT|FD_CLOSE));
	//
	//-- Socketの作成:
	//
	BOOL Socket(int nSocketType = SOCK_STREAM);
	//
	//-- Bind():
	//
	BOOL Bind(UINT nSocketPort, LPCTSTR lpszSocketAddress = NULL);
	BOOL Bind(const SOCKADDR* lpSockAddr, int nSockAddrLen);
	//
	//-- Socket Handleの設定/切り離し:
	//
	void Attach(SOCKET sock) { m_hSocket = sock; }
	void Detach(void)        { m_hSocket = INVALID_SOCKET; }
	//
	//-- Socketの作成とBind:
	//
	BOOL Create(UINT nSocketPort = 0,
		        int nSocketType = SOCK_STREAM,
				LPCTSTR lpszSocketAddress = NULL);
	//
	//-- ソケットのクローズ:
	//
	void Close();
	BOOL Shutdown(int nPrm);
	//
	//-- 接続受付:
	//
	BOOL Listen(int nConnectionBacklog = SOMAXCONN);

	BOOL Accept(CASocket& rConnectedSocket,
		                SOCKADDR* lpSockAddr = NULL,
						int* lpSockAddrLen = NULL);
	//
	//-- 接続要求:
	//
	BOOL Connect(LPCTSTR lpszHostAddress, UINT nHostPort);
	BOOL Connect(const SOCKADDR* lpSockAddr, int nSockAddrLen);
	BOOL Connect(DWORD dwIp, UINT nPort);
	//
	//-- 操作の完了待ち:
	//
	int  WaitForConnect(DWORD dwTmout = INFINITE);
	int  WaitForClose(DWORD dwTmout = INFINITE);
	//
	//-- I/O完了待ち:
	//
	int  WaitForRecv(DWORD dwTmout = INFINITE);
	int  WaitForSend(DWORD dwTmout = INFINITE);
	//
	//-- 受信:
	//
	BOOL Recv(CWsaBuf *pBuf, LPDWORD pBytes);
	BOOL GetRecvOverlapResult(LPDWORD pBytes);
	//
	//-- 送信:
	//
	BOOL Send(CWsaBuf *pBuf, LPDWORD pBytes);
	BOOL GetSendOverlapResult(LPDWORD pBytes);
	//
	//-- ソケットオプションを設定する:
	//
	BOOL SetSockOpt(int opt, BOOL set = TRUE);
	//
	//-- WinSock Errorの取得:
	//
	int  GetLastError() const { return (WSAGetLastError()); }
	static BOOL GetErrMsg(int code, CString& msg);
	//
	//-- 上位レベルの入出力: Read()/Write()
	//
	int  Write(LPCSTR pBuf, DWORD &dwBytes, DWORD dwTmout = INFINITE);
	int  Read(LPSTR pBuf, DWORD &dwBytes, DWORD dwTmout = INFINITE);
	//
private:
	int  GetAsyncResult(long lMask, int wBit);
	//
	//-- Worker Thread:
	//
	static UINT RecvThread(LPVOID pParam);
};

#endif // !defined(AFX_ASOCK_H__325776F3_ED22_4A27_9903_EC1B0806B012__INCLUDED_)
