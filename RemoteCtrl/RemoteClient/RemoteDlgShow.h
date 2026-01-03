#pragma once
#include "afxdialogex.h"


// CRemoteDlgShow dialog

class CRemoteDlgShow : public CDialogEx
{
	DECLARE_DYNAMIC(CRemoteDlgShow)

public:
	CRemoteDlgShow(CWnd* pParent = nullptr);   // standard constructor
	CPoint UserPoint2RemoteScreenPoint(CPoint &point, bool is_user = true);
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
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
};
