/////////////////////////////////////////////
//
// GL サンプルプログラム
//
//		Copyright (C) 2010 Graphtec CORP.
//
// ASock.cpp: CASocket クラス
//
//		単純なWindows SocketのWrapperクラス
//                  WinSock 2.0
//                  非同期ソケット
//                  非同期I/O
//
//////////////////////////////////////////////

#include "stdafx.h"

#include "ASocket.h"
#include "Fifo.h"

#define	BUFSZ	8192

/////////////////////////////////
// 構築/消滅
/////////////////////////////////

//
//-- コンストラクタ:
//
CASocket::CASocket()
{
	//
	//-- Socket/Event Handleに初期値を設定:
	//
	m_hSocket = INVALID_SOCKET;		// Socket Handle
	//
	m_hIEvent = NULL;
	m_hOEvent = NULL;
	m_hIoMtx = NULL;
	m_hSEvent = NULL;
	//
	m_pICmp = NULL;
	m_pOCmp = NULL;
	m_pIOvrlp = NULL;
	m_pOOvrlp = NULL;
	m_pRcvBuf = NULL;	// 受信データキュー
}

//
//-- デストラクタ:
//
CASocket::~CASocket()
{
	//
	//-- 同期オブジェクトの破棄:
	//
	if (m_hIoMtx != NULL) {
		::CloseHandle(m_hIoMtx);
	}
	if (m_hIEvent != NULL) {
		::CloseHandle(m_hIEvent);
	}
	if (m_hOEvent != NULL) {
		::CloseHandle(m_hOEvent);
	}
	if (m_hSEvent != NULL) {
		::CloseHandle(m_hSEvent);
	}
	//
	//-- new で確保した領域をdelete
	//
	if (m_pIOvrlp) {
		delete m_pIOvrlp;
		m_pIOvrlp = NULL;
	}
	if (m_pOOvrlp) {
		delete m_pOOvrlp;
		m_pOOvrlp = NULL;
	}
	if (m_pRcvBuf) {
		delete m_pRcvBuf;
		m_pRcvBuf = NULL;
	}
}

/////////////////////////////////
// 操作関数:
//

//
//-- WinSockのStartup: Ver 2.0で初期化する
//
BOOL CASocket::Startup()
{
	WORD   VerReq;
	WSADATA wsaData;

	VerReq = MAKEWORD(2, 0);		// WinSock 2.0指定
	if (::WSAStartup(VerReq, &wsaData)) {
		//
		//-- StartUpに失敗:
		//
		ASSERT(0);
		return (FALSE);
	}
	//
	//-- WinSock 2.0を指定した場合は、1.1しか使えなくてもエラーに
	//   ならないので、さらにチェックが必要
	//
	if (wsaData.wVersion != VerReq) {
		//
		//-- WinSock 1.1 しか使用できない:
		//
		::WSACleanup();	// Ver 1.1 でStartしてしまったので
		//
		ASSERT (0);
		return (FALSE);
	}
	return (TRUE);
}

//
//-- WinSockのCleanup: Resourceの解放
//
void CASocket::Cleanup()
{
	::WSACleanup();
}

//
//-- ソケットの作成:
//		input:  nSocketType     SOCK_STREAM | SOCL_DGRAM
//
BOOL CASocket::Socket(int nSocketType /* = SOCK_STREAM */)
{
	int nErr;
	//
	//-- WSASocket()でソケットを作成:
	//     SOCKET WSASocket(
	//         int  af,           // Address family(AF_INET)
	//         int  type,         // SOCK_STREAM | SOCK_DGRAM
	//         int  protocol,     // IPPROTO_TCP | IPPROTO_UDP
	//         LPWSAPROTOCOLINFO lpProtocolInfo,   // = NULL
	//         GROUP g,           // Reserved
	//         DWORD dwFlags);	  // WSA_FLG_OVERLAPPED
	//     return: Socket Handle
	//
	int  protocol;

	if (nSocketType == SOCK_STREAM) {
		protocol = IPPROTO_TCP;			// TCP
	} else {
		protocol = IPPROTO_UDP;			// UDP
	}
	m_hSocket = ::WSASocket(AF_INET,
		                    nSocketType,
						    protocol,
						    NULL,
						    0,
						    WSA_FLAG_OVERLAPPED);
	if (m_hSocket == INVALID_SOCKET) {
		ASSERT (0);
		return (FALSE);
	}
	//
	//-- Overlap I/O に設定する前提なので、ここで関連する
	//   Event および構造体の初期化も実行する
	//
	if ((m_hIoMtx = ::CreateMutex(NULL, FALSE, NULL)) == NULL) {
		nErr = ::GetLastError();
		ASSERT(0);
		return (FALSE);
	}
	if ((m_hSEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL)) == NULL) {
		nErr = ::GetLastError();
		ASSERT (0);
		return (FALSE);
	}
	if ((m_hIEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL)) == NULL) {
		nErr = ::GetLastError();
		ASSERT(0);
		return (FALSE);
	}
	if ((m_hOEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL)) == NULL) {
		nErr = ::GetLastError();
		ASSERT(0);
		return (FALSE);
	}
	//
	//-- Overlap I/O 構造体の初期化: 上記で生成したEventを指定
	//
	try {
		m_pIOvrlp = new WSAOVERLAPPED;
		m_pOOvrlp = new WSAOVERLAPPED;
	}
	catch (CMemoryException* ex) {
		ex->ReportError();
		ex->Delete();
		goto ErrExit;
	}
	//
	m_pIOvrlp->hEvent = m_hIEvent;	// Event Handleを設定
	m_pOOvrlp->hEvent = m_hOEvent;	//
	//
	//-- 受信ストリームのための Queue を生成:
	//
	try {
		m_pRcvBuf = new CFifo(BUFSZ);	//
	}
	catch (CMemoryException* ex) {
		ex->ReportError();
		ex->Delete();
		goto ErrExit;
	}
	//
	return (TRUE);

