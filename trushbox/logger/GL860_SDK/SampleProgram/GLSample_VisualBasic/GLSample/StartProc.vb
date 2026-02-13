'//////////////////////////////////////////////////////////////////
'//
'// GL サンプルプログラム
'//
'//      Copyright (C) 2010 Graphtec CORP.
'//
'/////////////////////////////////////////////////////////////////


Module StartProc

    '
    ' スタートボタン押下時の処理
    '
    '   return = スタート成功：True
    '            スタート失敗：False
    '
    Public Function StartFunction() As Boolean

        Dim Str As String   ' クエリー受信等、文字列ワーク
        Dim Ptr As Long     ' ポインターワーク
        Dim Ch As Long      ' CHループ

        Dim i As Long       ' 汎用
        Dim b As Boolean    ' 汎用
        Dim l As Long       ' 汎用

        StartFunction = False   ' 戻り値初期化

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
        ' 機種判別
        '
        If DevIo.SendQuery("*IDN?", Str) = False Then
            ' エラー
            GoTo COMM_ERR_EXIT
        End If

        '
        ' 文字列は "*IDN GRAPHTEC,GL860,0,1.00"
        ' の形式で戻って来るので、目的の機種名が
        ' 含まれるかを検査
        '
        If InStr(Str, "GL860") <= 0 Then   'GL260検索する
            GLSample.AddLog("未対応の機種です。" & vbCrLf)
            GoTo ERR_EXIT
        Else
            MachineKind = MC_GL860  ' GL860が接続されている
            GLSample.AddLog("機種：GL860" & vbCrLf)
        End If


        '
        ' チャネル数の取得
        '
        If DevIo.SendQuery(":INFO:CH?", Str) = False Then
            ' エラー
            GoTo COMM_ERR_EXIT
        End If

        '
        ' 文字列は ":INFO:CH 10"
        ' の形式で戻って来るので、最初のスペース文字を検出
        '
        Ptr = InStr(Str, " ")
        If Ptr <= 0 Then
            ' 区切り文字が見つからない
            GoTo COMM_ERR_EXIT
        End If

        Ptr = Ptr + 1           ' CH数文字列の先頭
        Str = Mid(Str, Ptr)     ' CH数文字列を抽出
        NumCh = Str             ' CH数

        GLSample.AddLog("CH数：" & NumCh & vbCrLf)


        '
        ' AMPの設定
        '
        ' 全CH同じ設定とする。
        ' DC/10V/FILT OFF
        ' Alarm OFF
        '
        For Ch = 1 To NumCh
            GLSample.UpdateLog("AMPの設定を行っています・・・CH" & Ch)

            ' 入力、レンジ、フィルタ
            Str = ":AMP:CH" & Ch & ":INP DC;RANG 10V;FILT OFF"
            If DevIo.SendCommand(Str) = False Then
                ' エラー
                GoTo COMM_ERR_EXIT
            End If

            ' アラーム
            Str = ":ALAR:CH" & Ch & ":SET OFF"
            If DevIo.SendCommand(Str) = False Then
                ' エラー
                GoTo COMM_ERR_EXIT
            End If

        Next Ch

        GLSample.AddLog(vbCrLf)


        '
        ' LOGICの設定
        '
        ' OFFとする
        '
        GLSample.AddLog("LOGICとPULSEを設定しています。" & vbCrLf)

        ' Logic記録
        ' GL800の場合はLOGICとPULSEをここでまとめてOFF
        If DevIo.SendCommand(":LOGIPUL:FUNC OFF") = False Then
            ' エラー
            GoTo COMM_ERR_EXIT
        End If


        '
        ' TRIGGERの設定
        '
        ' OFFとする。
        '
        GLSample.AddLog("TRIGGERを設定しています。" & vbCrLf)

        ' スタートトリガ
        If DevIo.SendCommand(":TRIG:COND0:SOUR OFF") = False Then
            ' エラー
            GoTo COMM_ERR_EXIT
        End If

        ' ストップトリガ
        If DevIo.SendCommand(":TRIG:COND1:SOUR OFF") = False Then
            ' エラー
            GoTo COMM_ERR_EXIT
        End If


        '
        ' データの保存先を設定
        '
        '  \MEM\Sample.GBD
        '  に固定。
        '
        '
        GLSample.AddLog("データの保存先を設定しています。" & vbCrLf)

        ' 既にファイルが作成されている場合に備えてファイル消去コマンドを送信。
        ' GLでは同名のファイルが存在した場合、自動で最後に_CPを付加する為。
        ' 本サンプルプログラムでは、固定のファイルを使用するために、削除する。

        Str = ":FILE:RM " & Chr(34) & "\MEM\Sample.GBD" & Chr(34)
        If DevIo.SendCommand(Str) = False Then
            ' エラー
            GoTo COMM_ERR_EXIT
        End If

        ' \MEM\Dample.GBDを設定
        Str = ":DATA:CAPT DISK," & Chr(34) & "\MEM\Sample.GBD" & Chr(34)
        If DevIo.SendCommand(Str) = False Then
            ' エラー
            GoTo COMM_ERR_EXIT
        End If

        '
        ' サンプリングの設定
        '
        ' 1sに固定
        '
        '
        GLSample.AddLog("サンプリングを設定しています。" & vbCrLf)

        If DevIo.SendCommand(":DATA:SAMP 1S") = False Then
            ' エラー
            GoTo COMM_ERR_EXIT
        End If


        '
        ' 記録スタート
        '

        GLSample.AddLog("記録をスタートしています。" & vbCrLf)

        If DevIo.SendCommand(":MEAS:START") = False Then
            ' エラー
            GoTo COMM_ERR_EXIT
        End If

        '
        ' 本当にスタートしたか確認する
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

            Ptr = Ptr + 1       ' ステータス文字列の先頭
            Str = Mid(Str, Ptr) ' ステータス文字列を抽出
            l = Str             ' ステータス

            l = l And 1         'REC Bit

            If l <> 0 Then
                ' 本体が記録状態になった
                b = True
                Exit For
            End If

            GLSample.WaitTime(500) ' 500ms Wait

        Next i

        If b = False Then
            ' 本体はスタートしてない？
            GLSample.AddLog("本体がスタートできませんでした。" & vbCrLf)
            GoTo ERR_EXIT
        End If

        GLSample.AddLog("スタートしました。" & vbCrLf)


        '
        ' デバイスをクローズ
        '
        GLSample.AddLog("デバイスを閉じています。" & vbCrLf)

        DevIo.DevClose()

        '
        ' 通信を破棄
        '
        DevIo.DevDestroy()

        StartFunction = True

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
        StartFunction = False

        DevIo.DevClose()
        DevIo.DevDestroy()

    End Function

End Module
