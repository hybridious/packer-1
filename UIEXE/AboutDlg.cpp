// aboutdlg.cpp : implementation of the CAboutDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "aboutdlg.h"

LRESULT CAboutDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CenterWindow(GetParent());
	WCHAR szMsg[] = 
		L"ѧϰ���̵Ĳ���\n��л��ѩ����ʱ�ڴ�ţ�ǵ����Ϸ���\n��л���°�ȫ��Ⱥ����ָ�������\n\n\t\t\tvienna\n\t\t\t2013��1��";
	SetDlgItemText(IDC_STATIC_MSG, szMsg);
	return TRUE;
}

LRESULT CAboutDlg::OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	EndDialog(wID);
	return 0;
}