ErrExit:
	if (m_pIOvrlp) {
		delete m_pIOvrlp;
		m_pIOvrlp = NULL;
	}
	if (m_pOOvrlp) {
		delete m_pOOvrlp;
		m_pOOvrlp = NULL;
	}
	if (m_pRcvBuf) {
		delete m_pRcvBuf;
		m_pRcvBuf = NULL;
	}
	return(FALSE);
}

//
//-- 非同期ソケットに設定する:
//		内部的に Event Object を生成する
//
BOOL CASocket::SetAsync(DWORD dwEvent /* =(FD_CONNECT|FD_CLOSE) */)
{
	//
	//-- Socket操作完了通知用のWSAEvent Objectを生成:
	//
	//
	//-- int WSAEventSelect(SOCKET   sock,
	//                      WSAEVENT hSEvent,
	//                      long     lEvents);
	//   return: 0 or SOCKET_ERROR
	//
	int  nRet;
	nRet = ::WSAEventSelect(m_hSocket, m_hSEvent, dwEvent);
	return (nRet == 0);
}

//
//-- ソケットのバインド:
//		input:  lpSockAddr    SOCKADDR_IN構造体をcastする
//                            内容は下記
//              nSockAddrLen  sizeof(lpSockAddr)
//		return: Error
//
//			struct address_in {
//				short          sin_family;	// Address famiry(AF_INET)
//				unsigned short sin_port;	// IP Port番号
//				struct in_addr sin_addr;	// IP Address
//				char           sin_zero[8];	// Padding
//			};
//
//			struct in_addr {
//				union {
//					struct {
//						unsigned char s_b1, s_b2, s_b3, s_b4;
//					} S_un_b;
//					struct {
//						unsigned short s_w1, s_w2;
//					} S_un_w;
//					unsigned long  S_addr;
//				} S_un;
//			};
//
//
BOOL CASocket::Bind(const SOCKADDR* lpSockAddr, int nSockAddrLen)
{
	int  nRet;

	nRet = ::bind(m_hSocket, lpSockAddr, nSockAddrLen);
	if (nRet) {
		int nErr = WSAGetLastError();
	}
	return (nRet == 0);
}

//
//-- Bind() その2:
//		input:  nSocketPort        SocketにbindするPort番号
//				lpszSocketAddress  接続を許可するIPアドレスマスク
//
BOOL CASocket::Bind(UINT nSocketPort,
					   LPCTSTR lpszSocketAddress /* = NULL */)
{
	SOCKADDR_IN sockAddr;
	::memset(&sockAddr, 0, sizeof(sockAddr));

	LPSTR lpszAscii = (LPSTR)lpszSocketAddress;
	sockAddr.sin_family = AF_INET;		// つねにAF_INET

	if (lpszAscii == NULL) {
		//
		//-- IP address maskがNULLのときは全てのIPを許可:
		//		- u_long htonl(u_long hostlong);
		//		Network Byte-orderのu_longを返す関数
		//
		sockAddr.sin_addr.s_addr = ::htonl(INADDR_ANY);
	} else {
		//
		//-- IP address maskが指定された場合:
		//		- unsigned long inet_addr(const char FAR *cp);
		//
		DWORD lResult = ::inet_addr(lpszAscii);
		if (lResult == INADDR_NONE) {
			//
			//-- 指定されたaddressが不正なIP address:
			//
			::WSASetLastError(WSAEINVAL);
			return (FALSE);
		}
		sockAddr.sin_addr.s_addr = lResult;		// IP maskを設定
	}
	//
	//-- Port番号:
	//		- u_short htons(u_short hostshort);
	//		Network Byte-orderのu_shortを返す関数
	//
	sockAddr.sin_port = ::htons((u_short)nSocketPort);
	//
	//-- Bind()その1を呼ぶ:
	//
	return Bind((const SOCKADDR*)&sockAddr, sizeof(sockAddr));
}

