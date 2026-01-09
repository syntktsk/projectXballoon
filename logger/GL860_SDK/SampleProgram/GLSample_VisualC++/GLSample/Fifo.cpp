//////////////////////////////////////////////////////////////////
//
// GL サンプルプログラム
//
//		Copyright (C) 2010 Graphtec CORP.
//
// Fifo.cpp: CFifo クラス
//
//	通信用バッファで使用するFIFOバッファを定義
//
//
/////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
//
// Fifo.cpp: CFifo クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Fifo.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

CFifo::CFifo(int nSize /* = 1024 */)
{
	//
	//-- nSize を 2~N にアラインする:
	//
	const int  nTbl[] = {
		1,		2,		4,		8,
		16,		32,		64,		128,
		256,	512,	1024,	2048,
		4096,	8192,	16384,	32768,
		65536,
	};

	if (nSize > 65536) {
		nSize = 65536;
	} else if (nSize < 64) {
		nSize = 64;
	} else {
		for (int i = 6; i < 16; i++) {
			if (nSize == nTbl[i]) {
				break;
			}
			if (nSize > nTbl[i] && nSize < nTbl[i + 1]) {
				nSize = nTbl[i + 1];
				break;
			}
		}
	}
	m_pBuf = new char[nSize + 8];	// Buffer領域を確保
	m_nMsk = nSize - 1;				// Mask
	m_nPut = m_nGet = 0;			// Pointer初期化
}

CFifo::~CFifo()
{
	if (m_pBuf) {
		delete[] m_pBuf;
	}
}

//////////////////////////////////////////
// オペレーション:
//

//
//-- データをFIFOに書く:
//
BOOL CFifo::Put(int nDat)
{
	if (Size() < FifoSize()) {
		m_pBuf[m_nPut] = nDat & 0xff;	// FIFOに書く
		m_nPut = (++m_nPut & m_nMsk);	// Put pointer更新
		return (TRUE);
	}
	return (FALSE);
}

//
//-- FIFOからデータを読む:
//
BOOL CFifo::Get(int &nDat)
{
	if (Size()) {
		nDat = m_pBuf[m_nGet] & 0xff;	// FIFOから読む
		m_nGet = (++m_nGet & m_nMsk);	// Get pointerを更新
		return (TRUE);
	}
	return (FALSE);
}

