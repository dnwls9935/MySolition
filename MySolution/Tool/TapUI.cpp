// TapUI.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Tool.h"
#include "TapUI.h"
#include "afxdialogex.h"


// TapUI 대화 상자입니다.

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


// TapUI 메시지 처리기입니다.
