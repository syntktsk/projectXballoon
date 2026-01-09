<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class GLSample
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
        Me.components = New System.ComponentModel.Container
        Dim resources As System.ComponentModel.ComponentResourceManager = New System.ComponentModel.ComponentResourceManager(GetType(GLSample))
        Me.Timer = New System.Windows.Forms.Timer(Me.components)
        Me.SaveFileDialog = New System.Windows.Forms.SaveFileDialog
        Me.BT_TRANS_SETTINGS = New System.Windows.Forms.Button
        Me.BT_START = New System.Windows.Forms.Button
        Me.BT_STOP = New System.Windows.Forms.Button
        Me.ED_STATUS = New System.Windows.Forms.TextBox
        Me.MenuStrip1 = New System.Windows.Forms.MenuStrip
        Me.MN_FILE = New System.Windows.Forms.ToolStripMenuItem
        Me.MN_QUIT = New System.Windows.Forms.ToolStripMenuItem
        Me.MN_SETTING = New System.Windows.Forms.ToolStripMenuItem
        Me.MN_TRANS_SETTINGS = New System.Windows.Forms.ToolStripMenuItem
        Me.MenuStrip1.SuspendLayout()
        Me.SuspendLayout()
        '
        'Timer
        '
        '
        'BT_TRANS_SETTINGS
        '
        Me.BT_TRANS_SETTINGS.Location = New System.Drawing.Point(184, 47)
        Me.BT_TRANS_SETTINGS.Name = "BT_TRANS_SETTINGS"
        Me.BT_TRANS_SETTINGS.Size = New System.Drawing.Size(92, 30)
        Me.BT_TRANS_SETTINGS.TabIndex = 0
        Me.BT_TRANS_SETTINGS.Text = "通信設定..."
        Me.BT_TRANS_SETTINGS.UseVisualStyleBackColor = True
        '
        'BT_START
        '
        Me.BT_START.Location = New System.Drawing.Point(12, 94)
        Me.BT_START.Name = "BT_START"
        Me.BT_START.Size = New System.Drawing.Size(124, 52)
        Me.BT_START.TabIndex = 1
        Me.BT_START.Text = "ス　タ　ー　ト"
        Me.BT_START.UseVisualStyleBackColor = True
        '
        'BT_STOP
        '
        Me.BT_STOP.Enabled = False
        Me.BT_STOP.Location = New System.Drawing.Point(151, 94)
        Me.BT_STOP.Name = "BT_STOP"
        Me.BT_STOP.Size = New System.Drawing.Size(124, 52)
        Me.BT_STOP.TabIndex = 2
        Me.BT_STOP.Text = "ス　ト　ッ　プ"
        Me.BT_STOP.UseVisualStyleBackColor = True
        '
        'ED_STATUS
        '
        Me.ED_STATUS.CausesValidation = False
        Me.ED_STATUS.Location = New System.Drawing.Point(12, 152)
        Me.ED_STATUS.Multiline = True
        Me.ED_STATUS.Name = "ED_STATUS"
        Me.ED_STATUS.ReadOnly = True
        Me.ED_STATUS.ScrollBars = System.Windows.Forms.ScrollBars.Both
        Me.ED_STATUS.Size = New System.Drawing.Size(263, 132)
        Me.ED_STATUS.TabIndex = 3
        Me.ED_STATUS.WordWrap = False
        '
        'MenuStrip1
        '
        Me.MenuStrip1.Items.AddRange(New System.Windows.Forms.ToolStripItem() {Me.MN_FILE, Me.MN_SETTING})
        Me.MenuStrip1.Location = New System.Drawing.Point(0, 0)
        Me.MenuStrip1.Name = "MenuStrip1"
        Me.MenuStrip1.Size = New System.Drawing.Size(288, 24)
        Me.MenuStrip1.TabIndex = 4
        Me.MenuStrip1.Text = "MenuStrip1"
        '
        'MN_FILE
        '
        Me.MN_FILE.CheckOnClick = True
        Me.MN_FILE.DropDownItems.AddRange(New System.Windows.Forms.ToolStripItem() {Me.MN_QUIT})
        Me.MN_FILE.Name = "MN_FILE"
        Me.MN_FILE.Size = New System.Drawing.Size(66, 20)
        Me.MN_FILE.Text = "ファイル(&F)"
        '
        'MN_QUIT
        '
        Me.MN_QUIT.Name = "MN_QUIT"
        Me.MN_QUIT.Size = New System.Drawing.Size(188, 22)
        Me.MN_QUIT.Text = "アプリケーションの終了(&X)"
        '
        'MN_SETTING
        '
        Me.MN_SETTING.DropDownItems.AddRange(New System.Windows.Forms.ToolStripItem() {Me.MN_TRANS_SETTINGS})
        Me.MN_SETTING.Name = "MN_SETTING"
        Me.MN_SETTING.Size = New System.Drawing.Size(56, 20)
        Me.MN_SETTING.Text = "設定(&S)"
        '
        'MN_TRANS_SETTINGS
        '
        Me.MN_TRANS_SETTINGS.Name = "MN_TRANS_SETTINGS"
        Me.MN_TRANS_SETTINGS.Size = New System.Drawing.Size(134, 22)
        Me.MN_TRANS_SETTINGS.Text = "通信設定(&R)"
        '
        'GLSample
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 12.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(288, 296)
        Me.Controls.Add(Me.ED_STATUS)
        Me.Controls.Add(Me.BT_STOP)
        Me.Controls.Add(Me.BT_START)
        Me.Controls.Add(Me.BT_TRANS_SETTINGS)
        Me.Controls.Add(Me.MenuStrip1)
        Me.Icon = CType(resources.GetObject("$this.Icon"), System.Drawing.Icon)
        Me.MainMenuStrip = Me.MenuStrip1
        Me.Name = "GLSample"
        Me.Text = "GLSample"
        Me.MenuStrip1.ResumeLayout(False)
        Me.MenuStrip1.PerformLayout()
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Friend WithEvents Timer As System.Windows.Forms.Timer
    Friend WithEvents SaveFileDialog As System.Windows.Forms.SaveFileDialog
    Friend WithEvents BT_TRANS_SETTINGS As System.Windows.Forms.Button
    Friend WithEvents BT_START As System.Windows.Forms.Button
    Friend WithEvents BT_STOP As System.Windows.Forms.Button
    Friend WithEvents ED_STATUS As System.Windows.Forms.TextBox
    Friend WithEvents MenuStrip1 As System.Windows.Forms.MenuStrip
    Friend WithEvents MN_FILE As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents MN_QUIT As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents MN_SETTING As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents MN_TRANS_SETTINGS As System.Windows.Forms.ToolStripMenuItem

End Class
