#pragma once
#include "afxwin.h"


// TapUI ��ȭ �����Դϴ�.

class TapUI : public CDialogEx
{
	DECLARE_DYNAMIC(TapUI)

public:
	TapUI(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~TapUI();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TAPUI };
#endif

protected:
	virtual void PostNcDestroy();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
private:

public:
	afx_msg void OnLbnSelchangeList1();
	CListBox UIListBox;
	afx_msg void OnBnClickedCheck1();
	CButton m_Batch;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	CButton m_Modify;
	afx_msg void OnBnClickedCheck2();
};
