// TapUI.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "Tool.h"
#include "TapUI.h"
#include "afxdialogex.h"


// TapUI ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(TapUI, CDialogEx)

TapUI::TapUI(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_TAPUI, pParent)
{

}

TapUI::~TapUI()
{
}

void TapUI::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(TapUI, CDialogEx)
END_MESSAGE_MAP()


// TapUI �޽��� ó�����Դϴ�.
