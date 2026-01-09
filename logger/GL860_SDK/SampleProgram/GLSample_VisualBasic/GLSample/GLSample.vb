'//////////////////////////////////////////////////////////////////
'//
'// GL サンプルプログラム
'//
'//      Copyright (C) 2010 Graphtec CORP.
'//
'/////////////////////////////////////////////////////////////////


Public Class GLSample

    Dim TimEvent As Boolean ' タイマーイベントフラグ


    Private Sub MTSample_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load

        SelIf = SEL_TCP
        IpAddress = "192.168.0.1"
        PortNo = "8023"
        MachineId = "0"

    End Sub



    '
    ' アプリケーションの終了
    '
    Private Sub MN_QUIT_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MN_QUIT.Click
        End
    End Sub


    '
    ' 通信設定ボタン処理
    '
    Private Sub BT_TRANS_SETTINGS_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles BT_TRANS_SETTINGS.Click
        ' 通信設定ダイアログを開く
        DlgTransSettings.ShowDialog()    ' モーダル
    End Sub

    Private Sub MN_TRANS_SETTINGS_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MN_TRANS_SETTINGS.Click
        BT_TRANS_SETTINGS_Click(sender, e)
    End Sub


    '
    ' スタートボタンが押された時の処理
    '
    Private Sub BT_START_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles BT_START.Click
        '
        ' スタートボタンを無効化
        '
        BT_START.Enabled = False

        '
        ' ステータス表示クリア
        '
        ED_STATUS.Text = ""


        If StartProc.StartFunction() = True Then
            ' スタート処理成功

            ' ストップボタンを有効化
            BT_STOP.Enabled = True
        Else
            ' スタート処理失敗

            ' スタートボタンを有効化
            BT_START.Enabled = True
        End If

    End Sub


    '
    ' ストップボタンが押された時の処理
    '

    Private Sub BT_STOP_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles BT_STOP.Click
        ' ストップボタンを無効化
        BT_STOP.Enabled = False

        '
        ' ステータス表示クリア
        '
        ED_STATUS.Text = ""


        If StopProc.StopFunction() = True Then
            ' ストップ処理成功

            ' スタートボタンを有効化
            BT_START.Enabled = True
        Else
            ' ストップ処理失敗

            ' ストップボタンを有効化
            BT_STOP.Enabled = True
        End If

    End Sub

    '
    ' Logの最後に文字列を追加
    '
    Public Sub AddLog(ByVal Str As String)

        ED_STATUS.AppendText(Str)

    End Sub


    '
    ' Logの最終行を上書きする
    '
    Public Sub UpdateLog(ByVal Str As String)
        Dim Ed As String
        Dim Pos As Long
        Dim Find As Boolean

        Ed = ED_STATUS.Text
        If Len(Ed) = 0 Then
            ED_STATUS.Clear()
            ED_STATUS.AppendText(Str)
        Else
            Find = False

            For Pos = Len(Ed) To 1 Step -1
                If Mid(Ed, Pos, 1) = vbLf Then
                    Find = True
                    Exit For
                End If
            Next Pos

            If Find = True Then
                Ed = Mid(Ed, 1, Pos)
                ED_STATUS.Text = Ed
                ED_STATUS.AppendText(Str)
            Else
                ED_STATUS.Clear()
                ED_STATUS.AppendText(Str)
            End If
        End If


    End Sub



    '
    ' 特定の時間待ちをする
    '
    '   Ti = 待ち時間　(ms)
    '
    Public Sub WaitTime(ByVal Ti As Single)

        Timer.Enabled = False
        Timer.Interval = Ti
        TimEvent = False
        Timer.Enabled = True

        Do While TimEvent = False
            Application.DoEvents()
        Loop

    End Sub


    Private Sub Timer_Tick(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles Timer.Tick
        TimEvent = True
    End Sub
End Class
