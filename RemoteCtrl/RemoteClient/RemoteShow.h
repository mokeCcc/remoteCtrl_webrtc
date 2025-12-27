#pragma once
#include "afxdialogex.h"


// CRemoteShow dialog

class CRemoteShow : public CDialog
{
	DECLARE_DYNAMIC(CRemoteShow)

public:
	CRemoteShow(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CRemoteShow();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_REMOTE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