//
//-- Create() 作成とバインド:
//		input:  nSocketPort        Port番号
//              nSocketType        SOCK_STREAM | SOCK_DGRAM
//              lpszSocketAddress  接続を許可するIP address mask
//
BOOL CASocket::Create(UINT nSocketPort          /* = 0 */,
						 int nSocketType           /* = SOCK_STREAM */,
						 LPCTSTR lpszSocketAddress /* = NULL */)
{
	//
	//-- Socketを生成:
	//
	if (Socket(nSocketType)) {
		if (Bind(nSocketPort, lpszSocketAddress)) {
			return (TRUE);
		}
		Close();
	}
	return (FALSE);
}

//
//-- ソケットのクローズ:
//
void CASocket::Close()
{
	if (m_hSocket != INVALID_SOCKET) {
		::closesocket(m_hSocket);
		m_hSocket = INVALID_SOCKET;
	}
}

//
//-- ソケットオプションの設定:
//		input:   optname   SO_BROADCAST...
//               set       有効に設定/無効に設定
//		return:  Error code(setsockopt()の返値)
//
BOOL CASocket::SetSockOpt(int optname, BOOL set /* = TRUE */)
{
	int   nSet, nRet;

	if (set) {
		nSet = 1;
	} else {
		nSet = 0;
	}
	nRet = ::setsockopt(m_hSocket, SOL_SOCKET, optname,
		             (const char *)&nSet, sizeof(int));
	return (nRet == 0);
}

//
//-- Shutdown: Closeの前に実行/送受信を禁止する
//		input: nPrm  無効にする機能
//                   SD_RECEIVE: 受信を無効にする
//                   SD_SEND:    送信を無効にする
//                   SD_BOTH:    送受信を無効にする
//
BOOL CASocket::Shutdown(int nPrm)
{
	int  nRet = ::shutdown(m_hSocket, nPrm);
	return (nRet == 0);
}

//
//-- 接続受付:
//		input:  rConnectedSocket  Create()/Socket()+Bind()したSocket
//                                のハンドル
//              lpSockAddr        SOCKADDR_IN構造体（結果の格納用)
//              lpSockAddrLen     SockAddrのサイズ
//
BOOL CASocket::Accept(CASocket& rConnectedSocket,
						 SOCKADDR*    lpSockAddr        /* = NULL */,
						 int*         lpSockAddrLen     /* = NULL */)
{
	rConnectedSocket.Attach(INVALID_SOCKET);
	//
	//-- accept()してみる:
	//		- lpSockAddr, lpSockAddrLenにNULLを指定した場合は
	//        接続した相手の情報を返さない
	//
	SOCKET hTemp = ::accept(m_hSocket, lpSockAddr, lpSockAddrLen);
	if (hTemp == INVALID_SOCKET) {
		DWORD dwProblem = ::GetLastError();
		::SetLastError(dwProblem);
	}
	rConnectedSocket.Attach(hTemp);
	return (hTemp != INVALID_SOCKET);
}

//
//-- 接続要求待ち:
//
BOOL CASocket::Listen(int nConnectionBacklog /* = SOMAXCONN */)
{
	int  nRet = ::listen(m_hSocket, nConnectionBacklog);
	return (nRet == 0);
}

//
//-- 接続要求:
//
BOOL CASocket::Connect(const SOCKADDR* lpSockAddr, int nSockAddrLen)
{
	int nRet = ::connect(m_hSocket, lpSockAddr, nSockAddrLen);
	return (nRet == 0);
}

//
//-- 接続要求(その2):
//		input:  lpszHostAddress  IP address or Host名文字列
//              nHostPort        ServerのPort番号
//
BOOL CASocket::Connect(LPCTSTR lpszHostAddress, UINT nHostPort)
{
	ASSERT(lpszHostAddress != NULL);
	SOCKADDR_IN sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));

	LPSTR lpszAscii = (LPTSTR)lpszHostAddress;
	sockAddr.sin_family = AF_INET;						// つねにAF_INET
	sockAddr.sin_addr.s_addr = inet_addr(lpszAscii);	// u_longのIPにする

	if (sockAddr.sin_addr.s_addr == INADDR_NONE) {
		//
		//-- IP addressが変: host名文字列かも?
		//
		LPHOSTENT lphost;

		lphost = ::gethostbyname(lpszAscii);
		if (lphost != NULL) {
			sockAddr.sin_addr.s_addr = ((LPIN_ADDR)lphost->h_addr)->s_addr;
		} else {
			//
			//-- IP addressでもhost名文字列でもない:
			//
			::WSASetLastError(WSAEINVAL);
			return (FALSE);
		}
	}
	//
	//-- Server Port番号:
	//
	sockAddr.sin_port = ::htons((u_short)nHostPort);
	//
	//-- Connect その1を実行:
	//
	return Connect((SOCKADDR*)&sockAddr, sizeof(sockAddr));
}

