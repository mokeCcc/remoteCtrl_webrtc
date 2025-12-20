
// RemoteClientDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "RemoteClient.h"
#include "RemoteClientDlg.h"
#include "afxdialogex.h"
#include "ClientSocket.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CRemoteClientDlg dialog

VOID Dump(BYTE* pData, unsigned int  nSize){
	std::string strOut;
	for (unsigned int i = 0; i < nSize; i++) {
		char buf[8] = {};
		if (i > 0 && (i % 16 == 0)) strOut += "\n";
		snprintf(buf, sizeof(buf), "%02X ", pData[i] & 0xff);
		strOut += buf;
	}
	strOut += "\n";
	OutputDebugStringA(strOut.c_str());
}

CString CRemoteClientDlg::getItemPath(HTREEITEM hTreeItem)
{
	CString strRet, strTmp;
	do {
		strTmp = m_Tree.GetItemText(hTreeItem);
		strRet = strTmp + '\\' + strRet;
		hTreeItem = m_Tree.GetParentItem(hTreeItem);
	} while (hTreeItem != NULL);
	return strRet;
}

CRemoteClientDlg::CRemoteClientDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_REMOTECLIENT_DIALOG, pParent)
	, m_remote_address(0)
	, m_remote_port(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CRemoteClientDlg::DeleteTreeChildrenItem(HTREEITEM hTree)
{
	HTREEITEM hSub = NULL;
	do {
		hSub = m_Tree.GetChildItem(hTree);
		if (hSub != NULL)
			m_Tree.DeleteItem(hSub);
	} while (hSub != NULL);

}

void CRemoteClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_IPAddress(pDX, IDC_IPADDRESS_REMOTE, m_remote_address);
	DDX_Text(pDX, IDC_EDIT_PORT, m_remote_port);
	DDX_Control(pDX, IDC_TREE_DIR, m_Tree);
	DDX_Control(pDX, IDC_LIST_FILE, m_List);
}

int CRemoteClientDlg::SendCommandPacket(int nCmd, bool autoCloseSocket , BYTE* pData, size_t nlenghth)
{
	UpdateData();
	CClientSocket* pClient = CClientSocket::getInstance();
	bool ret = pClient->InitializeSocket(m_remote_address, atoi((LPCTSTR)m_remote_port));
	if (!ret) {
		AfxMessageBox("network init failed!");
		return -1;
	}
	CPacket pack(nCmd, pData, nlenghth);
	ret = pClient->Send(pack);
	TRACE("Client send result:%d\r\n", ret);
	int dwCommand = pClient->DealCommand();
	TRACE("Client DealCommand:%d\r\n", dwCommand);
	if(autoCloseSocket)
		pClient->CloseServerSocket();
	return dwCommand;
}

BEGIN_MESSAGE_MAP(CRemoteClientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_TEST, &CRemoteClientDlg::OnBnClickedBtnTest)
	ON_BN_CLICKED(IDC_BUTTON_VIEWFILE, &CRemoteClientDlg::OnBnClickedButtonViewfile)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE_DIR, &CRemoteClientDlg::OnNMDblclkTreeDir)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_FILE, &CRemoteClientDlg::OnNMRClickListFile)
END_MESSAGE_MAP()


// CRemoteClientDlg message handlers

BOOL CRemoteClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	UpdateData();
	m_remote_address = 0x7F000001;
	m_remote_port = _T("8086");
	UpdateData(FALSE);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CRemoteClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CRemoteClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CRemoteClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CRemoteClientDlg::OnBnClickedBtnTest()
{
	SendCommandPacket(1981);
}


void CRemoteClientDlg::OnBnClickedButtonViewfile()
{
	// TODO: Add your control notification handler code here
	int ret = SendCommandPacket(1);
	if (ret == -1) {
		AfxMessageBox("command handled error!");
		return ;
	}
	CClientSocket* pClient = CClientSocket::getInstance();
	std::string drivers_info = pClient->GetPacket().strData;
	std::string dr;
	m_Tree.DeleteAllItems();
	for (size_t i = 0; i < drivers_info.size(); i++) {
		if (drivers_info[i] == ',') {
			dr += ":";
			HTREEITEM hTreeTmp = m_Tree.InsertItem(dr.c_str(),TVI_ROOT,TVI_LAST);
			m_Tree.InsertItem("", hTreeTmp, TVI_LAST);
			dr.clear();
			continue;
		}
		dr += drivers_info[i];
	}

}



void CRemoteClientDlg::OnNMDblclkTreeDir(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;
	CPoint pos;
	GetCursorPos(&pos);
	m_Tree.ScreenToClient(&pos);

	HTREEITEM hTreeItem = m_Tree.HitTest(pos,0);

	if (hTreeItem == NULL) {
		TRACE(" hTreeItem is empty");
		return;
	}
	if (m_Tree.GetChildItem(hTreeItem) == NULL) {
		return;
	}
	DeleteTreeChildrenItem(hTreeItem);
	m_List.DeleteAllItems();
	CString strPath = getItemPath(hTreeItem);
	SendCommandPacket(2,false, (BYTE*)(LPCTSTR)strPath, strPath.GetLength());
	CClientSocket* pClient = CClientSocket::getInstance();
	PFILEINFO pfinfo = (PFILEINFO)(pClient->GetPacket().strData.c_str());

	while (pfinfo->hasNext) {
		if (pfinfo->IsDirectory) {
			if (CString(pfinfo->szFileName) == "." || (CString(pfinfo->szFileName) == "..")) {
				int dwCmd = pClient->DealCommand();
				TRACE("Client DealCommand:%d\r\n", dwCmd);
				if (dwCmd < 0) break;
				pfinfo = (PFILEINFO)(pClient->GetPacket().strData.c_str());
				continue;
			}
			HTREEITEM hTreeTmp = m_Tree.InsertItem(pfinfo->szFileName, hTreeItem, TVI_LAST);
			m_Tree.InsertItem("", hTreeTmp, TVI_LAST);
		}
		else {
			m_List.InsertItem(0, pfinfo->szFileName);
		}
		
		
		//m_Tree.InsertItem(pfinfo->szFileName, hTreeItem, TVI_LAST);
		int dwCmd = pClient->DealCommand();
		TRACE("Client DealCommand:%d\r\n", dwCmd);
		if (dwCmd < 0) break;
		pfinfo = (PFILEINFO)(pClient->GetPacket().strData.c_str());
	}


	pClient->CloseServerSocket();

}

void CRemoteClientDlg::OnNMRClickListFile(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	CPoint ptMouse,ptList;
	GetCursorPos(&ptMouse);
	ptList = ptMouse;
	m_List.ScreenToClient(&ptList);

	int ListSelected = m_List.HitTest(ptList);
	if (ListSelected < 0)  return;
	CMenu menu;
	menu.LoadMenu(IDR_MENU_RCLK);
	CMenu * Popup = menu.GetSubMenu(0);
	if (Popup != NULL) {
		Popup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, ptMouse.x, ptMouse.y, this);
	}

}
