// TabObject.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "Tool.h"
#include "TabObject.h"
#include "afxdialogex.h"


// TabObject ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(TabObject, CDialogEx)

TabObject::TabObject(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_TABOBJECT, pParent)
{

}

TabObject::~TabObject()
{
}

void TabObject::PostNcDestroy()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	delete this;
	CDialogEx::PostNcDestroy();
}

void TabObject::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(TabObject, CDialogEx)
END_MESSAGE_MAP()


// TabObject �޽��� ó�����Դϴ�.