//
//-- 接続要求(その3):
//		input:  dwIp   IP address
//              nPort  Port番号
//
BOOL CASocket::Connect(DWORD dwIp, UINT nPort)
{
	SOCKADDR_IN sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));
	//
	sockAddr.sin_family = AF_INET;					// つねにAF_INET
	sockAddr.sin_addr.s_addr = ::htonl(dwIp);		// IPアドレス
	sockAddr.sin_port = ::htons((u_short)nPort);	// Port番号
	//
	return (Connect((SOCKADDR*)&sockAddr, sizeof(sockAddr)));
}

//
//-- 受信: 非同期Socket/非同期I/O
//		input:  pBuf   WSABUFへのポインタ(注意)
//              pBytes 受信byte数を受け取る変数(即時完了した場合のみ有効)
//
BOOL CASocket::Recv(CWsaBuf *pBuf, LPDWORD pBytes)
{
	//
	//-- WSARecv(SOCKET           s,					// Socket
	//           LPWSABUF         lpBuffers,			// 入力Buffer
	//           DWORD            dwBufferCount,		// 入力Buffer数
	//           LPDWORD          lpNumberOfBytesRecvd,	// 受信バイト数
	//           LPDWORD          lpFlags,				// 使用しない
	//           LPWSAOVERLAPPED  lpOverlapped,			// Overlapped 構造体
	//           LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
	//           );
	//
	//   注: LPWSABUF  typedef struct _WSABUF {
	//                     u_long   len;    // Bufferの長さ
	//                     char    *buf;    // Bufferへのポインタ
	//                 } WSABUF, *LPWSABUF;
	//
	//       複数のバッファを使用することが想定されているが、受信関数では
	//       単一のバッファのみ使用することとする。
	//
	DWORD  dwFlag = 0;		// NOT USING

	int nRet = ::WSARecv(m_hSocket,		// Socket
		               pBuf->BufAddr(),	// Buffer
			   	       1,				// Buffer数は 1 固定とする
				       pBytes,			// 受信byte数を返す変数(直ぐ完了したときのみ)
				       &dwFlag,			// NOT USING
				       m_pIOvrlp,		// Overlapped 構造体(別途生成済み)
				       m_pICmp);		// I/O 完了したときに呼ばれる関数 (別途設定)
	return (nRet == 0);
}

//
//-- 受信Overlap I/Oの結果を取得:
//		return: TRUE=成功/ FALSE=失敗
//			FALSEの場合は WSAGetLastError() でエラーの内容を取得
//
BOOL CASocket::GetRecvOverlapResult(LPDWORD pBytes)
{
	//
	//-- Overlapped I/O で Event を使用する場合に、結果を取得
	//
	DWORD  dwFlag = 0;		// NOT USING

	BOOL  bRet = ::WSAGetOverlappedResult(
		                m_hSocket,		// Socket
						m_pIOvrlp,		// Overlapped構造体へのポインタ
						pBytes,			// 受信byte数を返す変数
						FALSE,			// FALSE 固定とすること
						&dwFlag);		// NOT USING
	return (bRet);		//// WSAGetOverlappedResult() はBOOLを返す
}

//
//-- 送信: 非同期Socket/非同期I/O
//		input:  pBuf   WSABUFへのポインタ(注意)
//              pBytes 送信byte数を受け取る変数(即時完了した場合のみ有効)
//
BOOL CASocket::Send(CWsaBuf *pBuf, LPDWORD pBytes)
{
	//
	//-- WSASend(SOCKET  s,						// Socket
	//           LPWSABUF lpBuffers,			// 送信データ(WSABUFに注意)
	//           DWORD     dwBufferCount,		//
	//           LPDWORD   lpNumberOfBytesSent,
	//           DWORD     dwFlags,
	//           LPWSAOVERLAPPED  lpOverlapped,
	//           LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
	//           );
	//
	//
TRACE("<<SEND %s", pBuf->GetStr());
	int nRet = ::WSASend(m_hSocket,		// Socket
		               pBuf->BufAddr(),	// Buffer
			   	       1,				// Buffer数は 1 固定とする
				       pBytes,			// 受信byte数を返す変数(直ぐ完了したときのみ)
				       0,				// NOT USING
				       m_pOOvrlp,		// Overlapped 構造体(別途生成済み)
				       m_pOCmp);		// I/O 完了したときに呼ばれる関数 (別途設定)
	return (nRet == 0);
}

//
//-- 送信Overlap I/Oの戻り値を取得:
//		return: TRUE=成功/ FALSE=失敗
//			FALSEの場合は WSAGetLastError() でエラーの内容を取得
//
BOOL CASocket::GetSendOverlapResult(LPDWORD pBytes)
{
	//
	//-- Overlapped I/O で Event を使用する場合に、結果を取得
	//
	DWORD  dwFlag = 0;		// NOT USING

	BOOL  bRet = ::WSAGetOverlappedResult(
		                m_hSocket,		// Socket
						m_pOOvrlp,		// Overlapped構造体へのポインタ
						pBytes,			// 受信byte数を返す変数
						FALSE,			// FALSE 固定とすること
						&dwFlag);		// NOT USING
	return (bRet);
}

