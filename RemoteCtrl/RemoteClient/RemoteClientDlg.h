
// RemoteClientDlg.h : header file
//

#pragma once


// CRemoteClientDlg dialog
class CRemoteClientDlg : public CDialogEx
{
// Construction
public:
	CString getItemPath(HTREEITEM hTreeItem);
	CRemoteClientDlg(CWnd* pParent = nullptr);	// standard constructor
	void DeleteTreeChildrenItem(HTREEITEM hTree);
// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_REMOTECLIENT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

private:
	/*
	* nCmd paratemters:
	* 1:get dirvers inforamtions
	* 2:get directlies or files  inforamtions
	* 3:run files
	* 4:get files
	* 5:get screnns
	*/
	int SendCommandPacket(int nCmd, bool autoCloseSocket = true,BYTE* pData = NULL, size_t nlenghth = 0);
// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnTest();
	DWORD m_remote_address;
	CString m_remote_port;
	afx_msg void OnBnClickedButtonViewfile();
	CTreeCtrl m_Tree;
	afx_msg void OnNMDblclkTreeDir(NMHDR* pNMHDR, LRESULT* pResult);
	CListCtrl m_List;
	afx_msg void OnNMRClickListFile(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnFilecontrolDownload();
};
