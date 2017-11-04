// MatchDoc.cpp : implementation of the CMatchDoc class
//

#include "stdafx.h"
#include "Match.h"

#include "MatchDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMatchDoc

IMPLEMENT_DYNCREATE(CMatchDoc, CDocument)

BEGIN_MESSAGE_MAP(CMatchDoc, CDocument)
	//{{AFX_MSG_MAP(CMatchDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMatchDoc construction/destruction

CMatchDoc::CMatchDoc()
{
	// TODO: add one-time construction code here

}

CMatchDoc::~CMatchDoc()
{
}

BOOL CMatchDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CMatchDoc serialization

void CMatchDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMatchDoc diagnostics

#ifdef _DEBUG
void CMatchDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMatchDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMatchDoc commands
