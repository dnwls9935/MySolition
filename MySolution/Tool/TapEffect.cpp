// TapEffect.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "Tool.h"
#include "TapEffect.h"
#include "afxdialogex.h"


// TapEffect ��ȭ �����Դϴ�.

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


// TapEffect �޽��� ó�����Դϴ�.
