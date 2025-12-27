#pragma once
#include "afxdialogex.h"


// RemoteShowDlg dialog

class RemoteShowDlg : public CDialog
{
	DECLARE_DYNAMIC(RemoteShowDlg)

public:
	RemoteShowDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~RemoteShowDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_REMOTE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
