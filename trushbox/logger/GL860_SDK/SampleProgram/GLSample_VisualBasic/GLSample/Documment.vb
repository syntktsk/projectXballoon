'//////////////////////////////////////////////////////////////////
'//
'// GL サンプルプログラム
'//
'//      Copyright (C) 2010 Graphtec CORP.
'//
'/////////////////////////////////////////////////////////////////

Module Documment


    ' 定数

    Public Const SEL_TCP As Integer = 0
    Public Const SEL_USB As Integer = 1

    Public Const ASOCK_STREAM As Integer = 0
    Public Const ASOCK_DGRAM As Integer = 1

    Public Const MC_GL860 As Integer = 0


    ' Public変数

    Public SelIf As Integer         ' I/F セレクト
    Public IpAddress As String      ' IPアドレス
    Public PortNo As String         ' PORT番号
    Public MachineId As String      ' 機器ID

    Public MachineKind As Integer   ' 接続されてる機種
    Public NumCh As Integer         ' 接続機器のCH数

End Module
