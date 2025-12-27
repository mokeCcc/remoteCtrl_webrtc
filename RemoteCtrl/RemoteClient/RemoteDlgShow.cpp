// RemoteDlgShow.cpp : implementation file
//

#include "pch.h"
#include "RemoteClient.h"
#include "afxdialogex.h"
#include "RemoteDlgShow.h"
#include "RemoteClientDlg.h"

// CRemoteDlgShow dialog

IMPLEMENT_DYNAMIC(CRemoteDlgShow, CDialogEx)

CRemoteDlgShow::CRemoteDlgShow(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_SHOW, pParent)
{

}

CRemoteDlgShow::~CRemoteDlgShow()
{
}

void CRemoteDlgShow::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PIC, m_picture);
}


BEGIN_MESSAGE_MAP(CRemoteDlgShow, CDialogEx)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CRemoteDlgShow message handlers

void CRemoteDlgShow::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if (nIDEvent == 0) {
		TRACE("inside OnTimer! \r\n");
		CRemoteClientDlg* parent = reinterpret_cast<CRemoteClientDlg *> (GetParent());
		if( parent->m_isFull == true) {
			CRect rect;
			m_picture.GetWindowRect(rect);
		//	parent->m_image.BitBlt(m_picture.GetDC()->GetSafeHdc(), 0,0,SRCCOPY);
			parent->m_image.StretchBlt(m_picture.GetDC()->GetSafeHdc(),
				0, 0, rect.Width(), rect.Height(), SRCCOPY);
			m_picture.InvalidateRect(NULL);
			parent->m_image.Destroy();
			parent->m_isFull = false;
		}
	}
	CDialogEx::OnTimer(nIDEvent);
}

BOOL CRemoteDlgShow::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	SetTimer(0, 50, NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
