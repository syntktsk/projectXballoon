//////////////////////////////////////////////////////////////////
//
// GL サンプルプログラム
//
//		Copyright (C) 2010 Graphtec CORP.
//
/////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
//
// Fifo.h: CFifo クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FIFO_H__5A891F15_4640_4634_8642_96117AAF5AF3__INCLUDED_)
#define AFX_FIFO_H__5A891F15_4640_4634_8642_96117AAF5AF3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


////////////////////////////////////////////
//  高速な? FIFO クラス:
//
class CFifo
{
	//
	// コンストラクタ/デストラクタ
	//
public:
	CFifo(int nSize = 1024);
	~CFifo();
	//
	// アトリビュート
	//
private:
	int   m_nPut;		// Put pointer
	int   m_nGet;		// Get pointer
	char *m_pBuf;		// FIFO buffer
	int   m_nMsk;		// Bit Mask
	//
	// オペレーション
	//
public:
	void  Init()		{ m_nPut = m_nGet = 0; }				// 初期化する
	int   Size()		{ return ((m_nPut - m_nGet) & m_nMsk); }	// FIFOにあるデータ数
	int   FifoSize()	{ return (m_nMsk + 1); }					// 実際に設定されたFIFOサイズ
	//
	BOOL  Put(int nDat);	// データをFIFOに入れる
	BOOL  Get(int &nDat);	// データをFIFOから取得
};

#endif // !defined(AFX_FIFO_H__5A891F15_4640_4634_8642_96117AAF5AF3__INCLUDED_)