////////////////////////
//  I/Oの完了:
//
//-- 受信Overlap I/O の完了を待つ:
//
int CASocket::WaitForRecv(DWORD dwTmout /* = INFINITE */)
{
	int nErr = ::WaitForSingleObject(m_hIEvent, dwTmout);
	return (nErr);
}

//
//-- 送信Overlap I/O の完了を待つ:
//
int CASocket::WaitForSend(DWORD dwTmout /* = INFINITE */)
{
	int nErr = ::WaitForSingleObject(m_hOEvent, dwTmout);
	return (nErr);
}


///////////////////////////////////////////////////////////////
// WaitForSingleObject():
//   return: WAIT_FAILED = 関数の実行が失敗
//           WAIT_TIMEOUT = タイムアウト
//           WAIT_OBJECT_0 = オブジェクトがシグナル状態(成功)
//           WAIT_ABANDONED = Mutexを所有していたスレッドが終了
//

//
//-- Connect/Acceptを待つ:
//
int CASocket::WaitForConnect(DWORD dwTmout /* = INFINITE */)
{
	WSANETWORKEVENTS  netEvents;

	int nErr = ::WaitForSingleObject(m_hSEvent, dwTmout);
	switch (nErr) {
	case WAIT_OBJECT_0:
		//
		//-- 待ちに成功: 発生イベントの判別(FD_CONNECT|FDCLOSE)
		//
		if ((nErr = ::WSAEnumNetworkEvents(m_hSocket, m_hSEvent, &netEvents)) == SOCKET_ERROR) {
			ASSERT(0);
		} else {
			//
			//-- Connect に対するエラーコード取得:
			//
			if (netEvents.lNetworkEvents & FD_CONNECT) {
				nErr = netEvents.iErrorCode[FD_CONNECT_BIT];
			} else {
				//
				//-- CONNECT Event ではない:
				//
				nErr = WSAECONNRESET;
				::SetLastError(nErr);
			}
		}
		break;
	case WAIT_TIMEOUT:
		//
		//-- タイムアウト:
		//
		::SetLastError(nErr);
		break;
	default:
		nErr = ::GetLastError();
		ASSERT(0);
	}
	return (nErr);
}

//
//-- Close(切断)を待つ:
//
int CASocket::WaitForClose(DWORD dwTmout /* = INFINITE */)
{
	WSANETWORKEVENTS  netEvents;

	int nErr = ::WaitForSingleObject(m_hSEvent, dwTmout);
	switch (nErr) {
	case WAIT_OBJECT_0:
		//
		//-- 待ちに成功: 発生イベントの判別(FD_CONNECT|FDCLOSE)
		//
		if ((nErr = ::WSAEnumNetworkEvents(m_hSocket, m_hSEvent, &netEvents)) == SOCKET_ERROR) {
			ASSERT(0);
		} else {
			//
			//-- Close に対するエラーコード取得:
			//
			if (netEvents.lNetworkEvents & FD_CLOSE) {
				nErr = netEvents.iErrorCode[FD_CLOSE_BIT];
			} else {
				//
				//-- CLOSE Event ではない:
				//
				nErr = WSAEISCONN;			/// 適当なエラーコード
				::SetLastError(nErr);
			}
		}
		break;
	case WAIT_TIMEOUT:
		//
		//-- タイムアウト:
		//
		::SetLastError(nErr);
		break;
	default:
		nErr = ::GetLastError();
		ASSERT(0);
	}
	return (nErr);
}



///////////////////////////////////////////
//  Socketより上位レベルのI/O関数:
//    Read()/Write()関数に近い仕様とする
//    以下の関数はI/O完了までブロックする
//

