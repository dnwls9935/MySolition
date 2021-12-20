// TapMap.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "Tool.h"
#include "TapMap.h"
#include "afxdialogex.h"


// TapMap ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(TapMap, CDialogEx)

TapMap::TapMap(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_TAPMAP, pParent)
{

}

TapMap::~TapMap()
{
}

void TapMap::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_VerticesX);
	DDX_Control(pDX, IDC_EDIT2, m_VerticesZ);
}


BEGIN_MESSAGE_MAP(TapMap, CDialogEx)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_BUTTON1, &TapMap::OnBnClickedButton1)
END_MESSAGE_MAP()


void TapMap::PostNcDestroy()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	delete this;
	CDialogEx::PostNcDestroy();
}


BOOL TapMap::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}


int TapMap::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}


void TapMap::OnBnClickedButton1()
{
	CGameInstance* gameInstance = GET_INSTANCE(CGameInstance);

	gameInstance->FindObjct

	RELEASE_INSTANCE(CGameInstance);
}
