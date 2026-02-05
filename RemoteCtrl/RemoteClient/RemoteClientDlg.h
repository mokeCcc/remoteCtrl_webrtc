
// RemoteClientDlg.h : header file
//

#pragma once
#include "StatusDlg.h"
#include "RemoteDlgShow.h"
#define WM_SEND_PACKET  (WM_USER  + 1)
// CRemoteClientDlg dialog

typedef struct MouseEvent {
	MouseEvent() {
		nAction = 0;
		nButton = -1;
		ptXY.x = 0;
		ptXY.y = 0;
	}
	WORD nAction; // Click(1) move(2) dounle Click(4) 
	WORD nButton;// left(1)  right(2) mid(4)
	POINT ptXY;
}MOUSENV, * PMOUSENV;

class CRemoteClientDlg : public CDialogEx
{
// Construction
public:
	CImage m_image;
	bool m_isFull;
	int m_nRemotePixX = -1, m_nRemotePixY = -1;
	CString getItemPath(HTREEITEM hTreeItem);
	CRemoteClientDlg(CWnd* pParent = nullptr);	// standard constructor
	void DeleteTreeChildrenItem(HTREEITEM hTree);
	void threadDownloadFiles();
	static void threadEntryForDownFile(void* arg);
	static void threadEntryForRemoteCtrl(void* arg);
	void threadRemoteCtrl();
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
	CStatusDlg m_StatusDlg;
	CRemoteDlgShow m_remoteDlg;
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
	afx_msg void OnFilecontrolOpen();
	afx_msg void OnFilecontrolDelete();
	afx_msg LRESULT OnSendPacket(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedBtnRemote();
	afx_msg void OnIpnFieldchangedIpaddressRemote(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEnChangeEditPort();
};
