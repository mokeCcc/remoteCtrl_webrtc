// RemoteDlgShow.cpp : implementation file
//

#include "pch.h"
#include "RemoteClient.h"
#include "afxdialogex.h"
#include "RemoteDlgShow.h"
#include "RemoteClientDlg.h"
#include "ClientController.h"
// CRemoteDlgShow dialog

IMPLEMENT_DYNAMIC(CRemoteDlgShow, CDialogEx)

CRemoteDlgShow::CRemoteDlgShow(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_SHOW, pParent)
{

}

CPoint CRemoteDlgShow::UserPoint2RemoteScreenPoint(CPoint &point, bool is_user)
{
	CRect ClientRect;
	if (!is_user) ScreenToClient(&point);
	m_picture.GetClientRect(ClientRect);
	CRemoteClientDlg* parent = reinterpret_cast<CRemoteClientDlg*> (GetParent());
	if(parent->m_nRemotePixX != -1 && parent->m_nRemotePixY != -1)
		return CPoint(point.x * parent->m_nRemotePixX / ClientRect.Width(),point.y * parent->m_nRemotePixY /ClientRect.Height());
	return CPoint(0, 0);
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
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MBUTTONDBLCLK()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDBLCLK()
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

void CRemoteDlgShow::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CPoint remote = UserPoint2RemoteScreenPoint(point);
	MOUSENV event;
	event.nAction = 1;
	event.nButton = 0;
	event.ptXY = remote;
	CRemoteClientDlg* pParent = (CRemoteClientDlg*)GetParent();
	pParent->SendMessage(WM_SEND_PACKET, 9 << 1 | 1, (LPARAM)(&event));
	//CDialogEx::OnRButtonDown(nFlags, point);
	CDialogEx::OnLButtonDblClk(nFlags, point);
}

void CRemoteDlgShow::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	//	WORD nAction; // Click(0) double Click(1) up(2) 
	//WORD nButton;// left(0)  right(1) mid(2)
	//event.nAction = 2;
	CPoint remote = UserPoint2RemoteScreenPoint(point);
	MOUSENV event;
	event.nAction = 0;
	event.nButton = 0;
	event.ptXY = remote;
	CRemoteClientDlg* pParent = (CRemoteClientDlg*)GetParent();
	pParent->SendMessage(WM_SEND_PACKET, 9 << 1 | 1, (LPARAM)(&event));
	CDialogEx::OnLButtonDown(nFlags, point);
}

void CRemoteDlgShow::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CPoint remote = UserPoint2RemoteScreenPoint(point);
	MOUSENV event;
	event.nAction = 2;
	event.nButton = 0;
	event.ptXY = remote;
	CRemoteClientDlg* pParent = (CRemoteClientDlg*)GetParent();
	pParent->SendMessage(WM_SEND_PACKET, 9 << 1 | 1, (LPARAM)(&event));
	CDialogEx::OnLButtonUp(nFlags, point);
}

void CRemoteDlgShow::OnMButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CDialogEx::OnMButtonDblClk(nFlags, point);
}

void CRemoteDlgShow::OnMButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CDialogEx::OnMButtonDown(nFlags, point);
}

void CRemoteDlgShow::OnMButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CDialogEx::OnMButtonUp(nFlags, point);
}

void CRemoteDlgShow::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CPoint remote = UserPoint2RemoteScreenPoint(point);
	MOUSENV event;

	//	WORD nAction; //  Click(0) dounle Click(1) up(2) move(4)
	//WORD nButton;// left(0)  right(1) mid(2)
	event.nAction = 4;
	event.nButton = 8;
	event.ptXY = remote;
	CRemoteClientDlg* pParent = (CRemoteClientDlg*)GetParent();
	pParent->SendMessage(WM_SEND_PACKET, 9 << 1 | 1,(LPARAM)(&event));
	CDialogEx::OnMouseMove(nFlags, point);
}

void CRemoteDlgShow::OnRButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	// TODO: Add your message handler code here and/or call default
	//	WORD nAction; // Click(0) double Click(1) up(2) 
	//WORD nButton;// left(0)  right(1) mid(2)
	//event.nAction = 2;
	CPoint remote = UserPoint2RemoteScreenPoint(point);
	MOUSENV event;
	event.nAction = 1;
	event.nButton = 1;
	event.ptXY = remote;
	CRemoteClientDlg* pParent = (CRemoteClientDlg*)GetParent();
	pParent->SendMessage(WM_SEND_PACKET, 9 << 1 | 1, (LPARAM)(&event));
	//CDialogEx::OnRButtonDown(nFlags, point);
	CDialogEx::OnRButtonDblClk(nFlags, point);
}
