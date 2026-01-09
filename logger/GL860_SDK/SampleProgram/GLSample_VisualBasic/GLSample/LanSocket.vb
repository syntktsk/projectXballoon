'//////////////////////////////////////////////////////////////////
'//
'// GL サンプルプログラム
'//
'//      Copyright (C) 2010 Graphtec CORP.
'//
'/////////////////////////////////////////////////////////////////

Imports System.Net.Sockets


Module LanSocket



    '
    ' Wonsoxk風IPソケット　(クライアントのみ)
    '

    Dim SockErr As Integer          ' エラーコード

    Dim Sock As Socket              ' WinSock



    '
    ' ソケットの作成
    '   SocketType = SOCK_STREAM | SOCL_DGRAM
    '

    Public Sub Socket(ByVal SocketType As Integer)

        Dim ProtocolTp As ProtocolType
        Dim SocketTp As SocketType

        If SocketType = ASOCK_STREAM Then
            SocketTp = Net.Sockets.SocketType.Stream
            ProtocolTp = ProtocolType.Tcp
        Else
            SocketTp = Net.Sockets.SocketType.Dgram
            ProtocolTp = ProtocolType.Udp
        End If

        Sock = New Socket(AddressFamily.InterNetwork, SocketTp, ProtocolTp)

    End Sub



    '
    ' ソケットのクローズ
    '
    Public Sub SocketClose()
        Sock.Close()          ' 接続のクローズ
    End Sub


    '
    ' コネクト
    '
    '   IP   = IPアドレス
    '   Port = ポート番号
    '
    '   return = 接続に成功：True
    '            接続に失敗：False
    '
    Public Function Connect(ByVal Ip As String, ByVal Port As String) As Boolean

        Connect = True              ' 戻り値初期化

        SockErr = 0                 ' エラー初期化


        Try
            Sock.Connect(Ip, Port)      ' 接続
        Catch e As SocketException
            Sock.Close()
            Connect = False
            SockErr = e.ErrorCode
        End Try

    End Function



    '
    ' 送信（バイナリー）
    '
    '   Buf = 送信データのバッファ
    '   Bytes = In:送信するバイト数  Out:実際に送信されたバイト数
    '
    '   return = 送信に成功：True
    '            送信に失敗：False
    '
    Public Function WriteBin(ByRef Buf() As Byte, ByVal Bytes As Integer) As Boolean

        WriteBin = True             ' 戻り値初期化
        SockErr = 0                 ' エラー初期化

        ' タイムアウトの設定
        Sock.SendTimeout = 10000    '10秒

        Try
            Sock.Send(Buf, Bytes, SocketFlags.None)     ' 送信
        Catch e As SocketException
            WriteBin = False
            SockErr = e.ErrorCode
        End Try

    End Function



    '
    ' 送信（文字列）
    '
    '   Str = 送信文字列データ
    '
    '   return = 送信に成功：True
    '            送信に失敗：False
    '
    Public Function WriteStr(ByRef Str As String) As Boolean
        Dim SData() As Byte
        Dim i As Long

        WriteStr = True             ' 戻り値初期化
        SockErr = 0                 ' エラー初期化

        ' String型を直接送信出来ないので、
        ' 送信データをByte型の配列に置き換える
        ReDim SData(Str.Length - 1)
        For i = 0 To Str.Length - 1
            SData(i) = Asc(Str.Chars(i))
        Next i

        WriteStr = WriteBin(SData, Str.Length)

    End Function


    '
    ' 受信  (バイナリー)
    '
    '   Buf = 受信データの格納場所
    '   Bytes = In:受信バイト数　Out:受信できたバイト数
    '
    '   retuen = 受信成功：True
    '            受信失敗：False
    '
    Public Function ReadBin(ByRef Buf() As Byte, ByVal Bytes As Integer) As Boolean
        ' GetDataメソッドの場合、配列の途中から受信領域を指定出来ない為、
        ' 1バイトずつ受信して配列に順次格納する方式取る

        Dim Pos As Integer      ' 受信バッファのポインタ
        Dim rByte As Integer    ' 受信するバイト数

        ReadBin = True          ' 戻り値初期化

        ' タイムアウトの設定
        Sock.ReceiveTimeout = 10000    '10秒

        Pos = 0

        Try
            Do While Bytes <> 0

                rByte = Sock.Receive(Buf, Pos, Bytes, SocketFlags.None)

                Pos = Pos + rByte
                Bytes = Bytes - rByte
            Loop
        Catch e As SocketException

            ReadBin = False

        End Try

    End Function

    '
    ' 受信  (文字列)
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
        Dim c As Byte       ' 文字の一時格納領域
        Dim rcv(2) As Byte

        ReadStr = True      ' 戻り値初期化

        Str = ""            ' 受信領域初期化

        Do
            ' 1文字受信
            If ReadBin(rcv, 1) = False Then
                ReadStr = False
                Exit Do
            End If
            c = rcv(0)

            ' 終了条件の確認
            If c = 0 Then
                ' NULL文字
                Exit Do
            End If

            If c = Asc(vbLf) Then
                ' LF
                Exit Do
            End If

            If c <> Asc(vbCr) Then      ' CRは無視する
                Str = Str & Chr(c)      ' 最後に追加する
            End If

        Loop

    End Function




End Module
