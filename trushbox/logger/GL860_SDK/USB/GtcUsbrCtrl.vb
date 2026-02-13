'//////////////////////////////////////////////////////////////////
'//
'// GL サンプルプログラム
'//
'//      Copyright (C) 2010 Graphtec CORP.
'//
'/////////////////////////////////////////////////////////////////


Module GtcUsbrCtrl
    ' エラーコード
    Public Const INVALID_HANDLE_VALUE As Int32 = -1

    ' Windowsエラーコード（WINERROR.Hより抜粋）
    ' GtcUsbr.dll 内部で明示的に発生するエラーコード

    Public Const ERROR_SUCCESS As Int32 = 0
    Public Const ERROR_INVALID_HANDLE As Int32 = 6
    Public Const ERROR_NOT_ENOUGH_MEMORY As Int32 = 8
    Public Const ERROR_HANDLE_EOF As Int32 = 38
    Public Const ERROR_BUSY As Int32 = 170
    Public Const ERROR_DEVICE_NOT_CONNECTED As Int32 = 1167
    Public Const ERROR_NO_MORE_DEVICES As Int32 = 1248
    Public Const ERROR_TIMEOUT As Int32 = 1460


    '// Overlapped structue
    'typedef struct {
    '    DWORD dwErrorCode;              // Error Code
    '    DWORD dwNumberOfBytesTransfered;// Number of Bytes I/O
    '    HANDLE hEvent;                  // Handle of Event
    '} GtcUSBr_OVERLAPPED, *LPGtcUSBr_OVERLAPPED;
    Public Structure GtcUSBr_OVERLAPPED
        Dim dwErrorCode As Int32                    'Error Code
        Dim dwNumberOfBytesTransfered As Int32      'Number of Bytes I/O
        Dim hEvent As Int32                         'Handle of Event
    End Structure


    '// Thread Loacal variable
    'typedef struct {
    '    DWORD   dwErrorCode;        // Error code
    '} GtcUSBr_TRDLOCALVALUE, *LPGtcUSBr_TRDLOCALVALUE;
    Public Structure GtcUSBr_TRDLOCALVALUE
        Dim dwErrorCode As Int32                 'Error code
    End Structure


    '
    '// Get last Error code
    'DLLFUNC
    'DWORD __stdcall GtcUSBr_GetLastError(void);
    Declare Function GtcUSBr_GetLastError Lib "gtcusbr.dll" Alias "GtcUSBr_GetLastError" () As Int32


    '// Open device
    'DLLFUNC
    'HANDLE __stdcall GtcUSBr_OpenDevice(void);
    Declare Function GtcUSBr_OpenDevice Lib "gtcusbr.dll" Alias "GtcUSBr_OpenDevice" () As Int32


    '// Close device
    'DLLFUNC
    'BOOL __stdcall GtcUSBr_CloseDevice(
    '    HANDLE hDev                     // Handle to the USB to be I/O
    ');
    Declare Function GtcUSBr_CloseDevice Lib "gtcusbr.dll" Alias "GtcUSBr_CloseDevice" (
        ByVal hDev As Int32
    ) As Int32


    '// Read (Block mode)
    'DLLFUNC
    'BOOL __stdcall GtcUSBr_ReadDevice(
    '    HANDLE hDev,                    // Handle to the USB to be read
    '    LPVOID lpBuffer,                // Pointer to the buffer that receives the data read from the USB
    '    DWORD nNumberOfBytesToRead,     // Number of bytes to read
    '    LPDWORD lpNumberOfBytesRead,    // Pointer to the number of bytes read
    '    DWORD dwTimeOut                 // TimeOut(ms)
    ');
    Declare Function GtcUSBr_ReadDevice Lib "gtcusbr.dll" Alias "GtcUSBr_ReadDevice" (
        ByVal hDev As Int32,
        ByRef lpBuffer As Byte,
        ByVal nNumberOfBytesToRead As Int32,
        ByRef lpNumberOfBytesRead As Int32,
        ByVal dwTimeOut As Int32
    ) As Int32


    '// Read (Overlapped)
    'DLLFUNC
    'BOOL __stdcall GtcUSBr_ReadDeviceEx(
    '    HANDLE hDev,                    // Handle to the USB to be read
    '    LPVOID lpBuffer,                // Pointer to the buffer that receives the data read from the USB
    '    DWORD nNumberOfBytesToRead,     // Number of bytes to read
    '    LPGtcUSBr_OVERLAPPED lpGtcUSBr_Overlapped   // Pointer of overlapped structure
    ');
    Declare Function GtcUSBr_ReadDeviceEx Lib "gtcusbr.dll" Alias "GtcUSBr_ReadDeviceEx" (
        ByVal hDev As Int32,
        ByRef lpBuffer As Byte,
        ByVal nNumberOfBytesToRead As Int32,
        ByRef lpGtcUSBr_Overlapped As GtcUSBr_OVERLAPPED
    ) As Int32


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
    Declare Function GtcUSBr_WriteDevice Lib "gtcusbr.dll" Alias "GtcUSBr_WriteDevice" (
        ByVal hDev As Int32,
        ByRef lpBuffer As Byte,
        ByVal nNumberOfBytesToWrite As Int32,
        ByRef lpNumberOfBytesWrite As Int32,
        ByVal dwTimeOut As Int32
    ) As Int32


    '
    '// Write (Overlapped)
    'DLLFUNC
    'BOOL __stdcall GtcUSBr_WriteDeviceEx(
    '    HANDLE hDev,                    // Handle to the USB to be write
    '    LPVOID lpBuffer,                // Pointer to the buffer that sends the data write to the USB
    '    DWORD nNumberOfBytesToWrite,    // Number of bytes to write
    '    LPGtcUSBr_OVERLAPPED lpGtcUSBr_Overlapped   // Pointer of overlapped structure
    ');
    Declare Function GtcUSBr_WriteDeviceEx Lib "gtcusbr.dll" Alias "GtcUSBr_WriteDeviceEx" (
        ByVal hDev As Int32,
        ByRef lpBuffer As Byte,
        ByVal nNumberOfBytesToWrite As Int32,
        ByRef lpGtcUSBr_Overlapped As GtcUSBr_OVERLAPPED
    ) As Int32


End Module
