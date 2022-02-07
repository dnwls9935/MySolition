#pragma once
#include "afxwin.h"


// TapUI 대화 상자입니다.

class TapUI : public CDialogEx
{
	DECLARE_DYNAMIC(TapUI)

public:
	TapUI(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~TapUI();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TAPUI };
#endif

protected:
	virtual void PostNcDestroy();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

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
