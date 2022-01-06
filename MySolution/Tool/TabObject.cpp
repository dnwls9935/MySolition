// TabObject.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Tool.h"
#include "TabObject.h"
#include "afxdialogex.h"


// TabObject 대화 상자입니다.

IMPLEMENT_DYNAMIC(TabObject, CDialogEx)

TabObject::TabObject(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_TABOBJECT, pParent)
{

}

TabObject::~TabObject()
{
}

void TabObject::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(TabObject, CDialogEx)
END_MESSAGE_MAP()


// TabObject 메시지 처리기입니다.
