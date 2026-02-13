// GLSampleDoc.h : CGLSampleDoc クラスのインターフェイス
//


#pragma once


enum {SEL_IF_TCP = 0, SEL_IF_USB};				// 接続I/F
enum {	MC_NOP = 0,								// 機種
		MC_GL860,
};

#define	MAX_ALARM_SET	4						// アラームのSET数 最大値

class CGLSampleDoc : public CDocument
{
protected: // シリアル化からのみ作成します。
	CGLSampleDoc();
	DECLARE_DYNCREATE(CGLSampleDoc)

// 属性
public:
	DWORD			m_dwIpAddr;	// IP アドレス
	DWORD			m_dwPort;	// Port番号

	DWORD			m_dwMachineId;	// 機器ID（USBで使用）

	char			m_nIf;		// I/Fセレクト　0:TCP/IP 1:USB

	char			m_nMachine;	// 機種判別　1:GL860 その他:現在は未使用
	char			m_nCh;		// CH数


// 操作
public:

// オーバーライド
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// 実装
public:
	virtual ~CGLSampleDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成された、メッセージ割り当て関数
protected:
	DECLARE_MESSAGE_MAP()
};


