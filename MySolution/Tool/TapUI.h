#pragma once


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
};
