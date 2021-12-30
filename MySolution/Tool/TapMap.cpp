// TapMap.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "Tool.h"
#include "TapMap.h"
#include "afxdialogex.h"
#include "MainFrm.h"
#include "Form.h"

// TapMap ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(TapMap, CDialogEx)

TapMap::TapMap(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_TAPMAP, pParent)
	, m_radioValue(0)
	, m_brushRadius(0)
{

}

TapMap::~TapMap()
{
}

void TapMap::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO1, m_radioValue);
	DDX_Text(pDX, IDC_EDIT2, m_brushRadius);
	DDX_Control(pDX, IDC_EDIT2, m_BrushRadiusText);
	DDX_Control(pDX, IDC_CHECK1, m_CullMode);
	DDX_Control(pDX, IDC_LIST1, m_objectListBox);
	DDX_Control(pDX, IDC_CHECK2, m_BatchObject);
	DDX_Control(pDX, IDC_CHECK3, m_Modify);
}


BEGIN_MESSAGE_MAP(TapMap, CDialogEx)
	ON_WM_CREATE()
	ON_CONTROL_RANGE(BN_CLICKED, IDC_RADIO1, IDC_RADIO3, &TapMap::OnBnClieckRadio)
	ON_EN_CHANGE(IDC_EDIT2, &TapMap::OnEnChangeRadius)
	ON_BN_CLICKED(IDC_CHECK2, &TapMap::OnBnClickedCheck2)
	ON_BN_CLICKED(IDC_CHECK3, &TapMap::OnBnClickedCheck3)
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

}

void TapMap::OnBnClieckRadio(UINT value)
{
	UpdateData(TRUE);

	UpdateData(FALSE);
}


void TapMap::OnEnChangeRadius()
{
	// TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
	// CDialogEx::OnInitDialog() �Լ��� ������ 
	//�ϰ� ����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ� CRichEditCtrl().SetEventMask()�� ȣ������ ������
	// �� �˸� �޽����� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.


	
	CString str;
	m_BrushRadiusText.GetWindowTextW(str);
	m_brushRadius = _ttoi(str);
}


void TapMap::OnBnClickedCheck2()
{
	if (m_BatchObject.GetCheck()) {
		m_Modify.SetCheck(FALSE);
	}
	Invalidate(FALSE);
}


void TapMap::OnBnClickedCheck3()
{
	if (m_Modify.GetCheck()) {
		m_BatchObject.SetCheck(FALSE);
	}
	Invalidate(FALSE);
}
