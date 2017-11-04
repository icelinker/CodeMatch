// MatchView.cpp : implementation of the CMatchView class
//

#include "stdafx.h"
#include "Match.h"

#include "MatchDoc.h"
#include "MatchView.h"
#include "mydiblib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMatchView

IMPLEMENT_DYNCREATE(CMatchView, CScrollView)

BEGIN_MESSAGE_MAP(CMatchView, CScrollView)
	//{{AFX_MSG_MAP(CMatchView)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	ON_COMMAND(ID_IMG_YUCHULI, OnImgYuchuli)
	ON_COMMAND(ID_IMG_CENTERIDENTY, OnImgCenteridenty)
	ON_COMMAND(ID_IMG_JIEMA, OnImgJiema)
	ON_COMMAND(ID_MATCH_CODE, OnMatchCode)
	ON_COMMAND(ID_MATCH_JIAOBI, OnMatchJiaobi)
	ON_COMMAND(ID_MATCH_INCODING, OnMatchIncoding)
	ON_COMMAND(ID_HELP, OnHelp)
	ON_UPDATE_COMMAND_UI(ID_IMG_YUCHULI, OnUpdateImgYuchuli)
	ON_UPDATE_COMMAND_UI(ID_IMG_CENTERIDENTY, OnUpdateImgCenteridenty)
	ON_UPDATE_COMMAND_UI(ID_IMG_JIEMA, OnUpdateImgJiema)
	ON_UPDATE_COMMAND_UI(ID_MATCH_CODE, OnUpdateMatchCode)
	ON_UPDATE_COMMAND_UI(ID_MATCH_JIAOBI, OnUpdateMatchJiaobi)
	ON_UPDATE_COMMAND_UI(ID_MATCH_INCODING, OnUpdateMatchIncoding)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_AS, OnUpdateFileSaveAs)
	ON_COMMAND(ID_IMG_DELETE, OnImgDelete)
	ON_UPDATE_COMMAND_UI(ID_IMG_DELETE, OnUpdateImgDelete)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CScrollView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMatchView construction/destruction

CMatchView::CMatchView()
{
	// TODO: add construction code here

}

CMatchView::~CMatchView()
{
}

BOOL CMatchView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CScrollView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CMatchView drawing

void CMatchView::OnDraw(CDC* pDC)
{
	CMatchDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
	if(g_hDIB!=NULL) 
		DisplayDIB(pDC,g_hDIB);
}

/////////////////////////////////////////////////////////////////////////////
// CMatchView printing

BOOL CMatchView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CMatchView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CMatchView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CMatchView diagnostics

