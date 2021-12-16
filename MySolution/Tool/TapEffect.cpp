// TapEffect.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Tool.h"
#include "TapEffect.h"
#include "afxdialogex.h"


// TapEffect 대화 상자입니다.

IMPLEMENT_DYNAMIC(TapEffect, CDialogEx)

TapEffect::TapEffect(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_TAPEFFECT, pParent)
{

}

TapEffect::~TapEffect()
{
}

void TapEffect::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(TapEffect, CDialogEx)
END_MESSAGE_MAP()


// TapEffect 메시지 처리기입니다.