//
//-- Write():
//		input:  pBuf     送信するデータ
//              dwBytes  (I) データbyte数/(O) 送信したbyte数
//              dwTmout  タイムアウト(= INFINITE)
//      return: Error code: 0 = OK/ WSAExxxxxx
//              起きる可能性のあるError
//                WSAENETDOWN:     The network subsystem has failed.
//                WSAENETRESET:
//                WSAECONNABORTED:
//                WSAECONNRESET:
//		- 注意:
//			- 送信しようとしたbyte数を一度に送信できるとは限らない
//          - タイムアウトしたときは WAIT_TIMEOUT (=258) を返す
//
int CASocket::Write(LPCSTR pBuf, DWORD &dwBytes, DWORD dwTmout /*= INFINITE*/)
{
//TRACE("Write %s\n",pBuf);
	DWORD   dwWritten = 0;
	int     nErr;
	//
	//-- LPCSTR --> CWsaBuf:
	//
	CWsaBuf  cBuf((LPSTR)pBuf, dwBytes);	// 送信バッファを生成:
	//
	//-- 他に送信していない かどうか:
	//
	nErr = ::WaitForSingleObject(m_hIoMtx, dwTmout);
	switch (nErr) {
	case WAIT_OBJECT_0:
	case (WAIT_OBJECT_0 + 1):
		//
		//-- 正常に待ちが成立: GetWrite
		break;						// 正常に待ちが成立
	case WAIT_FAILED:
		nErr = ::GetLastError();	// 待機に失敗:
		dwBytes = dwWritten;		//
		ASSERT(0);
		goto Exit;
	case WAIT_TIMEOUT:				// WAIT_TIMEOUT = 258
		::SetLastError(nErr);		//
		dwBytes = dwWritten;		//
		goto Exit;
	default:
		dwBytes = dwWritten;		//
		ASSERT(0);					// このスレッドが死んでいる
		nErr = -1;					// 有り得ない
		goto Exit;
	}
	//
	//-- 送信:
	//
	//
	if (!Send(&cBuf, &dwWritten)) {
		if ((nErr = ::GetLastError()) == WSA_IO_PENDING) {
			//
			//-- 送信のOverlap I/Oが正常に起動された:
			//     - タイムアウト付でI/Oの完了を待つ
			//
			switch (WaitForSend(dwTmout)) {
			case WAIT_OBJECT_0:
				//
				//-- 送信が完了: 全部送信できたとは限らない
				//     - 結果を取得する
				//
				if (!GetSendOverlapResult(&dwWritten)) {
					nErr = ::GetLastError();
					ASSERT(0);		// Overlapped Resultの取得に失敗
				} else {
					nErr = 0;				// 成功:
				}
				break;
			case WAIT_TIMEOUT:
				//
				//-- タイムアウトした:
				//
				nErr = WAIT_TIMEOUT;		//
				::SetLastError(nErr);		// Error code設定
				break;
			default:
				nErr = GetLastError();
				ASSERT(0);				// 完了待ちに失敗
			}
		} else {
			//
			//-- 送信に失敗: これはある(ネットワーク切断など)
			//
			nErr = GetLastError();	// 実際に起きるError
		}
	} else {
		//
		//-- 送信が即時終了した場合:
		//
		nErr = 0;					// 成功:
	}
	//
	//-- Mutexを返却:
	//
Exit:
	if (!::ReleaseMutex(m_hIoMtx)) {
		nErr = ::GetLastError();
		ASSERT(0);
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
//-- Read():
//		input:  pBuf     受信データバッファ
//              dwBytes  (I) 受信したいbyte数/(O) 受信したbyte数
//              dwTmout  タイムアウト(= INFINITE)
//      return: 0 = OK/ WSAExxxxxx
//              起きる可能性のあるError
//                WSAENETDOWN:     The network subsystem has failed.
//                WSAENETRESET:
//                WSAECONNABORTED:
//                WSAECONNRESET:
//		- 注意:
//			- 受信しようとしたbyte数を一度に受信できるとは限らない
//          - タイムアウトしたときは WAIT_TIMEOUT (=258) を返す
//
int CASocket::Read(LPSTR pBuf, DWORD &dwBytes, DWORD dwTmout /*= INFINITE*/)
{
	int     nErr, nDat;
	DWORD   dwQueueDat, dwRead;

	//
	//-- キューに受信データがあるか?
	//
	nErr = 0;	////
	if (m_pRcvBuf->Size() == 0) {
		//
		//-- キューに受信データがない: 受信するまで待つ
		//
		const DWORD RecvBufSize = BUFSZ/2;
		CWsaBuf  cBuf(NULL, RecvBufSize);	// Localな受信バッファを生成

		if (!Recv(&cBuf, &dwRead)) {
			if ((nErr = ::GetLastError()) == WSA_IO_PENDING) {
				//
				//-- 受信のOverlap I/Oが正常に起動された:
				//		- タイムアウト付でI/O完了を待つ
				//
				switch (WaitForRecv(dwTmout)) {
				case WAIT_OBJECT_0:
					//
					//-- 受信が完了: 結果を取得して受信データを全部キューに詰める
					//
					if (!GetRecvOverlapResult(&dwRead)) {
						nErr = ::GetLastError();
						ASSERT(0);
					} else {
						nErr = 0;		// 成功:
					}
					break;
				case WAIT_TIMEOUT:
					//
					//-- タイムアウト:
					//
					nErr = WAIT_TIMEOUT;		//
					::SetLastError(nErr);		//
					break;
				default:
					nErr = ::GetLastError();	//
					ASSERT(0);
				}
			} else {
				//
				//-- ネットワークの切断などで受信に失敗:
				//
				nErr = ::GetLastError();	//// 実際に起きるエラー
			}
		} else {
			//
			//-- 即時終了:
			//
			nErr = 0;		// 成功:
		}
		//
		//-- 受信に成功した場合は、データをすべてキューに詰める:
		//
		if (nErr == 0 && dwRead > 0) {
			char *pRcv = cBuf.BufAddr()->buf;	////
			//
			while (dwRead--) {
				m_pRcvBuf->Put(*pRcv++);
			}
		}
	}
	if (!nErr) {
		//
		//-- 受信操作の場合は受信キューから:
		//     - 受信キュー(CFifo) の操作はスレッドセーフ(な筈)
		//
		if ((dwQueueDat = m_pRcvBuf->Size()) > 0) {
			//
			//-- キューに受信データがある:
			//
			if (dwQueueDat > dwBytes) {
				dwRead = dwBytes;
			} else {
				dwRead = dwQueueDat;
			}
			//
			//-- QueueからdwRead bytres読む:
			//
			dwBytes = dwRead;				//// 実際に読めたbyte数
			while (dwRead--) {
				m_pRcvBuf->Get(nDat);
				*pBuf++ = (nDat & 0xff);
			}
		} else {
			dwBytes = 0;
		}
	} else {
		dwBytes = 0;
	}
	return (nErr);
}


//
//-- WinSockのエラーを取得:
//
/* static */
BOOL CASocket::GetErrMsg(int code, CString& msg)
{
	if (code == 0) {
		return (TRUE);
	}
	switch (code) {
	case WSAEACCES:
		msg = "10013: Permission denied.";
		break;
	case WSAEADDRINUSE:
		msg = "10048: Address already in use.";
		break;
	case WSAEADDRNOTAVAIL:
		msg = "10049: Cannot assign requested address.";
		break;
	case WSAEAFNOSUPPORT:
		msg = "10047: Address family not supported by protocol family.";
		break;
	case WSAEALREADY:
		msg = "10037: Operatiion already in progress.";
		break;
	case WSAECONNABORTED:
		msg = "10053: Software caused connection abort.";
		break;
	case WSAECONNREFUSED:
		msg = "10061: Connection refused.";
		break;
	case WSAECONNRESET:
		msg = "10054: Connection reset by peer.";
		break;
	case WSAEDESTADDRREQ:
		msg = "10039: Destination address required.";
		break;
	case WSAEFAULT:
		msg = "10014: Bad address.";
		break;
	case WSAEHOSTDOWN:
		msg = "10064: Host is down.";
		break;
	case WSAEHOSTUNREACH:
		msg = "10065: No route to host.";
		break;
	case WSAEINPROGRESS:
		msg = "10036: Operation now in progress.";
		break;
	case WSAEINTR:
		msg = "10004: Interrupt function call.";
		break;
	case WSAEINVAL:
		msg = "10022: Invalid argument.";
		break;
	case WSAEISCONN:
		msg = "10056: Socket is already connected.";
		break;
	case WSAEMFILE:
		msg = "10024: Too many open files.";
		break;
	case WSAEMSGSIZE:
		msg = "10040: Message too long.";
		break;
	case WSAENETDOWN:
		msg = "10050: Newwork is down.";
		break;
	case WSAENETRESET:
		msg = "10052: Network dropped connection on reset.";
		break;
	case WSAENETUNREACH:
		msg = "10051: Network is unreachable.";
		break;
	case WSAENOBUFS:
		msg = "10055: No buffer space available.";
		break;
	case WSAENOPROTOOPT:
		msg = "10042: Bad protocol option.";
		break;
	case WSAENOTCONN:
		msg = "10057: Socket is not connectied.";
		break;
	case WSAENOTSOCK:
		msg = "10038: Socket operation on nonsocket.";
		break;
	case WSAEOPNOTSUPP:
		msg = "10045: Operation not supported.";
		break;
	case WSAEPFNOSUPPORT:
		msg = "10046: Protocol family not supported.";
		break;
	case WSAEPROCLIM:
		msg = "10067: Too many processes.";
		break;
	case WSAEPROTONOSUPPORT:
		msg = "10043: Protocol not supported.";
		break;
	case WSAEPROTOTYPE:
		msg = "10041: Protocol wrong type for socket.";
		break;
	case WSAESHUTDOWN:
		msg = "10058: Cannot send after socket shutdown.";
		break;
	case WSAESOCKTNOSUPPORT:
		msg = "10044: Socket type not supported.";
		break;
	case WSAETIMEDOUT:
		msg = "10060: Connection timed out.";
		break;
#if 1	// WinSock2.0
	case WSATYPE_NOT_FOUND:
		msg = "10109: Class type not found.";
		break;
#endif
	case WSAEWOULDBLOCK:
		msg = "10035: Resource temporarily unavailable.";
		break;
	case WSAHOST_NOT_FOUND:
		msg = "11001: Host not found.";
		break;
#if 0	// WinSock2.0
	case WSA_INVALID_HANDLE:
		msg = "%05d: Specified event object handle is invalid.";
		break;
	case WSA_INVALID_PRAMETER:
		msg = "%05d: One or more parameters are invalid.";
		break;
	case WSAINVALIDPROCTABLE:
		msg = "%05d: Invalid procedure table from service provider.";
		break;
	case WSAINVALIDPROVIDER:
		msg = "%05d: Invalid service provider version number.";
		break;
	case WSA_IO_INCOMPLETE:
		msg = "%05d: Overlapped I/O event object not in signaled state.";
		break;
	case WSA_IO_PENDING:
		msg = "%05d: Overlapped operations will complete later.";
		break;
	case WSA_NOT_ENOGH_MEMORY:
		msg = "%05d: Insufficient memory available.";
		break;
#endif
	case WSANOTINITIALISED:
		msg = "10093: Successful WSAStartup not yet performed.";
		break;
	case WSANO_DATA:
		msg = "11004: Valid name, no data record of requested type.";
		break;
	case WSANO_RECOVERY:
		msg = "11003: This is a nonrecoverable error.";
		break;
#if 0	// WinSock2.0
	case WSAPROVIDERFAILEDINIT:
		msg = "%05d: Unable to initialize a service provider.";
		break;
#endif
#if 1	// WinSock2.0
	case WSASYSCALLFAILURE:
		msg = "%05d: System call failure.";
		break;
#endif
	case WSASYSNOTREADY:
		msg = "10091: Network subsystem is unavailable.";
		break;
	case WSATRY_AGAIN:
		msg = "11002: Nonauthoritative host not found.";
		break;
	case WSAVERNOTSUPPORTED:
		msg = "10092: Winsock.dll version out of range.";
		break;
	case WSAEDISCON:
		msg = "10101: Graceful shutdown in progress.";
		break;
#if 1	// WinSock2.0
	case WSA_OPERATION_ABORTED:
		msg = "%05d: Overlapped operation aborted.";
		break;
#endif
	}
	return (FALSE);
}



//////////////////////////////////////////////////////////////////
// Send/Receive で使用する WSABUF と互換性のあるバッファクラス:
//

//
//-- コンストラクタ:
//      pBuf:  NULL以外の場合
//               新たにBufferを確保しないで、pBufをBufferとする
//             dwSize = 0: の場合
//               pBufを文字列と見なして、文字列の長さをlenに設定する
//             dwSize > 0: の場合
//               pBufの内容はBinaryデータとして、dwSizeをlenに設定する
//      pBuf:  NULLの場合
//               新たにdwSize byteのバッファを確保して、lenにdwSizeを設定する
//
CWsaBuf::CWsaBuf(LPSTR pBuf /* = NULL */, DWORD dwSize /* = 0*/)
{
	if (pBuf != NULL) {
		m_tBuf.buf = pBuf;
		if (dwSize == 0) {
			m_tBuf.len = ::lstrlen(pBuf);	// 文字列と見なす
		} else {
			m_tBuf.len = dwSize;
		}
		m_bNew = FALSE;						// new してない
	} else {
		if (dwSize == 0) {
			m_tBuf.buf = NULL;
			m_tBuf.len = 0;
			m_bNew = FALSE;
		} else {
			try {
				m_tBuf.buf = new char[dwSize];	// Bufferを確保
			}
			catch (CMemoryException* ex) {
				ex->ReportError();
				ex->Delete();
			}
			m_tBuf.len = dwSize;
			m_bNew = TRUE;					// new した
		}
	}
}

//
//-- デストラクタ:
//
CWsaBuf::~CWsaBuf()
{
	if (m_bNew) {
		delete[] m_tBuf.buf;	// 新たに確保したときだけdelete
		m_bNew = FALSE;			////
		m_tBuf.len = 0;			////
		m_tBuf.buf = NULL;		////
	}
}


//
//-- 文字列を設定する:
//		input:  pStr   設定する文字列
//		reutrn: 設定した文字列のbyte数
//
int CWsaBuf::SetStrings(LPCSTR pStr)
{
	int  nBytes;

	m_tBuf.buf = (LPSTR)pStr;		// 文字列へのpointer設定
	//
	for (nBytes = 0; *pStr++ != '\0'; ) {
		nBytes++;
	}
	m_tBuf.len = nBytes;	// 文字列の長さを設定
	return (nBytes);
}

//
//-- Bufferの先頭アドレスと長さを更新する:
//		input:  dwProg   進めるbyte数
//		return: Bufferの残りbyte数
//
int CWsaBuf::Update(DWORD dwProg)
{
	int  nRemain;

	if (dwProg > m_tBuf.len) {
		dwProg = m_tBuf.len;
	}
	nRemain = m_tBuf.len - dwProg;
	m_tBuf.len -= dwProg;
	m_tBuf.buf += dwProg;
	return (nRemain);		// 残り
}





