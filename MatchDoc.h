// MatchDoc.h : interface of the CMatchDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MATCHDOC_H__FCEDD2CF_2445_4B97_8E6A_FA66AB47AB81__INCLUDED_)
#define AFX_MATCHDOC_H__FCEDD2CF_2445_4B97_8E6A_FA66AB47AB81__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CMatchDoc : public CDocument
{
protected: // create from serialization only
	CMatchDoc();
	DECLARE_DYNCREATE(CMatchDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMatchDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMatchDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CMatchDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MATCHDOC_H__FCEDD2CF_2445_4B97_8E6A_FA66AB47AB81__INCLUDED_)
