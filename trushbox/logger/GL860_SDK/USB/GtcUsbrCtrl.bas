Attribute VB_Name = "GtcUsbrCtrl"
'//////////////////////////////////////////////////////////////////
'//
'// GL サンプルプログラム
'//
'//      Copyright (C) 2010 Graphtec CORP.
'//
'/////////////////////////////////////////////////////////////////

' エラーコード
Public Const INVALID_HANDLE_VALUE As Long = -1

' Windowsエラーコード（WINERROR.Hより抜粋）
' GtcUsbr.dll 内部で明示的に発生するエラーコード

Public Const ERROR_SUCCESS              As Long = 0
Public Const ERROR_INVALID_HANDLE       As Long = 6
Public Const ERROR_NOT_ENOUGH_MEMORY    As Long = 8
Public Const ERROR_HANDLE_EOF           As Long = 38
Public Const ERROR_BUSY                 As Long = 170
Public Const ERROR_DEVICE_NOT_CONNECTED As Long = 1167
Public Const ERROR_NO_MORE_DEVICES      As Long = 1248
Public Const ERROR_TIMEOUT              As Long = 1460


'// Overlapped structue
'typedef struct {
'    DWORD dwErrorCode;              // Error Code
'    DWORD dwNumberOfBytesTransfered;// Number of Bytes I/O
'    HANDLE hEvent;                  // Handle of Event
'} GtcUSBr_OVERLAPPED, *LPGtcUSBr_OVERLAPPED;
Public Type GtcUSBr_OVERLAPPED
    dwErrorCode As Long                 'Error Code
    dwNumberOfBytesTransfered As Long   'Number of Bytes I/O
    hEvent As Long                      'Handle of Event
End Type


'// Thread Loacal variable
'typedef struct {
'    DWORD   dwErrorCode;        // Error code
'} GtcUSBr_TRDLOCALVALUE, *LPGtcUSBr_TRDLOCALVALUE;
Public Type GtcUSBr_TRDLOCALVALUE
    dwErrorCode As Long                 'Error code
End Type


'
'// Get last Error code
'DLLFUNC
'DWORD __stdcall GtcUSBr_GetLastError(void);
Declare Function GtcUSBr_GetLastError Lib "gtcusbr.dll" Alias "GtcUSBr_GetLastError" () As Long


'// Open device
'DLLFUNC
'HANDLE __stdcall GtcUSBr_OpenDevice(void);
Declare Function GtcUSBr_OpenDevice Lib "gtcusbr.dll" Alias "GtcUSBr_OpenDevice" () As Long


'// Close device
'DLLFUNC
'BOOL __stdcall GtcUSBr_CloseDevice(
'    HANDLE hDev                     // Handle to the USB to be I/O
');
Declare Function GtcUSBr_CloseDevice Lib "gtcusbr.dll" Alias "GtcUSBr_CloseDevice" ( _
    ByVal hDev As Long _
) As Long


'// Read (Block mode)
'DLLFUNC
'BOOL __stdcall GtcUSBr_ReadDevice(
'    HANDLE hDev,                    // Handle to the USB to be read
'    LPVOID lpBuffer,                // Pointer to the buffer that receives the data read from the USB
'    DWORD nNumberOfBytesToRead,     // Number of bytes to read
'    LPDWORD lpNumberOfBytesRead,    // Pointer to the number of bytes read
'    DWORD dwTimeOut                 // TimeOut(ms)
');
Declare Function GtcUSBr_ReadDevice Lib "gtcusbr.dll" Alias "GtcUSBr_ReadDevice" ( _
    ByVal hDev As Long, _
    ByRef lpBuffer As Byte, _
    ByVal nNumberOfBytesToRead As Long, _
    ByRef lpNumberOfBytesRead As Long, _
    ByVal dwTimeOut As Long _
) As Long


'// Read (Overlapped)
'DLLFUNC
'BOOL __stdcall GtcUSBr_ReadDeviceEx(
'    HANDLE hDev,                    // Handle to the USB to be read
'    LPVOID lpBuffer,                // Pointer to the buffer that receives the data read from the USB
'    DWORD nNumberOfBytesToRead,     // Number of bytes to read
'    LPGtcUSBr_OVERLAPPED lpGtcUSBr_Overlapped   // Pointer of overlapped structure
');
Declare Function GtcUSBr_ReadDeviceEx Lib "gtcusbr.dll" Alias "GtcUSBr_ReadDeviceEx" ( _
    ByVal hDev As Long, _
    ByRef lpBuffer As Byte, _
    ByVal nNumberOfBytesToRead As Long, _
    ByRef lpGtcUSBr_Overlapped As GtcUSBr_OVERLAPPED _
) As Long


'
'// Write (Block mode)
'DLLFUNC
'BOOL __stdcall GtcUSBr_WriteDevice(
'    HANDLE hDev,                    // Handle to the USB to be write
'    LPVOID lpBuffer,                // Pointer to the buffer that sends the data write to the USB
'    DWORD nNumberOfBytesToWrite,    // Number of bytes to write
'    LPDWORD lpNumberOfBytesWrite,   // Pointer to the number of bytes write
'    DWORD dwTimeOut                 // TimeOut(ms)
');
Declare Function GtcUSBr_WriteDevice Lib "gtcusbr.dll" Alias "GtcUSBr_WriteDevice" ( _
    ByVal hDev As Long, _
    ByRef lpBuffer As Byte, _
    ByVal nNumberOfBytesToWrite As Long, _
    ByRef lpNumberOfBytesWrite As Long, _
    ByVal dwTimeOut As Long _
) As Long


'
'// Write (Overlapped)
'DLLFUNC
'BOOL __stdcall GtcUSBr_WriteDeviceEx(
'    HANDLE hDev,                    // Handle to the USB to be write
'    LPVOID lpBuffer,                // Pointer to the buffer that sends the data write to the USB
'    DWORD nNumberOfBytesToWrite,    // Number of bytes to write
'    LPGtcUSBr_OVERLAPPED lpGtcUSBr_Overlapped   // Pointer of overlapped structure
');
Declare Function GtcUSBr_WriteDeviceEx Lib "gtcusbr.dll" Alias "GtcUSBr_WriteDeviceEx" ( _
    ByVal hDev As Long, _
    ByRef lpBuffer As Byte, _
    ByVal nNumberOfBytesToWrite As Long, _
    ByRef lpGtcUSBr_Overlapped As GtcUSBr_OVERLAPPED _
) As Long






