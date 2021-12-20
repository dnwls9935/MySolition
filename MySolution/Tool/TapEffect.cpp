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

void TapEffect::PostNcDestroy()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	delete this;
	CDialogEx::PostNcDestroy();
}

void TapEffect::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(TapEffect, CDialogEx)
END_MESSAGE_MAP()


// TapEffect 메시지 처리기입니다.
