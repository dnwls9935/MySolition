// Form.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "Tool.h"
#include "Form.h"


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
	DDX_Control(pDX, IDC_TAB1, tabCtrl);
}

BEGIN_MESSAGE_MAP(Form, CFormView)
END_MESSAGE_MAP()


// Form �����Դϴ�.

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


// Form �޽��� ó�����Դϴ�.


void Form::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	CSize scrollSize(0, 0);
	SetScrollSizes(MM_TEXT, scrollSize);

	CString tab1 = _T("Effect");

	tabCtrl.InsertItem(1, tab1);

	RECT pRcTemp;
	tabCtrl.GetWindowRect(&pRcTemp);

	// ����Ʈ��

}
