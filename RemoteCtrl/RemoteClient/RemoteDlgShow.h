#pragma once
#include "afxdialogex.h"


// CRemoteDlgShow dialog

class CRemoteDlgShow : public CDialogEx
{
	DECLARE_DYNAMIC(CRemoteDlgShow)

public:
	CRemoteDlgShow(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CRemoteDlgShow();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_SHOW };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CStatic m_picture;
	virtual BOOL OnInitDialog();
};
