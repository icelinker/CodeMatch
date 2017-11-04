; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CMatchView
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "Match.h"
LastPage=0

ClassCount=5
Class1=CMatchApp
Class2=CMatchDoc
Class3=CMatchView
Class4=CMainFrame

ResourceCount=2
Resource1=IDR_MAINFRAME
Class5=CAboutDlg
Resource2=IDD_ABOUTBOX

[CLS:CMatchApp]
Type=0
HeaderFile=Match.h
ImplementationFile=Match.cpp
Filter=N

[CLS:CMatchDoc]
Type=0
HeaderFile=MatchDoc.h
ImplementationFile=MatchDoc.cpp
Filter=N

[CLS:CMatchView]
Type=0
HeaderFile=MatchView.h
ImplementationFile=MatchView.cpp
Filter=C
BaseClass=CScrollView
VirtualFilter=VWC


[CLS:CMainFrame]
Type=0
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
Filter=T
LastObject=ID_IMG_YUCHULI




[CLS:CAboutDlg]
Type=0
HeaderFile=Match.cpp
ImplementationFile=Match.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[MNU:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_OPEN
Command2=ID_FILE_SAVE
Command3=ID_FILE_SAVE_AS
Command4=ID_APP_EXIT
Command5=ID_IMG_YUCHULI
Command6=ID_IMG_CENTERIDENTY
Command7=ID_IMG_DELETE
Command8=ID_IMG_JIEMA
Command9=ID_MATCH_CODE
Command10=ID_MATCH_JIAOBI
Command11=ID_MATCH_INCODING
Command12=ID_APP_ABOUT
Command13=ID_HELP
CommandCount=13

[ACL:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_FILE_PRINT
Command5=ID_EDIT_UNDO
Command6=ID_EDIT_CUT
Command7=ID_EDIT_COPY
Command8=ID_EDIT_PASTE
Command9=ID_EDIT_UNDO
Command10=ID_EDIT_CUT
Command11=ID_EDIT_COPY
Command12=ID_EDIT_PASTE
Command13=ID_NEXT_PANE
Command14=ID_PREV_PANE
CommandCount=14

[TB:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_EDIT_CUT
Command5=ID_EDIT_COPY
Command6=ID_EDIT_PASTE
Command7=ID_FILE_PRINT
Command8=ID_APP_ABOUT
CommandCount=8

