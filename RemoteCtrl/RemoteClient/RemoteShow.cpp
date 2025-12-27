// RemoteShow.cpp : implementation file
//

#include "pch.h"
#include "RemoteClient.h"
#include "afxdialogex.h"
#include "RemoteShow.h"


// CRemoteShow dialog

IMPLEMENT_DYNAMIC(CRemoteShow, CDialog)

CRemoteShow::CRemoteShow(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_DLG_REMOTE, pParent)
{

}

CRemoteShow::~CRemoteShow()
{
}

void CRemoteShow::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CRemoteShow, CDialog)
END_MESSAGE_MAP()


// CRemoteShow message handlers
