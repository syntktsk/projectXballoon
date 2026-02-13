'//////////////////////////////////////////////////////////////////
'//
'// GL サンプルプログラム
'//
'//      Copyright (C) 2010 Graphtec CORP.
'//
'/////////////////////////////////////////////////////////////////


Module UsbSocket


    '
    ' Wonsoxk風USBソケット　(クライアントのみ)
    '

    Dim Handle As Long              ' USBデバイスハンドル


    '
    ' ソケットの作成
    '   SocketType = SOCK_STREAM | SOCL_DGRAM
    '

    Public Sub Socket(ByVal SocketType As Integer)

        ' 特に処理無し

    End Sub


    '
    ' ソケットのクローズ
    '
    Public Sub SocketClose()
        If Handle <> INVALID_HANDLE_VALUE Then
            GtcUSBr_CloseDevice(Handle) ' 接続のクローズ
            Handle = INVALID_HANDLE_VALUE
        End If
    End Sub



    '
    ' コネクト
    '
    '   Id   = 機器ID
    '
    '   return = 接続に成功：True
    '            接続に失敗：False
    '
    Public Function Connect(ByVal Id As String) As Boolean
        Dim nOpen As Integer        ' デバイスのポインタ
        Dim hUsb(128) As Long       ' ハンドルの一時格納場所
        Dim Cmd As String           ' コマンド文字列
        Dim Qry As String           ' 受信文字列
        Dim nErr As Boolean         ' エラー

        Dim i As Integer            ' 汎用

        Connect = True              ' 戻り値初期化

        Qry = ""

        ' 目的の機器IDを持つデバイスがオープンされるまで
        ' オープンを続ける
        nOpen = 0
        Do
            ' USBをオープン
            Handle = GtcUSBr_OpenDevice
            If Handle = INVALID_HANDLE_VALUE Or Handle = 0 Then
                ' オープンに失敗した
                Connect = False
                Exit Do
            End If

            ' 一時保存
            hUsb(nOpen) = Handle
            nOpen = nOpen + 1

            ' 機器IDをチェックする
            Cmd = ":IF:ID?" & vbCrLf
            nErr = WriteStr(Cmd)
            If nErr = True Then
                ' クエリーを受信
                nErr = ReadStr(Qry)
                If nErr = True Then
					If Id = Mid(Qry, 8, 1) Then
						' IDが一致した
						nOpen = nOpen - 1
						Exit Do
					End If

				Else
                    ' 受信エラー
                    Connect = False
                    Exit Do
                End If
            Else
                ' 送信エラー
                Connect = False
                Exit Do
            End If
        Loop

        ' 指定ID以外のハンドルをクローズする
        For i = 0 To nOpen - 1
            GtcUSBr_CloseDevice(hUsb(i))
        Next i

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
    Public Function WriteBin(ByRef Buf() As Byte, ByVal Bytes As Long) As Boolean

        Dim ToWrite As Long         ' 書き込むバイト数
        Dim Pos As Long             ' バッファポインタ
        Dim rBytes As Long          ' 書き込めたバイト数

        Dim nErr As Long            ' エラーコード
        Dim nCnt As Integer         ' Loopカウンタ


        WriteBin = True             ' 戻り値初期化

        Pos = 0

        ToWrite = Bytes

        Do While ToWrite <> 0
            ' 書き込み
            For nCnt = 0 To 100
                nErr = GtcUSBr_WriteDevice(Handle, Buf(Pos), ToWrite, rBytes, 100)
                If nErr = 0 Then
                    ' エラー
                    If GtcUSBr_GetLastError() <> ERROR_TIMEOUT Then
                        WriteBin = False
                        Exit Do
                    End If

                    If nCnt = 100 Then
                        ' タイムアウト
                        WriteBin = False
                        Exit Do
                    End If

                Else
                    ' 正常終了
                    Exit For
                End If

                Application.DoEvents()
            Next nCnt

            Pos = Pos + rBytes           ' 今回書けたByte数だけ進める
            ToWrite = ToWrite - rBytes   ' まだ書けてないバイト数

            Application.DoEvents()
        Loop

        Bytes = Pos    ' 書けたバイト数
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

        Dim Buf(512) As Byte        ' データバッファ
        Dim nLen As Integer         ' 文字列の長さ

        Dim ToWrite As Long         ' 書き込むバイト数
        Dim sPtr As Long            ' バッファポインタ
        Dim rBytes As Long          ' 書き込めたバイト数

        Dim nErr As Long            ' エラーコード
        Dim nCnt As Integer         ' Loopカウンタ

        Dim i As Integer            ' 汎用

        WriteStr = True             ' 戻り値初期化

        sPtr = 0

        nLen = Len(Str)

        Do While nLen <> 0
            ' StringからByte配列へ変換する
            If nLen > 512 Then
                ToWrite = 512
            Else
                ToWrite = nLen
            End If

            For i = 0 To ToWrite - 1
                Buf(i) = Asc(Mid(Str, sPtr + i + 1, 1))
            Next i

            ' 書き込み
            For nCnt = 0 To 100
                nErr = GtcUSBr_WriteDevice(Handle, Buf(0), ToWrite, rBytes, 100)
                If nErr = 0 Then
                    ' エラー
                    If GtcUSBr_GetLastError() <> ERROR_TIMEOUT Then
                        WriteStr = False
                        Exit Do
                    End If

                    If nCnt = 100 Then
                        ' タイムアウト
                        WriteStr = False
                        Exit Do
                    End If

                Else
                    ' 正常終了
                    Exit For
                End If

                Application.DoEvents()
            Next nCnt

            sPtr = sPtr + rBytes   ' 今回書けたByte数だけ進める
            nLen = nLen - rBytes   ' まだ書けてないバイト数

            Application.DoEvents()

        Loop

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
    Public Function ReadBin(ByRef Buf() As Byte, ByVal Bytes As Long) As Boolean

        Dim ToRead As Long         ' 読み込むバイト数
        Dim Pos As Long            ' バッファポインタ
        Dim rBytes As Long          ' 読めたバイト数

        Dim nErr As Long            ' エラーコード
        Dim nCnt As Integer         ' Loopカウンタ


        ReadBin = True             ' 戻り値初期化

        Pos = 0
        ToRead = Bytes

        Do While ToRead <> 0
            ' 読み込み
            For nCnt = 0 To 100
                nErr = GtcUSBr_ReadDevice(Handle, Buf(Pos), ToRead, rBytes, 100)
                If nErr = 0 Then
                    ' エラー
                    If GtcUSBr_GetLastError() <> ERROR_TIMEOUT Then
                        ReadBin = False
                        Exit Do
                    End If

                    If nCnt = 100 Then
                        ' タイムアウト
                        ReadBin = False
                        Exit Do
                    End If

                Else
                    ' 正常終了
                    Exit For
                End If

                Application.DoEvents()
            Next nCnt

            Pos = Pos + rBytes           ' 今回読めたByte数だけ進める
            ToRead = ToRead - rBytes     ' まだ読めてないバイト数

            Application.DoEvents()

        Loop

        Bytes = Pos    ' 読めたバイト数

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
        Dim rBytes As Long          ' 読めたバイト数

        Dim nErr As Long    ' エラーコード
        Dim nCnt As Integer         ' Loopカウンタ


        ReadStr = True      ' 戻り値初期化
        Str = ""            ' 受信領域初期化

        Do
            ' 1文字受信
            For nCnt = 0 To 100
                nErr = GtcUSBr_ReadDevice(Handle, c, 1, rBytes, 100)
                If nErr = 0 Then
                    ' エラー
                    If GtcUSBr_GetLastError() <> ERROR_TIMEOUT Then
                        ReadStr = False
                        Exit Do
                    End If

                    If nCnt = 100 Then
                        ' タイムアウト
                        ReadStr = False
                        Exit Do
                    End If

                Else
                    ' 正常終了
                    Exit For
                End If

                Application.DoEvents()
            Next nCnt

            ' 終了条件の確認
            If c = 0 Then
                ' NULL文字
                Exit Do
            End If

            If c = Asc(vbLf) Then
                ' LF
                Exit Do
            End If

            If c <> Asc(vbCr) Then   ' CRは無視する
                Str = Str & Chr(c)  ' 最後に追加する
            End If

        Loop
    End Function


End Module
