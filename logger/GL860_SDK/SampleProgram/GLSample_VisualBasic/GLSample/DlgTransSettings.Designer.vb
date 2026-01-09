<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class DlgTransSettings
    Inherits System.Windows.Forms.Form

    'フォームがコンポーネントの一覧をクリーンアップするために dispose をオーバーライドします。
    <System.Diagnostics.DebuggerNonUserCode()> _
    Protected Overrides Sub Dispose(ByVal disposing As Boolean)
        Try
            If disposing AndAlso components IsNot Nothing Then
                components.Dispose()
            End If
        Finally
            MyBase.Dispose(disposing)
        End Try
    End Sub

    'Windows フォーム デザイナで必要です。
    Private components As System.ComponentModel.IContainer

    'メモ: 以下のプロシージャは Windows フォーム デザイナで必要です。
    'Windows フォーム デザイナを使用して変更できます。  
    'コード エディタを使って変更しないでください。
    <System.Diagnostics.DebuggerStepThrough()> _
    Private Sub InitializeComponent()
        Me.GroupBox1 = New System.Windows.Forms.GroupBox
        Me.RD_USB = New System.Windows.Forms.RadioButton
        Me.RD_TCP = New System.Windows.Forms.RadioButton
        Me.Label1 = New System.Windows.Forms.Label
        Me.Label2 = New System.Windows.Forms.Label
        Me.Label3 = New System.Windows.Forms.Label
        Me.Cancel_Button = New System.Windows.Forms.Button
        Me.OK_Button = New System.Windows.Forms.Button
        Me.ED_IP1 = New System.Windows.Forms.TextBox
        Me.Label4 = New System.Windows.Forms.Label
        Me.Label5 = New System.Windows.Forms.Label
        Me.ED_IP2 = New System.Windows.Forms.TextBox
        Me.Label6 = New System.Windows.Forms.Label
        Me.ED_IP3 = New System.Windows.Forms.TextBox
        Me.ED_IP4 = New System.Windows.Forms.TextBox
        Me.ED_PORT = New System.Windows.Forms.TextBox
        Me.ED_MACHINE_ID = New System.Windows.Forms.TextBox
        Me.GroupBox1.SuspendLayout()
        Me.SuspendLayout()
        '
        'GroupBox1
        '
        Me.GroupBox1.Controls.Add(Me.RD_USB)
        Me.GroupBox1.Controls.Add(Me.RD_TCP)
        Me.GroupBox1.Location = New System.Drawing.Point(27, 13)
        Me.GroupBox1.Name = "GroupBox1"
        Me.GroupBox1.Size = New System.Drawing.Size(97, 74)
        Me.GroupBox1.TabIndex = 2
        Me.GroupBox1.TabStop = False
        Me.GroupBox1.Text = "転送I/F"
        '
        'RD_USB
        '
        Me.RD_USB.AutoSize = True
        Me.RD_USB.Location = New System.Drawing.Point(21, 41)
        Me.RD_USB.Name = "RD_USB"
        Me.RD_USB.Size = New System.Drawing.Size(46, 16)
        Me.RD_USB.TabIndex = 1
        Me.RD_USB.TabStop = True
        Me.RD_USB.Text = "USB"
        Me.RD_USB.UseVisualStyleBackColor = True
        '
        'RD_TCP
        '
        Me.RD_TCP.AutoSize = True
        Me.RD_TCP.Location = New System.Drawing.Point(21, 19)
        Me.RD_TCP.Name = "RD_TCP"
        Me.RD_TCP.Size = New System.Drawing.Size(61, 16)
        Me.RD_TCP.TabIndex = 0
        Me.RD_TCP.TabStop = True
        Me.RD_TCP.Text = "TCP/IP"
        Me.RD_TCP.UseVisualStyleBackColor = True
        '
        'Label1
        '
        Me.Label1.AutoSize = True
        Me.Label1.Location = New System.Drawing.Point(141, 24)
        Me.Label1.Name = "Label1"
        Me.Label1.Size = New System.Drawing.Size(51, 12)
        Me.Label1.TabIndex = 3
        Me.Label1.Text = "IPアドレス"
        '
        'Label2
        '
        Me.Label2.AutoSize = True
        Me.Label2.Location = New System.Drawing.Point(141, 56)
        Me.Label2.Name = "Label2"
        Me.Label2.Size = New System.Drawing.Size(57, 12)
        Me.Label2.TabIndex = 4
        Me.Label2.Text = "ポート番号"
        '
        'Label3
        '
        Me.Label3.AutoSize = True
        Me.Label3.Location = New System.Drawing.Point(141, 88)
        Me.Label3.Name = "Label3"
        Me.Label3.Size = New System.Drawing.Size(40, 12)
        Me.Label3.TabIndex = 5
        Me.Label3.Text = "機器ID"
        '
        'Cancel_Button
        '
        Me.Cancel_Button.DialogResult = System.Windows.Forms.DialogResult.Cancel
        Me.Cancel_Button.Location = New System.Drawing.Point(479, 66)
        Me.Cancel_Button.Name = "Cancel_Button"
        Me.Cancel_Button.Size = New System.Drawing.Size(100, 21)
        Me.Cancel_Button.TabIndex = 1
        Me.Cancel_Button.Text = "キャンセル"
        '
        'OK_Button
        '
        Me.OK_Button.Location = New System.Drawing.Point(479, 12)
        Me.OK_Button.Name = "OK_Button"
        Me.OK_Button.Size = New System.Drawing.Size(100, 47)
        Me.OK_Button.TabIndex = 0
        Me.OK_Button.Text = "OK"
        '
        'ED_IP1
        '
        Me.ED_IP1.Location = New System.Drawing.Point(204, 22)
        Me.ED_IP1.MaxLength = 3
        Me.ED_IP1.Name = "ED_IP1"
        Me.ED_IP1.Size = New System.Drawing.Size(50, 19)
        Me.ED_IP1.TabIndex = 6
        '
        'Label4
        '
        Me.Label4.AutoSize = True
        Me.Label4.Location = New System.Drawing.Point(260, 26)
        Me.Label4.Name = "Label4"
        Me.Label4.Size = New System.Drawing.Size(7, 12)
        Me.Label4.TabIndex = 7
        Me.Label4.Text = "."
        '
        'Label5
        '
        Me.Label5.AutoSize = True
        Me.Label5.Location = New System.Drawing.Point(329, 26)
        Me.Label5.Name = "Label5"
        Me.Label5.Size = New System.Drawing.Size(7, 12)
        Me.Label5.TabIndex = 9
        Me.Label5.Text = "."
        '
        'ED_IP2
        '
        Me.ED_IP2.Location = New System.Drawing.Point(273, 22)
        Me.ED_IP2.MaxLength = 3
        Me.ED_IP2.Name = "ED_IP2"
        Me.ED_IP2.Size = New System.Drawing.Size(50, 19)
        Me.ED_IP2.TabIndex = 8
        '
        'Label6
        '
        Me.Label6.AutoSize = True
        Me.Label6.Location = New System.Drawing.Point(398, 26)
        Me.Label6.Name = "Label6"
        Me.Label6.Size = New System.Drawing.Size(7, 12)
        Me.Label6.TabIndex = 11
        Me.Label6.Text = "."
        '
        'ED_IP3
        '
        Me.ED_IP3.Location = New System.Drawing.Point(342, 22)
        Me.ED_IP3.MaxLength = 3
        Me.ED_IP3.Name = "ED_IP3"
        Me.ED_IP3.Size = New System.Drawing.Size(50, 19)
        Me.ED_IP3.TabIndex = 10
        '
        'ED_IP4
        '
        Me.ED_IP4.Location = New System.Drawing.Point(411, 22)
        Me.ED_IP4.MaxLength = 3
        Me.ED_IP4.Name = "ED_IP4"
        Me.ED_IP4.Size = New System.Drawing.Size(50, 19)
        Me.ED_IP4.TabIndex = 12
        '
        'ED_PORT
        '
        Me.ED_PORT.Location = New System.Drawing.Point(204, 53)
        Me.ED_PORT.Name = "ED_PORT"
        Me.ED_PORT.Size = New System.Drawing.Size(50, 19)
        Me.ED_PORT.TabIndex = 13
        '
        'ED_MACHINE_ID
        '
        Me.ED_MACHINE_ID.Location = New System.Drawing.Point(204, 85)
        Me.ED_MACHINE_ID.Name = "ED_MACHINE_ID"
        Me.ED_MACHINE_ID.Size = New System.Drawing.Size(50, 19)
        Me.ED_MACHINE_ID.TabIndex = 14
        '
        'DlgTransSettings
        '
        Me.AcceptButton = Me.OK_Button
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 12.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.CancelButton = Me.Cancel_Button
        Me.ClientSize = New System.Drawing.Size(591, 119)
        Me.Controls.Add(Me.ED_MACHINE_ID)
        Me.Controls.Add(Me.ED_PORT)
        Me.Controls.Add(Me.ED_IP4)
        Me.Controls.Add(Me.Label6)
        Me.Controls.Add(Me.ED_IP3)
        Me.Controls.Add(Me.Label5)
        Me.Controls.Add(Me.ED_IP2)
        Me.Controls.Add(Me.Label4)
        Me.Controls.Add(Me.ED_IP1)
        Me.Controls.Add(Me.Label3)
        Me.Controls.Add(Me.Label2)
        Me.Controls.Add(Me.Label1)
        Me.Controls.Add(Me.GroupBox1)
        Me.Controls.Add(Me.Cancel_Button)
        Me.Controls.Add(Me.OK_Button)
        Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog
        Me.MaximizeBox = False
        Me.MinimizeBox = False
        Me.Name = "DlgTransSettings"
        Me.ShowInTaskbar = False
        Me.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent
        Me.Text = "通信設定"
        Me.GroupBox1.ResumeLayout(False)
        Me.GroupBox1.PerformLayout()
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Friend WithEvents GroupBox1 As System.Windows.Forms.GroupBox
    Friend WithEvents RD_USB As System.Windows.Forms.RadioButton
    Friend WithEvents RD_TCP As System.Windows.Forms.RadioButton
    Friend WithEvents Label1 As System.Windows.Forms.Label
    Friend WithEvents Label2 As System.Windows.Forms.Label
    Friend WithEvents Label3 As System.Windows.Forms.Label
    Friend WithEvents Cancel_Button As System.Windows.Forms.Button
    Friend WithEvents OK_Button As System.Windows.Forms.Button
    Friend WithEvents ED_IP1 As System.Windows.Forms.TextBox
    Friend WithEvents Label4 As System.Windows.Forms.Label
    Friend WithEvents Label5 As System.Windows.Forms.Label
    Friend WithEvents ED_IP2 As System.Windows.Forms.TextBox
    Friend WithEvents Label6 As System.Windows.Forms.Label
    Friend WithEvents ED_IP3 As System.Windows.Forms.TextBox
    Friend WithEvents ED_IP4 As System.Windows.Forms.TextBox
    Friend WithEvents ED_PORT As System.Windows.Forms.TextBox
    Friend WithEvents ED_MACHINE_ID As System.Windows.Forms.TextBox

End Class
