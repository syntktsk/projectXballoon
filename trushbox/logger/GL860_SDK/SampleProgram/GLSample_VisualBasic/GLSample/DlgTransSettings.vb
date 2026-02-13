'//////////////////////////////////////////////////////////////////
'//
'// GL サンプルプログラム
'//
'//      Copyright (C) 2010 Graphtec CORP.
'//
'/////////////////////////////////////////////////////////////////


Imports System.Windows.Forms


Public Class DlgTransSettings

    Private Sub DlgTransSettings_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        ' IPアドレスのロード
        ED_IP1.Text = GetIpStr(IpAddress, 0)
        ED_IP2.Text = GetIpStr(IpAddress, 1)
        ED_IP3.Text = GetIpStr(IpAddress, 2)
        ED_IP4.Text = GetIpStr(IpAddress, 3)
        ' PORT番号のロード
        ED_PORT.Text = PortNo
        ' 機器IDのロード
        ED_MACHINE_ID.Text = MachineId

        ' ラジオボタンと、TEXTボックスの有効/無効を決める
        If SelIf = SEL_TCP Then
            ' TCP/IPの場合
            ED_IP1.Enabled = True
            ED_IP2.Enabled = True
            ED_IP3.Enabled = True
            ED_IP4.Enabled = True

            ED_PORT.Enabled = True

            ED_MACHINE_ID.Enabled = False

            RD_TCP.Checked = True
            RD_USB.Checked = False

        Else
            ' USBの場合
            ED_IP1.Enabled = False
            ED_IP2.Enabled = False
            ED_IP3.Enabled = False
            ED_IP4.Enabled = False

            ED_PORT.Enabled = False

            ED_MACHINE_ID.Enabled = True

            RD_TCP.Checked = False
            RD_USB.Checked = True


        End If



    End Sub

    '
    ' IPアドレス文字列から個々の数値を取り出す
    '
    '   IpAdr = IPアドレス文字列 "192.168.0.1"
    '   nPoint = 取り出す位置　0:最初の数字(192)　1:2番目の数字(168)・・・・
    '
    '   戻り値：取り出された数字文字列
    '
    Private Function GetIpStr(ByVal IpStr As String, ByVal nPoint As Integer) As String

        Dim Adr(4) As String    ' 分解したアドレス文字列
        Dim APtr As Integer     ' Adrの添え字
        Dim IPtr As Integer     ' IpStrの中身を指すポインタ

        Dim c As String


        APtr = 0
        IPtr = 1
        While (APtr < 4)
            c = Mid(IpStr, IPtr, 1)
            If c <> "." And c <> "" Then
                Adr(APtr) = Adr(APtr) + c
            Else
                APtr = APtr + 1
            End If

            IPtr = IPtr + 1

        End While

        GetIpStr = Adr(nPoint)

    End Function


    '
    ' TCP/IPラジオボタンがクリックされた場合
    '
    Private Sub RD_TCP_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles RD_TCP.Click
        ' TCP/IP関係の設定をイネーブル
        ED_IP1.Enabled = True
        ED_IP2.Enabled = True
        ED_IP3.Enabled = True
        ED_IP4.Enabled = True

        ED_PORT.Enabled = True

        ' USB関係の設定はディセーブル
        ED_MACHINE_ID.Enabled = False

    End Sub

    '
    ' USBラジオボタンがクリックされた場合
    '
    Private Sub RD_USB_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles RD_USB.Click
        ' TCP/IP関係の設定をイネーブル
        ED_IP1.Enabled = False
        ED_IP2.Enabled = False
        ED_IP3.Enabled = False
        ED_IP4.Enabled = False

        ED_PORT.Enabled = False

        ' USB関係の設定はディセーブル
        ED_MACHINE_ID.Enabled = True

    End Sub

    '
    ' OKボタンが押された場合
    '
    ' コントロールの中身を変数へ反映する
    '
    Private Sub OK_Button_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles OK_Button.Click

        ' IPアドレス
        IpAddress = ED_IP1.Text & "." & ED_IP2.Text & "." & ED_IP3.Text & "." & ED_IP4.Text
        ' PORT番号
        PortNo = ED_PORT.Text
        ' 機器ID
        MachineId = ED_MACHINE_ID.Text
        ' I/Fセレクト
        If RD_TCP.Checked = True Then
            SelIf = SEL_TCP
        Else
            SelIf = SEL_USB
        End If

        ' ダイアログを閉じる

        Me.DialogResult = System.Windows.Forms.DialogResult.OK
        Me.Close()
    End Sub

    '
    ' Cancelボタンが押された場合
    '
    Private Sub Cancel_Button_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles Cancel_Button.Click
        ' そのままダイアログを閉じる
        Me.DialogResult = System.Windows.Forms.DialogResult.Cancel
        Me.Close()
    End Sub


    '
    ' TEXTボックスの数値入力チェック
    '
    '   Ed  = TEXTボックスオブジェクト
    '   Min = 数値の最小値
    '   Max = 数値の最大値
    '
    '   Retuen = False : 問題なし
    '            True  : 数字以外の文字が存在するか、数値の範囲オーバー
    '
    Private Function NumValidate(ByVal Ed As TextBox, ByVal Min As Long, ByVal Max As Long) As Boolean

        Dim Str As String

        NumValidate = False

        ' 数値以外が入力されていないかチェック
        If Not IsNumeric(Ed.Text) Then
            MsgBox("このフィールドには数値を入力してください。", , "入力エラー")
            NumValidate = True
        Else
            ' 0～255の範囲かチェック
            If Ed.Text > Max Or Ed.Text < Min Then
                Str = Min & "～" & Max & "の範囲の数値を入力してください。"
                MsgBox(Str, , "入力エラー")
                NumValidate = True
            End If
        End If


    End Function


    '
    ' IPアドレス１の入力チェック
    '
    Private Sub ED_IP1_Validating(ByVal sender As System.Object, ByVal e As System.ComponentModel.CancelEventArgs) Handles ED_IP1.Validating
        e.Cancel = NumValidate(ED_IP1, 0, 255)
    End Sub

    '
    ' IPアドレス２の入力チェック
    '
    Private Sub ED_IP2_Validating(ByVal sender As System.Object, ByVal e As System.ComponentModel.CancelEventArgs) Handles ED_IP2.Validating
        e.Cancel = NumValidate(ED_IP2, 0, 255)
    End Sub

    '
    ' IPアドレス３の入力チェック
    '
    Private Sub ED_IP3_Validating(ByVal sender As System.Object, ByVal e As System.ComponentModel.CancelEventArgs) Handles ED_IP3.Validating
        e.Cancel = NumValidate(ED_IP3, 0, 255)
    End Sub


    '
    ' IPアドレス４の入力チェック
    '
    Private Sub ED_IP4_Validating(ByVal sender As System.Object, ByVal e As System.ComponentModel.CancelEventArgs) Handles ED_IP4.Validating
        e.Cancel = NumValidate(ED_IP4, 0, 255)
    End Sub



    '
    ' PORT番号の入力チェック
    '
    Private Sub ED_PORT_Validating(ByVal sender As System.Object, ByVal e As System.ComponentModel.CancelEventArgs) Handles ED_PORT.Validating
        e.Cancel = NumValidate(ED_PORT, 0, 65535)
    End Sub


    '
    ' 機器IDの入力チェック
    '
    Private Sub ED_MACHINE_ID_Validating(ByVal sender As System.Object, ByVal e As System.ComponentModel.CancelEventArgs) Handles ED_MACHINE_ID.Validating
        e.Cancel = NumValidate(ED_MACHINE_ID, 0, 9)
    End Sub

End Class
