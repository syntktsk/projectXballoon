'//////////////////////////////////////////////////////////////////
'//
'// GL サンプルプログラム
'//
'//      Copyright (C) 2010 Graphtec CORP.
'//
'/////////////////////////////////////////////////////////////////

Imports Microsoft.VisualBasic
Imports System.IO

Module StopProc


    Const S_OUT As Long = &H4   ' 開始点範囲外
    Const E_OUT As Long = &H2   ' 終了点範囲外
    Const T_ERR As Long = &H1   ' エラー

    Const TRANS_POINTS As Long = 500    ' 一回の転送で受け取るデータ点数
    Dim DataBuf(&H20000) As Byte        ' データバッファ
    ' TRANS_POINTS分の受け取りに十分な大きさがある事

    '
    ' ストップボタン押下時の処理
    '
    '   return = ストップ成功：True
    '            ストップ失敗：False
    '
    Public Function StopFunction() As Boolean

        Dim Str As String   ' クエリー受信等、文字列ワーク
        Dim Ptr As Long     ' ポインターワーク

        Dim i As Long       ' 汎用
        Dim b As Boolean    ' 汎用
        Dim l As Long       ' 汎用

        Dim Err As Integer  ' エラー

        StopFunction = False   ' 戻り値初期化

        Str = ""

        '
        ' 通信の構築
        '
        DevIo.DevCreate(SelIf)

        '
        ' デバイスのオープン
        '
        ' オープンに関しては、TCPとUSBで引数が異なるため、
        ' 個別にオープンする。
        '

        GLSample.AddLog("デバイスをオープンしています。" & vbCrLf)

        Select Case SelIf
            Case SEL_TCP
                ' TCPでオープン
                If DevIo.DevOpen(IpAddress, PortNo) = False Then
                    ' デバイスのオープンに失敗した
                    GoTo OPEN_ERR_EXIT
                End If

            Case SEL_USB
                ' USBでオープン
                If DevIo.DevOpen(MachineId, "") = False Then
                    ' デバイスのオープンに失敗した
                    GoTo OPEN_ERR_EXIT
                End If

            Case Else
                ' その他（通常はあり得ない）
                GoTo OPEN_ERR_EXIT
        End Select

        '
        ' エラークリア
        '
        If DevIo.SendCommand("*CLS") = False Then
            ' エラー
            GoTo COMM_ERR_EXIT
        End If

        '
        ' 記録ストップ
        '
        If DevIo.SendCommand(":MEAS:STOP") = False Then
            ' エラー
            GoTo COMM_ERR_EXIT
        End If

        '
        ' 本当にストップしたか確認する
        '
        b = False
        For i = 0 To 20

            If DevIo.SendQuery(":STAT:COND?", Str) = False Then
                ' エラー
                GoTo COMM_ERR_EXIT
            End If

            '
            ' 文字列は ":STAT:COND 1234"
            ' の形式で戻って来るので、最初のスペース文字を検出
            '
            Ptr = InStr(Str, " ")
            If Ptr <= 0 Then
                ' 区切り文字が見つからない
                GoTo COMM_ERR_EXIT
            End If

            Ptr = Ptr + 1           ' ステータス文字列の先頭
            Str = Mid(Str, Ptr)     ' ステータス文字列を抽出
            l = Str                 ' ステータス

            l = l And 1             ' REC bit
            If l = 0 Then
                ' 本体が停止状態になった
                b = True
                Exit For
            End If

            GLSample.WaitTime(500) ' 500ms Wait
        Next i

        If b = False Then
            ' 本体はストップしてない？
            GLSample.AddLog("本体がストップできませんでした。" & vbCrLf)

            GoTo ERR_EXIT
        End If


        GLSample.AddLog("ストップしました。" & vbCrLf)

        '
        ' 収録されたデータを取得する
        '

        If MsgBox("収録されたデータを転送しますか？", _
                  vbYesNo + vbQuestion, _
                  "確認" _
                  ) = vbYes Then


            '
            ' 収録されたデータを保存する
            '
            Err = GetRecData
            Select Case Err
                Case 1 ' ディスクエラー
                    GoTo ERR_EXIT

                Case 2  ' 通信エラー
                    GoTo COMM_ERR_EXIT

                Case 3  ' 本体エラー
                    GoTo ERR_EXIT

            End Select
        End If


        '
        ' デバイスをクローズ
        '
        GLSample.AddLog("デバイスを閉じています。" & vbCrLf)

        DevIo.DevClose()

        '
        ' 通信を破棄
        '
        DevIo.DevDestroy()

        StopFunction = True

        Exit Function

        '/////////////////////////////////////////////////////////////////
        '
        '
        '
        ' エラー処理
        '
        '