#ifdef _DEBUG
void CMatchView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CMatchView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CMatchDoc* CMatchView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMatchDoc)));
	return (CMatchDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMatchView message handlers

void CMatchView::OnInitialUpdate() 
{
	CScrollView::OnInitialUpdate();
	
	CSize sizeTotal;
	// TODO: calculate the total size of this view
	sizeTotal.cx = sizeTotal.cy = 100;
	SetScrollSizes(MM_TEXT, sizeTotal);
	
}

void CMatchView::OnFileOpen() 
{
	// TODO: Add your command handler code here
	// ���ļ��Ի���
    region_id=0;               //������
    average_radius=0;
    is256=0;
    step=0;
	pic++;
	char		szFilter[] = "λͼ�ļ� (*.bmp)|*.bmp|";
	CFileDialog *FileDlg = new CFileDialog(TRUE, NULL, NULL, OFN_OVERWRITEPROMPT, szFilter, NULL);
	if(FileDlg->DoModal() == IDOK)
		strPathName = FileDlg->GetPathName();
	else return;
    //����һ���ļ�����
   	CFile file;
	//��ֻ��ģʽ���ļ�
	file.Open (strPathName,CFile::modeRead);
	isFileOpen=1;
	strFileTitle=file.GetFileTitle();
	AfxGetApp()->m_pMainWnd->SetWindowText((LPCTSTR) (strFileTitle + " - Match"));
	//��ȡ�ļ���HDIB�����. ע��:��ʱֻ�Ƕ�ȡλͼ�ļ����ļ�ͷ֮��Ĳ���,�����ļ�ͷ
	g_hDIB=::ReadDIBFile (file);
	//HDIB���: ����һ��洢λͼ���ݵ��ڴ�����ĵ�ַ
	//HDIB�������:λͼ��Ϣͷ����ɫ��(����еĻ�)��DIBͼ������
	//�ر��ļ�
	file.Close ();
	//ָ��DIB��ָ��(ָ��λͼ��Ϣͷ)
	BYTE* lpDIB=(BYTE*)::GlobalLock ((HGLOBAL)g_hDIB);
	// ��ȡDIB����ɫ���е���ɫ��Ŀ
	WORD wNumColors;	
	wNumColors = ::DIBNumColors((char*)lpDIB);	
	//�ж��Ƿ���256ɫλͼ
	if (wNumColors == 256)
	{
		step=1;
		is256=1;
	}
	//���ù�����
	CSize sz(DIBWidth((char*)lpDIB),DIBHeight((char*)lpDIB));
	SetScrollSizes(MM_TEXT, sz);

	//����Ļ����ʾλͼ
	CDC* pDC=GetDC();
	DisplayDIB(pDC,g_hDIB);					
	
}

void CMatchView::OnFileSave() 
{
	// TODO: Add your command handler code here
	
	CFile file(strFileTitle+".bmp", CFile::modeReadWrite|CFile::modeCreate);
	::SaveDIB (g_hDIB,file);	

}

void CMatchView::OnFileSaveAs() 
{
	// TODO: Add your command handler code here
	char szFilter[] = "λͼ�ļ� (*.bmp)|*.bmp|";
	CFileDialog *FileDlg = new CFileDialog(FALSE, NULL, NULL, OFN_OVERWRITEPROMPT, szFilter, NULL);
	if(FileDlg->DoModal() == IDOK)
	{
	CString	strTitle= FileDlg->GetFileTitle();
	CFile file(strTitle+".bmp", CFile::modeReadWrite|CFile::modeCreate);
	::SaveDIB (g_hDIB,file);
	}
}

//�������
void CMatchView::OnHelp() 
{
	// TODO: Add your command handler code here
	ShellExecute(NULL,"open","F:\\CodeMatch\\help.txt",NULL,"",SW_SHOWDEFAULT ); 
}

//ͼ��Ԥ����
void CMatchView::OnImgYuchuli() 
{	
	// TODO: Add your command handler code here
	ImageProcess(g_hDIB);
	
	//����Ļ����ʾλͼ
	CDC* pDC=GetDC();
	DisplayDIB(pDC,g_hDIB);	
	
}

//ʶ������Բ
void CMatchView::OnImgCenteridenty() 
{
	// TODO: Add your command handler code here
	CenterIdentify(g_hDIB);
	//����Ļ����ʾλͼ
	CDC* pDC=GetDC();
	DisplayDIB(pDC,g_hDIB);	
}

//ȥ��αԲ��
void CMatchView::OnImgDelete() 
{
	// TODO: Add your command handler code here
	DelErr(g_hDIB);
	//����Ļ����ʾλͼ
	CDC* pDC=GetDC();
	DisplayDIB(pDC,g_hDIB);	
}
//��ǵ����
void CMatchView::OnImgJiema() 
{
	// TODO: Add your command handler code here
	Codejiema(g_hDIB);
	//����Ļ����ʾλͼ
	CDC* pDC=GetDC();
	DisplayDIB(pDC,g_hDIB);	
	CString str;
	str.Format("�����������!\n\n����ѱ������ļ�:%s.txt��!",strFileTitle);
	MessageBox(str);
}

//��ǵ�ƥ���1��:
void CMatchView::OnMatchCode() 
{
	// TODO: Add your command handler code here
 	 Codematch();
     ShellExecute(NULL,"open","F:\\CodeMatch\\pipei.txt",NULL,"",SW_MAXIMIZE); 
	
}

//��ǵ�ƥ���2��:
void CMatchView::OnMatchJiaobi() 
{
	// TODO: Add your command handler code here
    GetCrossRatio();
	CString str;
	str.Format("��ý��Ȳ�����!\n\n����ѱ������ļ�:6.txt,7.txt,8.txt,9.txt��!");
	MessageBox(str);
}

//��ǵ�ƥ���3��:
void CMatchView::OnMatchIncoding() 
{
	// TODO: Add your command handler code here
    Incodematch();
    ShellExecute(NULL,"open","F:\\CodeMatch\\pipei.txt",NULL,"",SW_MAXIMIZE); 	
}


void CMatchView::OnUpdateImgYuchuli(CCmdUI* pCmdUI) 
{
    	// TODO: Add your command update UI handler code here
		if(isFileOpen&&step<2)
		pCmdUI->Enable(true);
	    else
		pCmdUI->Enable(false);
}

void CMatchView::OnUpdateImgCenteridenty(CCmdUI* pCmdUI) 
{
		// TODO: Add your command update UI handler code here
		if(isFileOpen&&step>1&&step<3)
		pCmdUI->Enable(true);
	    else
		pCmdUI->Enable(false);
}

void CMatchView::OnUpdateImgDelete(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	    if(isFileOpen&&step>2&&step<4)
		pCmdUI->Enable(true);
	    else
		pCmdUI->Enable(false);
}

void CMatchView::OnUpdateImgJiema(CCmdUI* pCmdUI) 
{
		// TODO: Add your command update UI handler code here
		if(isFileOpen&&step>3&&step<5)
		pCmdUI->Enable(true);
	    else
		pCmdUI->Enable(false);
}

void CMatchView::OnUpdateMatchCode(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if(isFileOpen&&step>4&&step<6)
		pCmdUI->Enable(true);
	else
		pCmdUI->Enable(false);
}

void CMatchView::OnUpdateMatchJiaobi(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if(isFileOpen&&step>5&&step<7)
		pCmdUI->Enable(true);
	else
		pCmdUI->Enable(false);
}

void CMatchView::OnUpdateMatchIncoding(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if(isFileOpen&&step>6&&step<8)
		pCmdUI->Enable(true);
	else
		pCmdUI->Enable(false);
	
}

void CMatchView::OnUpdateFileSave(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if(isFileOpen)
		pCmdUI->Enable(true);
	else
		pCmdUI->Enable(false);
}

void CMatchView::OnUpdateFileSaveAs(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if(isFileOpen)
		pCmdUI->Enable(true);
	else
		pCmdUI->Enable(false);
}







