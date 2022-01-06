#pragma once
#include "afxcmn.h"

#include "GameInstance.h"

// Form �� ���Դϴ�.

class Form : public CFormView
{
	DECLARE_DYNCREATE(Form)

protected:
	Form();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
	virtual ~Form();

public:
	class TapMap*	tapMap = nullptr;
	class TapEffect* tapEffect = nullptr;
	class TapUI* tapUI = nullptr;
	class TabObject* tabObject = nullptr;

	_int		m_FocusTab = 0;

	CTabCtrl m_tabCtrl;

private:
	class CGameInstance*		gameInstance = nullptr;

private:
	HRESULT		ReadyTabSetting();
	
public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FORM };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult);
	virtual void OnInitialUpdate();
};


