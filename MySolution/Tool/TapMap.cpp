// TapMap.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Tool.h"
#include "TapMap.h"
#include "afxdialogex.h"
#include "MainFrm.h"
#include "Form.h"

// TapMap 대화 상자입니다.

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
}


BEGIN_MESSAGE_MAP(TapMap, CDialogEx)
	ON_WM_CREATE()
	ON_CONTROL_RANGE(BN_CLICKED, IDC_RADIO1, IDC_RADIO3, &TapMap::OnBnClieckRadio)
	ON_EN_CHANGE(IDC_EDIT2, &TapMap::OnEnChangeRadius)
END_MESSAGE_MAP()


void TapMap::PostNcDestroy()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	delete this;
	CDialogEx::PostNcDestroy();
}


BOOL TapMap::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
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
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialogEx::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString str;
	m_BrushRadiusText.GetWindowTextW(str);
	m_brushRadius = _ttoi(str);
}
