// RemoteShowDlg.cpp : implementation file
//

#include "pch.h"
#include "RemoteClient.h"
#include "afxdialogex.h"
#include "RemoteShowDlg.h"


// RemoteShowDlg dialog

IMPLEMENT_DYNAMIC(RemoteShowDlg, CDialog)

RemoteShowDlg::RemoteShowDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_DLG_REMOTE, pParent)
{

}

RemoteShowDlg::~RemoteShowDlg()
{
}

void RemoteShowDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(RemoteShowDlg, CDialog)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// RemoteShowDlg message handlers

void RemoteShowDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default

	CDialog::OnTimer(nIDEvent);
}