OPEN_ERR_EXIT:  ' デバイスのオープンに失敗した
        GLSample.AddLog("デバイスのオープンに失敗しました。" & vbCrLf)
        GoTo ERR_EXIT

COMM_ERR_EXIT:  ' 通信エラーで終了の場合
        GLSample.AddLog("デバイスとの通信に失敗しました。" & vbCrLf)
        GoTo ERR_EXIT

ERR_EXIT:  ' エラーで終了の場合
        StopFunction = False

        DevIo.DevClose()
        DevIo.DevDestroy()

    End Function


    '
    ' 本体に収録されているデータを転送
    '
    '   return = 正常終了　　　　　　：0
    '            ファイル入出力エラー：1
    '            通信エラー　　　　　：2
    '            本体動作エラー　　　：3
    '

    Private Function GetRecData() As Integer

        Dim FileName As String  ' 保存先のファイルネーム（PC フルパス）
        Dim FileNo As Integer      ' File NO

        Dim Str As String       ' 文字バッファ
        Dim Buf(32) As Byte     ' クエリ受信エリア

        Dim Stat(16) As Byte    ' ステータス

        Dim Size As Long        ' 読み込みサイズ

        Dim HeaderSize As Long  ' ヘッダーサイズ
        Dim DataSize As Long    ' データサイズ
        Dim RcvDataSize As Long ' 受信データサイズ

        Dim SttPos As Long      ' データ読み込み開始点
        Dim StpPos As Long      ' データ読み込み終了点

        Dim ErrCd As Long       ' エラーコード

        Dim i As Integer        ' 汎用
        Dim Ptr As Integer      ' 汎用
        Dim b As Boolean        ' 汎用

        ErrCd = 0     ' エラーコード初期化
        FileNo = -1

        '
        ' 転送先のファイル名を取得する
        '

        '
        ' ダイアログを開く
        '
        GLSample.SaveFileDialog.FileName = "Sample.GBD"
        GLSample.SaveFileDialog.Filter = "GBDファイル (*.GBD)|*.GBD|全てのファイル (*.*)|*.*"
        GLSample.SaveFileDialog.FilterIndex = 0
        GLSample.SaveFileDialog.DefaultExt = "GBD"

        If GLSample.SaveFileDialog.ShowDialog() <> DialogResult.OK Then
            GoTo DLG_CANCEL ' キャンセルボタンが押された場合に分岐
        End If

        ' ファイルネームを取得する
        FileName = GLSample.SaveFileDialog.FileName


        '
        ' 転送元の設定を行う
        '

        GLSample.AddLog("データの転送元を設定しています。" & vbCrLf)

        ' \MEM\Dample.GBDを設定
        Str = ":TRANS:SOUR DISK," & Chr(34) & "\MEM\Sample.GBD" & Chr(34)
        If DevIo.SendCommand(Str) = False Then
            ' エラー
            GoTo COMM_ERR_EXIT
        End If

        '
        ' ファイルのオープン
        '

        GLSample.AddLog("ファイルをオープンしています。" & vbCrLf)

        Try
            FileNo = FreeFile()

            FileOpen(FileNo, FileName, OpenMode.Binary, OpenAccess.Write)

            '
            ' 本体側のオープン
            '

            GLSample.AddLog("データをオープンしています。" & vbCrLf)

            ' コマンド送信
            If DevIo.SendCommand(":TRANS:OPEN?") = False Then
                ' エラー
                GoTo COMM_ERR_EXIT
            End If
            ' ステータス受信
            Size = 3    ' ステータスは3バイトのバイナリー
            If DevIo.ReadBin(Stat, Size) = False Then
                ' エラー
                GoTo COMM_ERR_EXIT
            End If
            ' ステータスの確認
            If Stat(2) <> 0 Then
                ' 本体のオープン処理に失敗した
                GLSample.AddLog("本体ファイルのオープンに失敗しました。" & vbCrLf)
                GoTo REC_ERR_EXIT
            End If


            '
            ' ヘッダーを読み込む
            '

            GLSample.AddLog("ヘッダーを読み込んでいます。" & vbCrLf)

            ' コマンド送信
            If DevIo.SendCommand(":TRANS:OUTP:HEAD?") = False Then
                ' エラー
                GoTo COMM_ERR_EXIT
            End If
            ' 受信サイズの受信
            Size = 8        ' 8バイト #6xxxxxx
            If DevIo.ReadBin(Buf, Size) = False Then
                ' エラー
                GoTo COMM_ERR_EXIT
            End If
            If Size <> 8 Then
                ' エラー
                GoTo COMM_ERR_EXIT
            End If
            ' 受信サイズを取り出す
            If Buf(0) <> Asc("#") Then
                ' おかしい？
                ' 予期せぬデータ
                ' 余分な受信データが有るかも知れないが、とりあえず無視。
                ' 念のため、本体の電源を一度切った方が良い。
                GLSample.AddLog("ヘッダーを受信できませんでした。" & vbCrLf)
                ' エラー
                GoTo COMM_ERR_EXIT
            End If

            HeaderSize = ByteToStr(Buf, 2, 6)

            ' ヘッダーの受信
            Size = HeaderSize + 4     ' ステータスとチェックサム領域を足す
            If DevIo.ReadBin(DataBuf, Size) = False Then
                ' エラー
                GoTo COMM_ERR_EXIT
            End If

            If Size <> HeaderSize + 4 Then
                ' エラー
                GoTo COMM_ERR_EXIT
            End If

            ' チェックサムの確認
            '（当サンプルプログラムでは行わない）


            ' ヘッダーの書き込み
            WriteDisk(FileNo, DataBuf, 2, HeaderSize)


            '
            ' データの読み込み
            '

            ' ヘッダーから、データ点数を得る

            ' "Counts"を探す
            Str = ByteToStr(DataBuf, 2, HeaderSize)
            Ptr = InStr(Str, "Counts")
            If Ptr <= 0 Then
                ' 見つからない
                GoTo COMM_ERR_EXIT
            End If

            Ptr = Ptr + 6   ' "Counts"をスキップ
            ' 最初に出現する数字を見つける
            b = False
            For i = 0 To 16     ' 無限ループにならないように
                If IsNumeric(Mid(Str, Ptr, 1)) = True Then
                    ' 発見
                    b = True
                    Exit For
                End If
                Ptr = Ptr + 1
            Next i

            If b = False Then
                ' 見つからなかった
                GoTo COMM_ERR_EXIT
            End If

            ' 数字を数値化する
            b = False
            For i = 0 To 32     ' 無限ループにならないように
                If IsNumeric(Mid(Str, Ptr + i, 1)) = False Then
                    DataSize = Mid(Str, Ptr, i)
                    b = True
                    Exit For
                End If
            Next i

            If b = False Then
                ' おかしい？
                GoTo COMM_ERR_EXIT
            End If

            ' データの受信
            '
            ' DataSize: データの総点数
            '
            For SttPos = 0 To DataSize - 1 Step TRANS_POINTS

                GLSample.UpdateLog("レコード " & SttPos & "/" & DataSize & " を転送しています・・・")

                ' データ転送の設定
                StpPos = SttPos + TRANS_POINTS - 1
                If StpPos >= DataSize Then
                    StpPos = DataSize - 1   ' Base 0
                End If

                Str = ":TRANS:OUTP:DATA " & SttPos + 1 & "," & StpPos + 1   ' Base 1
                If DevIo.SendCommand(Str) = False Then
                    ' エラー
                    GoTo COMM_ERR_EXIT
                End If

                ' データ取得コマンドの送信
                If DevIo.SendCommand(":TRANS:OUTP:DATA?") = False Then
                    ' エラー
                    GoTo COMM_ERR_EXIT
                End If

                ' 受信サイズの受信
                Size = 8    ' 8バイト #6xxxxxx
                If DevIo.ReadBin(Buf, Size) = False Then
                    ' エラー
                    GoTo COMM_ERR_EXIT
                End If
                If Size <> 8 Then
                    ' エラー
                    GoTo COMM_ERR_EXIT
                End If

                ' 受信サイズを取り出す
                If Buf(0) <> Asc("#") Then
                    ' おかしい？
                    ' 予期せぬデータ
                    ' 余分な受信データが有るかも知れないが、とりあえず無視。
                    ' 念のため、本体の電源を一度切った方が良い。
                    GLSample.AddLog(vbCrLf & "データを受信できませんでした。" & vbCrLf)
                    ' エラー
                    GoTo COMM_ERR_EXIT
                End If

                RcvDataSize = ByteToStr(Buf, 2, 6)

                If RcvDataSize = 0 Then
                    ' 受信データなし？
                    Exit For
                End If

				' データの受信
				Size = RcvDataSize  ' ステータスとチェックサム領域は含まれている
				If DevIo.ReadBin(DataBuf, Size) = False Then
                    ' エラー
                    GoTo COMM_ERR_EXIT
                End If
				If Size <> RcvDataSize Then
					' エラー
					GoTo COMM_ERR_EXIT
				End If

				' チェックサムの確認
				' （当サンプルプログラムでは行わない）


				' データの書き込み
				WriteDisk(FileNo, DataBuf, 2, RcvDataSize + 2)

			Next SttPos

			GLSample.UpdateLog("レコード " & DataSize & "/" & DataSize & " を転送しています・・・")
			GLSample.AddLog(vbCrLf)

            '
            ' 本体側のクローズ
            '

            GLSample.AddLog("データをクローズしています。" & vbCrLf)

            ' コマンド送信
            If DevIo.SendCommand(":TRANS:CLOSE?") = False Then
                ' エラー
                GoTo COMM_ERR_EXIT
            End If
            ' ステータス受信
            Size = 2    ' ステータスは2バイト
            If DevIo.ReadBin(Stat, Size) = False Then
                ' エラー
                GoTo COMM_ERR_EXIT
            End If
            ' ステータスの確認
            If Stat(1) <> 0 Then
                ' 本体のクローズ処理に失敗した
                GLSample.AddLog("本体ファイルのクローズに失敗しました。" & vbCrLf)
                GoTo REC_ERR_EXIT
            End If

            '
            ' ファイルのクローズ
            '

            GLSample.AddLog("ファイルをクローズしています。" & vbCrLf)

            FileClose(FileNo)

        Catch e As IOException

            ' ファイル入出力エラー
            MsgBox(Err.Description, vbOKOnly + vbCritical, "エラー")

            GLSample.AddLog(vbCrLf & "ディスク入出力エラーです。" & vbCrLf)

            ErrCd = 1
            FileNo = -1
            GoTo ERR_EXIT
        End Try

