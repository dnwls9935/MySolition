// Form.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Tool.h"
#include "Form.h"
#include "TapMap.h"
#include "TapEffect.h"
#include "TapUI.h"

// Form

IMPLEMENT_DYNCREATE(Form, CFormView)

Form::Form()
	: CFormView(IDD_FORM)
{

}

Form::~Form()
{
	
}

void Form::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_tabCtrl);
}

BEGIN_MESSAGE_MAP(Form, CFormView)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &Form::OnTcnSelchangeTab1)
END_MESSAGE_MAP()


// Form 진단입니다.

#ifdef _DEBUG
void Form::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void Form::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// Form 메시지 처리기입니다.


void Form::OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	m_FocusTab = m_tabCtrl.GetCurSel();

	switch (m_FocusTab)
	{
	case 0:
		tapMap->ShowWindow(SW_SHOW);
		tapEffect->ShowWindow(SW_HIDE);
		tapUI->ShowWindow(SW_HIDE);
		break;
	case 1:
		tapMap->ShowWindow(SW_HIDE);
		tapEffect->ShowWindow(SW_SHOW);
		tapUI->ShowWindow(SW_HIDE);
		break;
	case 2:
		tapMap->ShowWindow(SW_HIDE);
		tapEffect->ShowWindow(SW_HIDE);
		tapUI->ShowWindow(SW_SHOW);
		break;
	}
	*pResult = 0;
}

HRESULT Form::ReadyTabSetting()
{
	CFormView::OnInitialUpdate();

	CSize scrollSize(0, 0);
	SetScrollSizes(MM_TEXT, scrollSize);

	CString tab1 = TEXT("Map");
	CString tab2 = TEXT("Effect");
	CString tab3 = TEXT("UI");

	m_tabCtrl.SetCurSel(0);

	m_tabCtrl.InsertItem(1, tab1);
	m_tabCtrl.InsertItem(2, tab2);
	m_tabCtrl.InsertItem(3, tab3);


	CRect rtTemp;
	m_tabCtrl.GetWindowRect(&rtTemp);

	//첫번째 탭
	tapMap = new TapMap;
	tapMap->Create(IDD_TAPMAP, &m_tabCtrl);
	tapMap->MoveWindow(5, 25, rtTemp.Width() - 10, rtTemp.Height() - 30);
	tapMap->ShowWindow(SW_SHOW);

	//두번째 탭
	tapEffect = new TapEffect;
	tapEffect->Create(IDD_TAPEFFECT, &m_tabCtrl);
	tapEffect->MoveWindow(5, 25, rtTemp.Width() - 10, rtTemp.Height() - 30);
	tapEffect->ShowWindow(SW_HIDE);

	//세번째 탭
	tapUI = new TapUI;
	tapUI->Create(IDD_TAPUI, &m_tabCtrl);
	tapUI->MoveWindow(5, 25, rtTemp.Width() - 10, rtTemp.Height() - 30);
	tapUI->ShowWindow(SW_HIDE);

	return S_OK;
}

void Form::OnInitialUpdate()
{
	if (FAILED(ReadyTabSetting()))
	{
		MSGBOX(L"Failed to Ready TabSetting");
		return;
	}
}
