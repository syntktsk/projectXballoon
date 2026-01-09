'//////////////////////////////////////////////////////////////////
'//
'// GL サンプルプログラム
'//
'//      Copyright (C) 2010 Graphtec CORP.
'//
'/////////////////////////////////////////////////////////////////


Module DevIo


    Dim SelIf As Integer        ' I/F セレクト
    Dim IpAddress As String     ' IPアドレス
    Dim PortNo As String        ' PORT番号
    Dim MachineId As String     ' 機器ID


    '
    ' デバイスの生成
    '
    '   Sel = I/Fセレクト　SEL_TCP | SEL_USB
    '
    Public Sub DevCreate(ByVal Sel As Integer)
        SelIf = Sel
    End Sub


    '
    ' デバイスの破棄
    '
    Public Sub DevDestroy()
        ' 特に処理なし
    End Sub


    '
    ' デバイスのオープン
    '
    '   Address = Lan時：IPアドレス　USB時：機器ID
    '   Port    = Lan時：ポート番号　USB時：未使用
    '
    '   return = デバイスのオープンに成功：True
    '            デバイスのオープンに失敗：False
    '
    Public Function DevOpen(ByVal Address As String, ByVal Port As String) As Boolean

        Dim Err As Boolean

        DevOpen = True      '戻り値初期化

        If SelIf = SEL_TCP Then
            ' LAN時

            IpAddress = Address     ' IPアドレス
            PortNo = Port           ' PORT番号

            ' ソケット生成
            LanSocket.Socket(ASOCK_STREAM)

            ' コネクト
            If LanSocket.Connect(Address, Port) = False Then
                DevOpen = False
                Exit Function
            End If

        Else
            ' USB時
            MachineId = Address     ' 機器ID

            ' ソケット生成
            UsbSocket.Socket(ASOCK_STREAM)

            ' コネクト
            Err = UsbSocket.Connect(Address)
            If Err = False Then
                DevOpen = False
                Exit Function
            End If


        End If

    End Function


    '
    ' クローズ
    '
    Public Sub DevClose()

        If SelIf = SEL_TCP Then
            LanSocket.SocketClose()
        Else
            UsbSocket.SocketClose()
        End If

    End Sub


    '
    ' 読み込み（バイナリ）
    '
    '   Buf = 受信データの格納場所
    '   Bytes = In:受信バイト数　Out:受信できたバイト数
    '
    '   retuen = 受信成功：True
    '            受信失敗：False
    '
    Public Function ReadBin(ByRef Buf() As Byte, ByVal Bytes As Long) As Boolean
        If SelIf = SEL_TCP Then
            ReadBin = LanSocket.ReadBin(Buf, Bytes)
        Else
            ReadBin = UsbSocket.ReadBin(Buf, Bytes)
        End If
    End Function

    '
    ' 読み込み  (文字列)
    '
    '   Str = 受信文字列データの格納場所
    '
    '   retuen = 受信成功：True
    '            受信失敗：False
    '
    '   NULL文字か、LFを受信するまでStrに文字を格納する。
    '   Strに、NULL文字、CR,LFは含まない
    '
    Public Function ReadStr(ByRef Str As String) As Boolean
        If SelIf = SEL_TCP Then
            ReadStr = LanSocket.ReadStr(Str)
        Else
            ReadStr = UsbSocket.ReadStr(Str)
        End If
    End Function


    '
    ' 書き込み（バイナリー）
    '
    '   Buf = 送信データのバッファ
    '   Bytes = In:送信するバイト数  Out:実際に送信されたバイト数
    '
    '   return = 送信に成功：True
    '            送信に失敗：False
    '
    Public Function WriteBin(ByRef Buf() As Byte, ByVal Bytes As Long) As Boolean
        If SelIf = SEL_TCP Then
            WriteBin = LanSocket.WriteBin(Buf, Bytes)
        Else
            WriteBin = UsbSocket.WriteBin(Buf, Bytes)
        End If
    End Function

    '
    ' 書き込み（文字列）
    '
    '   Str = 送信文字列データ
    '
    '   return = 送信に成功：True
    '            送信に失敗：False
    '
    Public Function WriteStr(ByRef Str As String) As Boolean
        If SelIf = SEL_TCP Then
            WriteStr = LanSocket.WriteStr(Str)
        Else
            WriteStr = UsbSocket.WriteStr(Str)
        End If
    End Function


    '
    ' 問い合わせコマンドの送信と応答文字列受信
    '
    '   Command = 送信コマンド
    '   Query   = 受信文字列の格納場所
    '
    Public Function SendQuery(ByRef Command As String, ByRef Query As String) As Boolean
        Dim Cmd As String

        SendQuery = True    ' 戻り値初期化

        ' CR+LFを付加
        Cmd = Command & vbCrLf

        ' 問い合わせコマンド送信
        If WriteStr(Cmd) = False Then
            SendQuery = False
            Exit Function
        End If

        ' 応答文字列受信
        If ReadStr(Query) = False Then
            SendQuery = False
            Exit Function
        End If

    End Function

    '
    ' コマンド送信
    '
    '   Command = コマンド文字列
    '
    Public Function SendCommand(ByRef Command As String) As Boolean
        Dim Cmd As String

        SendCommand = True    ' 戻り値初期化

        ' CR+LFを付加
        Cmd = Command & vbCrLf

        ' 問い合わせコマンド送信
        If WriteStr(Cmd) = False Then
            SendCommand = False
            Exit Function
        End If

    End Function


End Module
