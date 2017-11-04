// MatchView.h : interface of the CMatchView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MATCHVIEW_H__AE1CBB3C_660B_4031_876B_6D16326E9CF5__INCLUDED_)
#define AFX_MATCHVIEW_H__AE1CBB3C_660B_4031_876B_6D16326E9CF5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CMatchView : public CScrollView
{
protected: // create from serialization only
	CMatchView();
	DECLARE_DYNCREATE(CMatchView)

// Attributes
public:
	CMatchDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMatchView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMatchView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CMatchView)
	afx_msg void OnFileOpen();
	afx_msg void OnFileSave();
	afx_msg void OnFileSaveAs();
	afx_msg void OnImgYuchuli();
	afx_msg void OnImgCenteridenty();
	afx_msg void OnImgJiema();
	afx_msg void OnMatchCode();
	afx_msg void OnMatchJiaobi();
	afx_msg void OnMatchIncoding();
	afx_msg void OnHelp();
	afx_msg void OnUpdateImgYuchuli(CCmdUI* pCmdUI);
	afx_msg void OnUpdateImgCenteridenty(CCmdUI* pCmdUI);
	afx_msg void OnUpdateImgJiema(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMatchCode(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMatchJiaobi(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMatchIncoding(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileSave(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileSaveAs(CCmdUI* pCmdUI);
	afx_msg void OnImgDelete();
	afx_msg void OnUpdateImgDelete(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in MatchView.cpp
inline CMatchDoc* CMatchView::GetDocument()
   { return (CMatchDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MATCHVIEW_H__AE1CBB3C_660B_4031_876B_6D16326E9CF5__INCLUDED_)