DLG_CANCEL:  ' ファイルの保存ダイアログでキャンセルされた場合

        GetRecData = ErrCd

        Exit Function



        '/////////////////////////////////////////////////////////////////
        '
        '
        '
        ' エラー処理
        '
        '



COMM_ERR_EXIT:  ' 通信エラー
        ErrCd = 2
        GoTo ERR_EXIT


REC_ERR_EXIT:  ' 本体側のエラー
        ErrCd = 3
        GoTo ERR_EXIT


ERR_EXIT:
        '
        ' 後始末
        '
        If FileNo > 0 Then
            FileClose(FileNo)
        End If

        GetRecData = Err.Description

    End Function


    '
    ' 指定バッファから指定バイトをディスクに書き込む
    '
    '   FileNo  = ファイル番号
    '   Buf()   =　データバッファ
    '   Length  = 書き込むバイト数
    '
    Private Sub WriteDisk(ByVal FileNo As Integer, ByRef Buf() As Byte, ByVal Start As Long, ByVal Size As Long)
        Dim cnt As Long
        Dim b As Byte

        On Error GoTo ERR_EXIT

        If Size > 0 Then
            For cnt = Start To Start + Size - 1
                b = Buf(cnt)
                FilePut(FileNo, b)
            Next cnt
        End If

        Exit Sub

ERR_EXIT:
        Err.Raise(Err.Number)

    End Sub

    '
    ' Byteの配列からStringへ変換する
    '
    '   Buf() = Byte配列
    '   Size  = 文字数
    '
    '   return = 変換後の文字列(String)
    '
    Private Function ByteToStr(ByRef Buf() As Byte, ByVal Start As Long, ByVal Size As Long) As String

        Dim cnt As Long     'Loop
        Dim Str As String

        Str = ""    ' 初期化

        If Size > 0 Then
            For cnt = Start To Start + Size - 1
                Str = Str & Chr(Buf(cnt))
            Next cnt
        End If

        ByteToStr = Str

    End Function


End Module
