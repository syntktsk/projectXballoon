#ifndef devioH
#define devioH

#ifdef MODULETEST
	#define DLLFUNC
#else	// MODULETEST
#ifdef DLL
	#define DLLFUNC __declspec( dllexport )
#else	// DLL
	#define DLLFUNC __declspec( dllimport )
#endif	// DLL
#endif	// MODULETEST

#include <windows.h>

// オーバーラップ構造体
typedef struct {					// 非同期入出力用
	DWORD dwErrorCode;				// エラーコード
	DWORD dwNumberOfBytesTransfered;// 入出力バイト数
	HANDLE hEvent;					// イベントハンドル
} GtcUSBr_OVERLAPPED, *LPGtcUSBr_OVERLAPPED;

// スレッドローカル変数
typedef struct {
	DWORD	dwErrorCode;		// 最後に起きたエラーコード
} GtcUSBr_TRDLOCALVALUE, *LPGtcUSBr_TRDLOCALVALUE;


#ifdef __cplusplus
extern "C"{
#endif

// エラーコードの取得
DLLFUNC
DWORD __stdcall GtcUSBr_GetLastError(void);

// デバイスオープン
DLLFUNC
HANDLE __stdcall GtcUSBr_OpenDevice(void);

// デバイスクローズ
DLLFUNC
BOOL __stdcall GtcUSBr_CloseDevice(
	HANDLE hDev						// デバイスのハンドル
);

// 同期読み込み
DLLFUNC
BOOL __stdcall GtcUSBr_ReadDevice(
	HANDLE hDev,					// デバイスのハンドル
	LPVOID lpBuffer,				// データバッファ
	DWORD nNumberOfBytesToRead,		// 読み取り対象のバイト数
	LPDWORD lpNumberOfBytesRead,	// 読み取ったバイト数
	DWORD dwTimeOut					// TimeOut時間指定(ミリ秒)
);

// 非同期読み込み
DLLFUNC
BOOL __stdcall GtcUSBr_ReadDeviceEx(
	HANDLE hDev,					// デバイスのハンドル
	LPVOID lpBuffer,				// データバッファ
	DWORD nNumberOfBytesToRead,		// 読み取り対象のバイト数
	LPGtcUSBr_OVERLAPPED lpGtcUSBr_Overlapped	// オーバーラップ構造体のバッファ
);

// 同期書き込み
DLLFUNC
BOOL __stdcall GtcUSBr_WriteDevice(
	HANDLE hDev,					// デバイスのハンドル
	LPVOID lpBuffer,				// データバッファ
	DWORD nNumberOfBytesToWrite,	// 書き込み対象のバイト数
	LPDWORD lpNumberOfBytesWrite,	// 書き込んだバイト数
	DWORD dwTimeOut					// TimeOut時間指定(ミリ秒)
);

// 非同期書き込み
DLLFUNC
BOOL __stdcall GtcUSBr_WriteDeviceEx(
	HANDLE hDev,					// デバイスのハンドル
	LPVOID lpBuffer,				// データバッファ
	DWORD nNumberOfBytesToWrite,	// 書き込み対象のバイト数
	LPGtcUSBr_OVERLAPPED lpGtcUSBr_Overlapped	// オーバーラップ構造体のバッファ
);


#ifdef __cplusplus
}
#endif

#endif
